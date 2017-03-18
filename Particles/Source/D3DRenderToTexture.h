#pragma once
#include "D3DRenderTarget.h"
#include "D3DFullscreenQuad.h"
#include "D3DSamplerState.h"
#include "D3DShader.h"

namespace PostProcessing
{
	using namespace Direct3D;
	using namespace Direct3DResource;

	class D3DRenderToTexture
	{
		D3DFullscreenQuad m_fullscreenQuad;
		D3DSamplerState m_pSamplerState;
		D3DShader m_passthroughShader;

	public:
		D3DRenderToTexture();
		~D3DRenderToTexture();
		D3DRenderToTexture(const D3DRenderToTexture &) = delete;
		D3DRenderToTexture(D3DRenderToTexture &&) = delete;
		D3DRenderToTexture &operator=(const D3DRenderToTexture &) = delete;
		D3DRenderToTexture &operator=(D3DRenderToTexture &&) = delete;

		bool initialize(const D3DDevice &device, const std::string &passthroughShaderName);

		void render(const D3DDevice &device, ID3D11RenderTargetView * dest, const D3DRenderTarget &src);

	private:
				
	};
}
