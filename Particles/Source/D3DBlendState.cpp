#include <Precompiled.h>

#include "D3DBlendState.h"

using namespace Direct3DResource;

Direct3DResource::D3DBlendState::D3DBlendState()
{
	ZeroMemory(&m_description, sizeof(m_description));
}

Direct3DResource::D3DBlendState::~D3DBlendState()
{

}

bool Direct3DResource::D3DBlendState::create(const D3DDevice & device, bool alphaToCoverage, bool independentBlend)
{
	m_description.AlphaToCoverageEnable = alphaToCoverage;
	m_description.IndependentBlendEnable = independentBlend;

	HRESULT hr = device.get_device()->CreateBlendState(&m_description, m_pBlendState.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Error creating D3D11 blend state.", 0, 0);
		return false;
	}

	return true;
}

void Direct3DResource::D3DBlendState::add_target_state(uint8_t target, bool blendEnabled, uint8_t writeMask, D3D11_BLEND srcBlend, D3D11_BLEND destBlend,
	D3D11_BLEND srcAlphaBlend, D3D11_BLEND destAlphaBlend, D3D11_BLEND_OP blendOp, D3D11_BLEND_OP blendOpAlpha)
{
	m_description.RenderTarget[target].BlendEnable = blendEnabled;
	m_description.RenderTarget[target].BlendOp = blendOp;
	m_description.RenderTarget[target].BlendOpAlpha = blendOpAlpha;
	m_description.RenderTarget[target].SrcBlend = srcBlend;
	m_description.RenderTarget[target].SrcBlendAlpha = srcAlphaBlend;
	m_description.RenderTarget[target].DestBlend = destBlend;
	m_description.RenderTarget[target].DestBlendAlpha = destAlphaBlend;
	m_description.RenderTarget[target].RenderTargetWriteMask = writeMask;
}

ID3D11BlendState * Direct3DResource::D3DBlendState::get_blend_state() const
{
	return m_pBlendState.Get();
}
