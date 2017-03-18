#pragma once

#include "D3DDevice.h"

namespace Direct3DResource
{
	using namespace Direct3D;

	class D3DDepthStencil 
	{
		uint32_t m_width;
		uint32_t m_height;
		DXGI_FORMAT m_textureFormat;
		DXGI_FORMAT m_shaderResourceViewFormat;
		DXGI_FORMAT m_depthStencilViewFormat;
		ComPtr<ID3D11Texture2D> m_pTexture;
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

	public:
		D3DDepthStencil();
		~D3DDepthStencil();
		D3DDepthStencil(const D3DDepthStencil &) = delete;
		D3DDepthStencil(D3DDepthStencil &&) = delete;
		D3DDepthStencil &operator=(const D3DDepthStencil &) = delete;
		D3DDepthStencil &operator=(D3DDepthStencil &&) = delete;

		// NOTE: Returns the SRV for the DSV
		ID3D11ShaderResourceView * get_srv() const;

		// NOTE: Returns the DSV
		ID3D11DepthStencilView * get_dsv() const;

		// NOTE: Initialize the depth stencil view
		bool initialize(const D3DDevice &device, uint32_t width, uint32_t height,
			DXGI_FORMAT textureFormat, DXGI_FORMAT shaderResourceViewFormat, DXGI_FORMAT depthStencilViewFormat);

	private:
		bool create_texture(const D3DDevice &device);
		bool create_srv(const D3DDevice &device);
		bool create_dsv(const D3DDevice &device);
	};
}
