#include <Precompiled.h>

#include "DeferredLightPass.h"

using namespace Rendering;
using namespace Direct3D;

Rendering::DeferredLightPass::DeferredLightPass() :
	m_directionalLightsDirty(false)
{
	ZeroMemory(&m_directionalLightData, sizeof(m_directionalLightData));
}

Rendering::DeferredLightPass::~DeferredLightPass()
{

}

bool Rendering::DeferredLightPass::initialize(const D3DDevice &device, const std::string & directionalLightShaderName)
{
	if (!m_directionalLightsShader.compile(device, directionalLightShaderName, SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL))
	{
		return false;
	}

	if (!m_samplerState.initialize(device, D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP, 
		D3D11_COMPARISON_NEVER,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR, 1))
	{
		return false; 
	}

	if (!m_directionalLightsBuffer.create(device))
	{
		return false;
	}

	if (!m_fullscreenQuad.initialize(device, m_directionalLightsShader)) 
	{
		return false; 
	}

	return true;
}

void Rendering::DeferredLightPass::add_directional_light(const XMFLOAT3 &direction, const XMFLOAT3 &intensity)
{
	uint32_t index = static_cast<uint32_t>(m_directionalLightData.lightCount.x);

	if (index < MAX_DIRECTIONAL_LIGHTS)
	{
		m_directionalLightData.lights[index].direction = direction;
		m_directionalLightData.lights[index].intensity = intensity;
		++m_directionalLightData.lightCount.x;
		m_directionalLightsDirty = true;
	}
}

void DeferredLightPass::render(const D3DDevice &device, const PerspectiveCamera &camera, const DeferredGeometryPass &deferred, const D3DRenderTarget &target)
{
	const uint32_t SRV_COUNT = 4;
	const float CLEAR_COLOR[4] = { 0, 0, 0, 1 };

	ID3D11DeviceContext * context = device.get_context();
	ID3D11RenderTargetView * renderTargetView = target.get_rtv();
	ID3D11Buffer * lightBuffer = m_directionalLightsBuffer.get_data();
	ID3D11SamplerState * samplerState = m_samplerState.get_state();
	ID3D11Buffer * matrixBuffer = camera.get_matrix_buffer();
	
	ID3D11ShaderResourceView * shaderResourceViews[SRV_COUNT] =
	{
		deferred.get_target(GBuffer::GBUFFER_DIFFUSE).get_srv(),
		deferred.get_target(GBuffer::GBUFFER_NORMAL_SPECULAR).get_srv(),
		deferred.get_depth_stencil().get_srv(),
	};
	
	if (m_directionalLightsDirty)
	{
		m_directionalLightsBuffer.map_data(context, &m_directionalLightData);
	}

	context->ClearRenderTargetView(renderTargetView, CLEAR_COLOR);
	context->OMSetRenderTargets(1, &renderTargetView, 0);
	m_directionalLightsShader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	m_directionalLightsShader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);
	context->PSSetShaderResources(0, SRV_COUNT, shaderResourceViews);
	context->PSSetSamplers(0, 1, &samplerState);
	context->VSSetConstantBuffers(0, 1, &matrixBuffer);
	context->PSSetConstantBuffers(1, 1, &lightBuffer);
	
	m_fullscreenQuad.render(device);
		
	renderTargetView = 0;
	for (uint32_t srv = 0; srv < SRV_COUNT; ++srv)
	{
		shaderResourceViews[srv] = 0;
	}

	context->OMSetRenderTargets(0, 0, 0);
	context->PSSetShaderResources(0, SRV_COUNT, shaderResourceViews);
	context->VSSetShader(0, 0, 0);
	context->PSSetShader(0, 0, 0);
}