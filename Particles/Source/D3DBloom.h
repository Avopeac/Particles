#pragma once
#include "D3DShader.h"
#include "D3DRenderTarget.h"
#include "D3DSamplerState.h"
#include "D3DFullscreenQuad.h"
#include "D3DConstantBuffer.h"

namespace PostProcessing
{
	using namespace DirectX;
	using namespace Direct3D;
	using namespace Direct3DResource;

	struct BlurData 
	{
		XMFLOAT2 direction;
		XMFLOAT2 size;
	};

	struct BlendFactorData 
	{
		XMFLOAT2 factor;
		float exposure;
		float padding;
	};

	class D3DBloom 
	{
		float m_blurStrength;
		float m_bloomFactor;
		float m_exposureTime;
		uint32_t m_blurPasses;

		D3DShader m_thresholdShader;
		D3DShader m_blurShader;
		D3DShader m_mergeShader;
		D3DSamplerState m_pSamplerState;
		D3DRenderTarget m_renderTarget0;
		D3DRenderTarget m_renderTarget1;
		D3DFullscreenQuad m_fullscreenQuad;
		D3DConstantBuffer<BlurData> m_blurBuffer;
		D3DConstantBuffer<BlendFactorData> m_blendBuffer;
		D3D11_VIEWPORT m_viewport;

	public:
		using Str = std::string;
		using StrRef = const Str &;

		D3DBloom();
		~D3DBloom();
		D3DBloom(const D3DBloom &) = delete;
		D3DBloom(D3DBloom &&) = delete;
		D3DBloom &operator=(const D3DBloom &) = delete;
		D3DBloom &operator=(D3DBloom &&) = delete;

		bool initialize(const D3DDevice &device, uint32_t width, uint32_t height, 
			StrRef thresholdShaderName, StrRef blurShaderName, StrRef mergeShaderName);

		void render(const D3DDevice &device, ID3D11RenderTargetView * const dest, const D3DRenderTarget &src);

		void set_blur_passes(uint32_t passes);

		// NOTE: The blur strength (how many texels to offset)
		void set_blur_strength(float strength);

		// NOTE: The bloom factor [DestinationTexture = SourceTexture * (1.0 - BloomFactor) + BloomTexture * BloomFactor] 
		void set_bloom_factor(float bloom);

		// NOTE: The exposure factor for HDR
		void set_exposure_factor(float exposure);

	private:
		
		void apply_threshold(const D3DDevice &device, const D3DRenderTarget &src);
		void apply_gaussian_blur(const D3DDevice &device);
		void apply_merge(const D3DDevice &device, const D3DRenderTarget &src, ID3D11RenderTargetView * const dest);
	};
}
