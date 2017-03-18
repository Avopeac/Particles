#include <Precompiled.h>

#include "D3DRenderTarget.h"

using namespace Direct3D;
using namespace Direct3DResource;

Direct3DResource::D3DRenderTarget::D3DRenderTarget() :
	m_flags(0),
	m_width(0),
	m_height(0),
	m_mips(1),
	m_textureFormat(DXGI_FORMAT_UNKNOWN),
	m_shaderResourceViewFormat(DXGI_FORMAT_UNKNOWN),
	m_renderTargetViewFormat(DXGI_FORMAT_UNKNOWN),
	m_unorderedAccessViewFormat(DXGI_FORMAT_UNKNOWN)
{

}

bool Direct3DResource::D3DRenderTarget::initialize(const Direct3D::D3DDevice &device, uint32_t width, uint32_t height, uint8_t flags,
	DXGI_FORMAT textureFormat, DXGI_FORMAT shaderResourceViewFormat, 
	DXGI_FORMAT renderTargetViewFormat, DXGI_FORMAT unorderedAccessViewFormat, 
	uint32_t mips)
{
	m_width = width;
	m_height = height;
	m_flags = flags;
	m_textureFormat = textureFormat;
	m_shaderResourceViewFormat = shaderResourceViewFormat;
	m_renderTargetViewFormat = renderTargetViewFormat;
	m_unorderedAccessViewFormat = unorderedAccessViewFormat;
	m_mips = mips;

	if (!create_texture(device)) 
	{
		return false; 
	}

	if (flags & RenderTargetFlags::ENABLE_RTV)
	{
		if (!create_rtv(device))
		{
			return false;
		}
	}
	
	if (flags & RenderTargetFlags::ENABLE_SRV)
	{
		if (!create_srv(device))
		{
			return false;
		}
	}
	
	if (flags & RenderTargetFlags::ENABLE_UAV)
	{
		if (!create_uav(device))
		{
			return false;
		}
	}
			 
	return true;
}

bool Direct3DResource::D3DRenderTarget::create_texture(const Direct3D::D3DDevice &device)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ArraySize = 1;

	desc.BindFlags = 0;
	if (m_flags & RenderTargetFlags::ENABLE_RTV) 
	{
		desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
	}

	if (m_flags & RenderTargetFlags::ENABLE_SRV)
	{
		desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
	}

	if (m_flags & RenderTargetFlags::ENABLE_UAV)
	{
		desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
	}

	desc.Format = m_textureFormat;
	desc.Height = m_height;
	desc.Width = m_width;
	desc.MiscFlags = m_mips > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	desc.MipLevels = m_mips;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	HRESULT hr = device.get_device()->CreateTexture2D(&desc, 0, m_pTexture.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create texture for render target.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DRenderTarget::create_srv(const Direct3D::D3DDevice &device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_shaderResourceViewFormat;
	desc.Texture2D.MipLevels = m_mips;
	desc.Texture2D.MostDetailedMip = 0;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	HRESULT hr = device.get_device()->CreateShaderResourceView(m_pTexture.Get(), &desc, m_pShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create shader resource for render target.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DRenderTarget::create_rtv(const Direct3D::D3DDevice &device)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_renderTargetViewFormat;
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	HRESULT hr = device.get_device()->CreateRenderTargetView(m_pTexture.Get(), &desc, m_pRenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create render target view for render target.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DRenderTarget::create_uav(const Direct3D::D3DDevice &device)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_unorderedAccessViewFormat;
	desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D; 
	HRESULT hr = device.get_device()->CreateUnorderedAccessView(m_pTexture.Get(), &desc, m_pUnorderedAccessView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create unordered access view for render target.", 0, 0);
		return false;
	}

	return true;
}

ID3D11ShaderResourceView * Direct3DResource::D3DRenderTarget::get_srv() const
{
	return m_pShaderResourceView.Get();
}

ID3D11RenderTargetView * Direct3DResource::D3DRenderTarget::get_rtv() const
{
	return m_pRenderTargetView.Get();
}

ID3D11UnorderedAccessView * Direct3DResource::D3DRenderTarget::get_uav() const
{
	return m_pUnorderedAccessView.Get();
}

ID3D11Texture2D * Direct3DResource::D3DRenderTarget::get_texture() const
{
	return m_pTexture.Get();
}

uint32_t Direct3DResource::D3DRenderTarget::get_width() const
{
	return m_width;
}

uint32_t Direct3DResource::D3DRenderTarget::get_height() const
{
	return m_height;
}
