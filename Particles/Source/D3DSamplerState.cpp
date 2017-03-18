#include <Precompiled.h>

#include "D3DSamplerState.h"

using namespace Direct3D;
using namespace Direct3DResource;

D3DSamplerState::D3DSamplerState() 
{

}

D3DSamplerState::~D3DSamplerState() 
{

}

ID3D11SamplerState * Direct3DResource::D3DSamplerState::get_state() const
{
	return m_pSamplerState.Get();
}

bool D3DSamplerState::initialize(const D3DDevice &device, 
	D3D11_TEXTURE_ADDRESS_MODE u, D3D11_TEXTURE_ADDRESS_MODE v, D3D11_TEXTURE_ADDRESS_MODE w, 
	D3D11_COMPARISON_FUNC compFunc, D3D11_FILTER filter, uint8_t maxAnisotropy)
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = u;
	desc.AddressV = v;
	desc.AddressW = w;
	desc.ComparisonFunc = compFunc;
	desc.Filter = filter;
	desc.MaxAnisotropy = maxAnisotropy;
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = device.get_device()->CreateSamplerState(&desc, m_pSamplerState.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create sampler state.", 0, 0);
		return false;
	}

	return true;
}
