#pragma once

#include "D3DRenderTarget.h"
#include "D3DBlendState.h"
#include "D3DSamplerState.h"
#include "D3DFullscreenQuad.h"
#include "D3DConstantBuffer.h"

namespace Particles
{
	using namespace Direct3DResource;
	using namespace PostProcessing;

	enum ParticleSystemBlendMode
	{
		BLEND_ADDITIVE,
		BLEND_MULTIPLICATIVE,
		BLEND_TRANSPARENT,
		BLEND_NONE,
	};

	class ParticleSystemSharedResources
	{
		D3DRenderTarget m_collisionTarget;
		ID3D11RenderTargetView * m_renderTarget;
		D3DBlendState m_additiveBlendState;
		D3DBlendState m_multiplicativeBlendState;
		D3DBlendState m_transparencyBlendState;
		D3DBlendState m_noneBlendState;

		D3DSamplerState m_samplerState;
		D3D11_VIEWPORT m_viewport;
		D3DConstantBuffer<XMFLOAT4> m_targetDimensionBuffer;

	public:
		ParticleSystemSharedResources();
		~ParticleSystemSharedResources();
		ParticleSystemSharedResources(const ParticleSystemSharedResources &) = delete;
		ParticleSystemSharedResources(ParticleSystemSharedResources &&) = delete;
		ParticleSystemSharedResources &operator=(const ParticleSystemSharedResources &) = delete;
		ParticleSystemSharedResources &operator=(ParticleSystemSharedResources &&) = delete;

		bool create(const D3DDevice &device, uint32_t width, uint32_t height);
		void begin(const D3DDevice &device, ID3D11RenderTargetView * const target);

		const D3DBlendState * const get_render_blend_state(ParticleSystemBlendMode blend) const;
		const D3DSamplerState * const get_sampler_state() const;
		const D3DRenderTarget * const get_collision_render_target() const;
		ID3D11RenderTargetView * const get_color_render_target() const;

		const D3DConstantBuffer<XMFLOAT4> * const get_target_dimension_buffer() const;
		D3D11_VIEWPORT get_viewport() const;

	protected:

	private:

	};
}

