#include <Precompiled.h>
#include "ParticleSystemSharedResources.h"

using namespace Direct3D;
using namespace Direct3DResource;
using namespace Particles;

Particles::ParticleSystemSharedResources::ParticleSystemSharedResources() :
	m_renderTarget(0)
{
	ZeroMemory(&m_viewport, sizeof(m_viewport));
}

Particles::ParticleSystemSharedResources::~ParticleSystemSharedResources()
{

}

bool Particles::ParticleSystemSharedResources::create(const D3DDevice &device, uint32_t targetWidth, uint32_t targetHeight) 
{
	m_viewport.Width = static_cast<float>(targetWidth);
	m_viewport.Height = static_cast<float>(targetHeight);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	m_additiveBlendState.add_target_state(0, true, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);
	m_additiveBlendState.add_target_state(1, true, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_ONE, D3D11_BLEND_ONE, 
		D3D11_BLEND_SRC_ALPHA,  D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);

	m_multiplicativeBlendState.add_target_state(0, true, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);
	m_multiplicativeBlendState.add_target_state(1, true, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_DEST_COLOR, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);

	m_transparencyBlendState.add_target_state(0, true, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);
	m_transparencyBlendState.add_target_state(1, true, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);

	m_noneBlendState.add_target_state(0, false, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_ONE, D3D11_BLEND_ONE,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);
	m_noneBlendState.add_target_state(1, false, D3D11_COLOR_WRITE_ENABLE_ALL, D3D11_BLEND_ONE, D3D11_BLEND_ONE,
		D3D11_BLEND_ONE, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD, D3D11_BLEND_OP_ADD);

	if (!m_additiveBlendState.create(device, false, true) ||
		!m_multiplicativeBlendState.create(device, false, true) ||
		!m_transparencyBlendState.create(device, false, true) ||
		!m_noneBlendState.create(device, false, false) ||
		!m_samplerState.initialize(device, D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP, 
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_COMPARISON_NEVER, 
			D3D11_FILTER_MIN_MAG_MIP_LINEAR, 1))
	{
		return false;
	}

	uint8_t positionFlags = Direct3DResource::RenderTargetFlags::ENABLE_RTV;
	positionFlags |= Direct3DResource::RenderTargetFlags::ENABLE_SRV;
	positionFlags |= Direct3DResource::RenderTargetFlags::ENABLE_UAV;
	if (!m_collisionTarget.initialize(device, targetWidth, targetHeight, positionFlags,
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 1))
	{
		return false;
	}

	XMFLOAT4 targetDimension = XMFLOAT4(m_viewport.Width, m_viewport.Height,
		1.0f / m_viewport.Width, 1.0f / m_viewport.Height);
	if (!m_targetDimensionBuffer.create(device) ||
		!m_targetDimensionBuffer.map_data(device.get_context(), &targetDimension))
	{
		return false;
	}

	return true;
}

void Particles::ParticleSystemSharedResources::begin(const D3DDevice & device, ID3D11RenderTargetView * const target)
{
	const float CLEAR_COLOR[4] = { 0,0,0,0 };
	ID3D11RenderTargetView * collisionTargetView = m_collisionTarget.get_rtv();
	device.get_context()->ClearRenderTargetView(collisionTargetView, CLEAR_COLOR);
	m_renderTarget = target;
}

const D3DBlendState * const Particles::ParticleSystemSharedResources::get_render_blend_state(ParticleSystemBlendMode blend) const
{
	switch (blend)
	{
		case BLEND_ADDITIVE: { return &m_additiveBlendState; } break;
		case BLEND_MULTIPLICATIVE: { return &m_multiplicativeBlendState; } break;
		case BLEND_NONE: { return &m_noneBlendState; } break;
		// NOTE: Alpha blend by default
		default: { return &m_transparencyBlendState; }
	}
}

const D3DSamplerState * const Particles::ParticleSystemSharedResources::get_sampler_state() const
{
	return &m_samplerState;
}

const D3DRenderTarget * const Particles::ParticleSystemSharedResources::get_collision_render_target() const
{
	return &m_collisionTarget;
}

ID3D11RenderTargetView * const Particles::ParticleSystemSharedResources::get_color_render_target() const
{
	return m_renderTarget;
}

const D3DConstantBuffer<XMFLOAT4>* const Particles::ParticleSystemSharedResources::get_target_dimension_buffer() const
{
	return &m_targetDimensionBuffer;
}

D3D11_VIEWPORT Particles::ParticleSystemSharedResources::get_viewport() const
{
	return m_viewport;
}
