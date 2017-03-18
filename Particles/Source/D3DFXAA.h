#pragma once
#include "D3DRenderTarget.h"
#include "D3DFullscreenQuad.h"
#include "D3DSamplerState.h"
#include "D3DShader.h"

namespace PostProcessing
{
	using namespace Direct3D;
	using namespace Direct3DResource;

	class D3DFXAA
	{
		D3DFullscreenQuad m_fullscreenQuad;
		D3DSamplerState m_pSamplerState;
		D3DShader m_fxaaShader;

	public:
		D3DFXAA();
		~D3DFXAA();
		D3DFXAA(const D3DFXAA &) = delete;
		D3DFXAA(D3DFXAA &&) = delete;
		D3DFXAA &operator=(const D3DFXAA &) = delete;
		D3DFXAA &operator=(D3DFXAA &&) = delete;

		bool initialize(const D3DDevice &device, const std::string &fxaaShaderName);

		void render(const D3DDevice &device, ID3D11RenderTargetView * const dest, const D3DRenderTarget &src);

	private:

	};
}

