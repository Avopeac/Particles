#include <Precompiled.h>

#include "MeshTexture.h"

using namespace Mesh;

Mesh::MeshTexture::MeshTexture() :
	m_texturePathHash(0), 
	m_loaded(false),
	m_texturePath(""),
	m_supportedFileType(TextureFileType::TEXTURE_FILE_UNKNOWN)
{

}

Mesh::MeshTexture::~MeshTexture()
{

}

bool Mesh::MeshTexture::load(const D3DDevice &device)
{
	// NOTE: Use DirectXTeX to load image files
	DirectX::ScratchImage scratch;
	DirectX::TexMetadata meta;

	// NOTE: DirectXTeX needs wide char strings
	std::wstring fileName = string_to_wchar_t(m_texturePath);

	// NOTE: Load image based on the set filetype
	HRESULT hr = 0;
	switch (m_supportedFileType)
	{
		default: 

		case TEXTURE_FILE_WIC:
		{
			hr = DirectX::LoadFromWICFile(fileName.c_str(), DirectX::CP_FLAGS_NONE, &meta, scratch, 0);
		}
		break;

		case TEXTURE_FILE_DDS:
		{
			hr = DirectX::LoadFromDDSFile(fileName.c_str(), DirectX::CP_FLAGS_NONE, &meta, scratch);
		}
		break;

		case TEXTURE_FILE_TGA:
		{
			hr = DirectX::LoadFromTGAFile(fileName.c_str(), &meta, scratch);
		}
		break;

		case TEXTURE_FILE_UNKNOWN:
		{
			--hr;
		}
		break;
	}

	std::string error = "Failed to load image from file at path: " + m_texturePath;
	// NOTE: Failed to load the image file
	if (FAILED(hr))
	{
		MessageBoxA(0, error.c_str(), 0, 0);
		return false;
	}

	hr = DirectX::CreateTexture(device.get_device(), scratch.GetImages(), scratch.GetImageCount(), meta, m_pResource.GetAddressOf());
	// NOTE: Failed to create texture resource
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create texture resource.", 0, 0);
		return false;
	}

	hr = DirectX::CreateShaderResourceView(device.get_device(), scratch.GetImages(), scratch.GetImageCount(), meta, m_pShaderResourceView.GetAddressOf());
	// NOTE: Failed to create shader resource view
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create shader resource view for texture.", 0, 0);
		return false;
	}

	m_textureSize = XMFLOAT4((float)meta.width, (float)meta.height, 1.0f / (float)meta.width, 1.0f / (float)meta.height);

	// NOTE: Texture is loaded successfully
	m_loaded = true;
	return m_loaded;
}

void Mesh::MeshTexture::set_path(const std::string & path)
{
	m_texturePath = path;
	m_texturePathHash = std::hash<std::string>{}(path);
}

void Mesh::MeshTexture::set_filetype(TextureFileType filetype)
{
	m_supportedFileType = filetype;
}

bool Mesh::MeshTexture::is_loaded() const
{
	return m_loaded;
}

size_t Mesh::MeshTexture::get_hash() const
{
	return m_texturePathHash;
}

const XMFLOAT4 & Mesh::MeshTexture::get_size() const
{
	return m_textureSize;
}

const std::string & Mesh::MeshTexture::get_path() const
{
	return m_texturePath;
}

ID3D11ShaderResourceView * Mesh::MeshTexture::get_srv() const
{
	return m_pShaderResourceView.Get();
}

std::wstring Mesh::MeshTexture::string_to_wchar_t(const std::string & str)
{
	std::wstring temp(str.length(), L' ');
	std::copy(str.begin(), str.end(), temp.begin());
	return temp;
}
