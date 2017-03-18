#pragma once

#include "D3DDevice.h"

namespace Direct3DResource
{
	using namespace Direct3D;

	enum RenderTargetFlags
	{
		ENABLE_SRV = 0x01,
		ENABLE_RTV = 0x02,
		ENABLE_UAV = 0x04,
	};

	class D3DRenderTarget 
	{
		uint8_t m_flags;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_mips = 1;
		DXGI_FORMAT m_textureFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT m_shaderResourceViewFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT m_renderTargetViewFormat = DXGI_FORMAT_UNKNOWN;
		DXGI_FORMAT m_unorderedAccessViewFormat = DXGI_FORMAT_UNKNOWN;
		ComPtr<ID3D11Texture2D> m_pTexture = 0;
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = 0;
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView = 0;
		ComPtr<ID3D11UnorderedAccessView> m_pUnorderedAccessView = 0;

	public:
		D3DRenderTarget();
		~D3DRenderTarget() = default;
		D3DRenderTarget(const D3DRenderTarget &) = default;
		D3DRenderTarget(D3DRenderTarget &&) = default;
		D3DRenderTarget &operator=(const D3DRenderTarget &) = default;
		D3DRenderTarget &operator=(D3DRenderTarget &&) = default;

		// NOTE: Returns the SRV if present
		ID3D11ShaderResourceView * get_srv() const;

		// NOTE: Returns the RTV if present
		ID3D11RenderTargetView * get_rtv() const;

		// NOTE: Returns the UAV if present
		ID3D11UnorderedAccessView * get_uav() const;

		// NOTE: Returns the underlying texture
		ID3D11Texture2D * get_texture() const;

		// NOTE: Returns the width of the target
		uint32_t get_width() const;

		// NOTE: Returns the height of the target
		uint32_t get_height() const;

		// NOTE: Create the render target with the given formats
		bool initialize(const D3DDevice &device, 
			uint32_t width, uint32_t height, uint8_t flags, 
			DXGI_FORMAT textureFormat, 
			DXGI_FORMAT shaderResourceViewFormat, 
			DXGI_FORMAT renderTargetViewFormat, 
			DXGI_FORMAT unorderedAccessViewFormat, 
			uint32_t mips);

	private:
		bool create_srv(const Direct3D::D3DDevice &device);
		bool create_rtv(const Direct3D::D3DDevice &device);
		bool create_uav(const Direct3D::D3DDevice &device);
		bool create_texture(const Direct3D::D3DDevice &device);
	};
}
