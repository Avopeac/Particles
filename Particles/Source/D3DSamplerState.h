#pragma once

#include "D3DDevice.h"

namespace Direct3DResource 
{
	using namespace Direct3D;

	class D3DSamplerState 
	{
	public:
		D3DSamplerState();
		~D3DSamplerState();
		D3DSamplerState(const D3DSamplerState &) = delete;

		ID3D11SamplerState * get_state() const;
		bool initialize(const D3DDevice &device,
			D3D11_TEXTURE_ADDRESS_MODE u, D3D11_TEXTURE_ADDRESS_MODE v, D3D11_TEXTURE_ADDRESS_MODE w,
			D3D11_COMPARISON_FUNC compFunc, D3D11_FILTER filter, uint8_t maxAnisotropy);

	private:
		ComPtr<ID3D11SamplerState> m_pSamplerState = 0;
	};
}

