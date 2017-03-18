#include <Precompiled.h>
#include "ParticleSystemTextureArray.h"

using namespace Particles;

Particles::ParticleSystemTextureArray::ParticleSystemTextureArray(uint32_t width, uint32_t height) :
	m_width(width),
	m_height(height)
{

}

Particles::ParticleSystemTextureArray::~ParticleSystemTextureArray()
{

}

ID3D11ShaderResourceView * Particles::ParticleSystemTextureArray::get_srv() const
{
	return m_pTextureShaderResourceView.Get();
}

void Particles::ParticleSystemTextureArray::add(const std::string & path, TextureFileType type)
{
	TexturePair pair;
	pair.path = path;
	pair.type = type;
	bool found = false;
	size_t hash = std::hash<std::string>{}(pair.path);

	for (uint32_t index = 0; index < m_hashes.size(); ++index)
	{
		if (m_hashes[index] == hash)
		{
			found = true;
			break;
		}
	}

	if (!found)
	{
		m_hashes.push_back(hash);
		m_textures.push_back(pair);
	}
}

bool Particles::ParticleSystemTextureArray::load(const D3DDevice &device)
{

	DirectX::ScratchImage * scratches = new DirectX::ScratchImage[m_textures.size()];
	DirectX::TexMetadata * meta = new DirectX::TexMetadata[m_textures.size()];
	D3D11_SUBRESOURCE_DATA * data = new D3D11_SUBRESOURCE_DATA[m_textures.size()];

	bool success = true;
	uint32_t index = 0;
	for (auto it : m_textures)
	{
		std::wstring filePath = string_to_wchar_t(it.path);
		switch (it.type)
		{
			case TextureFileType::TEXTURE_FILE_DDS: { DirectX::LoadFromDDSFile(filePath.c_str(), WIC_FLAGS_NO_16BPP, &meta[index], scratches[index]); } break;
			case TextureFileType::TEXTURE_FILE_WIC: { DirectX::LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NO_16BPP, &meta[index], scratches[index]); } break;
			case TextureFileType::TEXTURE_FILE_TGA: { DirectX::LoadFromTGAFile(filePath.c_str(), &meta[index], scratches[index]); } break;
			default: { success = false; }
		}
		
		if (meta[index].width != m_width || meta[index].height != m_height)
		{
			success = false;
			break;
		}

		if (!scratches[index].OverrideFormat(DXGI_FORMAT_R8G8B8A8_UNORM))
		{
			success = false;
			break;
		}

		const DirectX::Image * image = scratches[index].GetImage(0,0,0);
		data[index].pSysMem = scratches[index].GetPixels();
		data[index].SysMemPitch = static_cast<uint32_t>(image->rowPitch);
		data[index].SysMemSlicePitch = static_cast<uint32_t>(image->slicePitch);
		++index;
	}

	if (!success)
	{
		return false;
	}

	// NOTE: Create texture array
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ArraySize = static_cast<uint32_t>(m_textures.size());
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Height = m_width;
		desc.Width = m_height;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hr = device.get_device()->CreateTexture2D(&desc, data, m_pTexture.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxA(0, "Failed to create particle textures.", 0, 0);
			return false;
		}
	}

	// NOTE: Create texture array SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Texture2DArray.ArraySize = static_cast<uint32_t>(m_textures.size());
		desc.Texture2DArray.MipLevels = 1;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

		HRESULT hr = device.get_device()->CreateShaderResourceView(m_pTexture.Get(), &desc, m_pTextureShaderResourceView.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxA(0, "Failed to create particle texture srv.", 0, 0);
			return false;
		}
	}

	delete[] scratches;
	delete[] meta;
	delete[] data;

	return true;
}

int32_t Particles::ParticleSystemTextureArray::find(const std::string & path) const
{
	size_t hash = std::hash<std::string>{}(path);
	for (int32_t index = 0; index < m_textures.size(); ++index)
	{
		if (hash == m_hashes[index])
		{
			return index;
		}
	}

	return -1;
}

std::wstring Particles::ParticleSystemTextureArray::string_to_wchar_t(const std::string & str)
{
	std::wstring temp(str.length(), L' ');
	std::copy(str.begin(), str.end(), temp.begin());
	return temp;
}
