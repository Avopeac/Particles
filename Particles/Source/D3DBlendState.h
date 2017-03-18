#pragma once

#include "D3DDevice.h"

namespace Direct3DResource
{
	using namespace Direct3D;

	class D3DBlendState
	{
		D3D11_BLEND_DESC m_description;
		ComPtr<ID3D11BlendState> m_pBlendState;

	public:

		D3DBlendState();
		~D3DBlendState();
		D3DBlendState(const D3DBlendState &) = delete;
		D3DBlendState(D3DBlendState &&) = delete;
		D3DBlendState &operator=(const D3DBlendState &) = delete;
		D3DBlendState &operator=(D3DBlendState &&) = delete;
		
		bool create(const D3DDevice &device, bool alphaToCoverage, bool independentBlend);

		void add_target_state(uint8_t target, bool blendEnabled, uint8_t writeMask, D3D11_BLEND srcBlend, D3D11_BLEND destBlend,
			D3D11_BLEND srcAlphaBlend, D3D11_BLEND destAlphaBlend, D3D11_BLEND_OP blendOp, D3D11_BLEND_OP blendOpAlpha);

		ID3D11BlendState * get_blend_state() const;

	protected:

	private:

	};
}
