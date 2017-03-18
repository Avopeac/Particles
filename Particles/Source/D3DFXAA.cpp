#include <Precompiled.h>
#include "D3DFXAA.h"

using namespace PostProcessing;

PostProcessing::D3DFXAA::D3DFXAA()
{
}

PostProcessing::D3DFXAA::~D3DFXAA()
{
}

bool PostProcessing::D3DFXAA::initialize(const D3DDevice & device, const std::string & fxaaShaderName)
{
	if (!m_fxaaShader.compile(device, fxaaShaderName, SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL) ||
		!m_fullscreenQuad.initialize(device, m_fxaaShader))
	{
		return false;
	}

	/*if (!m_pSamplerState.initialize(device, D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_NEVER,
		D3D11_FILTER_ANISOTROPIC, 4))
	{
		return false;
	}*/

	if (!m_pSamplerState.initialize(device, D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_NEVER,
		D3D11_FILTER_MIN_MAG_MIP_LINEAR, 1))
	{
		return false;
	}

	return true;
}

void PostProcessing::D3DFXAA::render(const D3DDevice & device, ID3D11RenderTargetView * dest, const D3DRenderTarget & src)
{
	ID3D11DeviceContext * context = device.get_context();
	ID3D11RenderTargetView * renderTargetView = dest;
	ID3D11ShaderResourceView * shaderResourceView = src.get_srv();
	ID3D11SamplerState * samplerState = m_pSamplerState.get_state();

	// NOTE: Set resources, shaders etc.
	m_fxaaShader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	m_fxaaShader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);
	context->OMSetRenderTargets(1, &renderTargetView, 0);
	context->PSSetShaderResources(0, 1, &shaderResourceView);
	context->PSSetSamplers(0, 1, &samplerState);

	// NOTE: Set up input assembly for quad
	m_fullscreenQuad.render(device);

	// NOTE: Reset resources etc. 
	shaderResourceView = 0;
	renderTargetView = 0;
	context->PSSetShaderResources(0, 1, &shaderResourceView);
	context->OMSetRenderTargets(1, &renderTargetView, 0);
}
