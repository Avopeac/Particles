#include <Precompiled.h>

#include "D3DDepthStencil.h"

using namespace Direct3D;
using namespace Direct3DResource;

Direct3DResource::D3DDepthStencil::D3DDepthStencil() :
	m_width(0),
	m_height(0),
	m_textureFormat(DXGI_FORMAT_UNKNOWN),
	m_shaderResourceViewFormat(DXGI_FORMAT_UNKNOWN),
	m_depthStencilViewFormat(DXGI_FORMAT_UNKNOWN)
{

}

Direct3DResource::D3DDepthStencil::~D3DDepthStencil()
{

}

ID3D11ShaderResourceView * Direct3DResource::D3DDepthStencil::get_srv() const
{
	return m_pShaderResourceView.Get();
}

ID3D11DepthStencilView * Direct3DResource::D3DDepthStencil::get_dsv() const
{
	return m_pDepthStencilView.Get();
}

bool Direct3DResource::D3DDepthStencil::initialize(const D3DDevice &device, uint32_t width, uint32_t height, DXGI_FORMAT tex_format, DXGI_FORMAT srv_format, DXGI_FORMAT dsv_format)
{
	m_width = width;
	m_height = height;
	m_textureFormat = tex_format;
	m_shaderResourceViewFormat = srv_format;
	m_depthStencilViewFormat = dsv_format;

	if (!create_texture(device) || !create_srv(device) || !create_dsv(device))
	{
		return false;
	}

	return true;
}

bool Direct3DResource::D3DDepthStencil::create_texture(const D3DDevice &device)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = m_textureFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	HRESULT hr = device.get_device()->CreateTexture2D(&desc, 0, m_pTexture.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create texture for depth stencil.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DDepthStencil::create_srv(const D3DDevice &device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels = -1;
	HRESULT hr = device.get_device()->CreateShaderResourceView(m_pTexture.Get(), &desc, m_pShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create shader resource for depth stencil.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DDepthStencil::create_dsv(const D3DDevice &device)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_depthStencilViewFormat;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	HRESULT hr = device.get_device()->CreateDepthStencilView(m_pTexture.Get(), &desc, m_pDepthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create depth stencil view.", 0, 0);
		return false;
	}

	return true;
}
