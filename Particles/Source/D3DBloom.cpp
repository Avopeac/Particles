#include <Precompiled.h>

#include "D3DBloom.h"

using namespace Direct3D;
using namespace Direct3DResource;
using namespace PostProcessing;

PostProcessing::D3DBloom::D3DBloom() :
	m_blurStrength(1.0f),
	m_bloomFactor(0.5f),
	m_exposureTime(1.0f),
	m_blurPasses(1)
{

}

PostProcessing::D3DBloom::~D3DBloom()
{

}

void PostProcessing::D3DBloom::set_blur_passes(uint32_t passes)
{
	m_blurPasses = passes;
}

void PostProcessing::D3DBloom::set_blur_strength(float strength)
{
	m_blurStrength = strength;
}

void PostProcessing::D3DBloom::set_bloom_factor(float bloom)
{
	m_bloomFactor = bloom;
}

void PostProcessing::D3DBloom::set_exposure_factor(float exposure)
{
	m_exposureTime = exposure;
}

bool PostProcessing::D3DBloom::initialize(const D3DDevice & device, uint32_t width, uint32_t height,
	StrRef thresholdShaderName, StrRef blurShaderName, StrRef mergeShaderName) 
{
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(height);

	// NOTE: Compile the necessary shaders for the post-FX
	if (!m_thresholdShader.compile(device, thresholdShaderName, SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL) ||
		!m_blurShader.compile(device, blurShaderName, SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL) ||
		!m_mergeShader.compile(device, mergeShaderName, SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL))
	{ 
		return false; 
	}

	// NOTE: Use same vertex attributes for all shaders, just use one of the for fullscreen quad
	if (!m_fullscreenQuad.initialize(device, m_thresholdShader)) 
	{ 
		return false; 
	}

	if (!m_pSamplerState.initialize(device, D3D11_TEXTURE_ADDRESS_CLAMP, 
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_COMPARISON_NEVER, 
		D3D11_FILTER_ANISOTROPIC, 4))
	{ 
		return false; 
	}

	if (!m_blurBuffer.create(device) || !m_blendBuffer.create(device)) 
	{ 
		return false; 
	}
	
	uint8_t renderTargetFlags = RenderTargetFlags::ENABLE_SRV | RenderTargetFlags::ENABLE_RTV;
	if (!m_renderTarget0.initialize(device, width, height, renderTargetFlags, 
		DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 1) ||
		!m_renderTarget1.initialize(device, width, height, renderTargetFlags,
			DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 1))
	{
		return false;
	}

	return true;
}

void PostProcessing::D3DBloom::render(const D3DDevice & device, ID3D11RenderTargetView * const dest, const D3DRenderTarget & src)
{
	// NOTE: Set default blend state and assign texture sampler
	ID3D11DeviceContext * context = device.get_context();
	ID3D11SamplerState * sampler = m_pSamplerState.get_state();
	context->PSSetSamplers(0, 1, &sampler);
	context->OMSetBlendState(0, 0, ~0);

	// NOTE: Set new viewport, as we may downsample effect
	D3D11_VIEWPORT old_viewport = device.get_current_viewport();
	device.set_viewport(m_viewport);
	apply_threshold(device, src);
	apply_gaussian_blur(device);
	
	// NOTE: Set to old viewport, to upsample the effect
	device.set_viewport(old_viewport);
	apply_merge(device, src, dest);
}

void PostProcessing::D3DBloom::apply_threshold(const D3DDevice & device, const D3DRenderTarget & src)
{
	const float CLEAR_COLOR[4] = { 0, 0, 0, 1 };
	ID3D11DeviceContext * context = device.get_context();
	ID3D11RenderTargetView * renderTargetView = m_renderTarget0.get_rtv();
	ID3D11ShaderResourceView * shaderResourceView = src.get_srv();

	// NOTE: Set shaders and target
	context->ClearRenderTargetView(renderTargetView, CLEAR_COLOR);
	m_thresholdShader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	m_thresholdShader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);
	context->OMSetRenderTargets(1, &renderTargetView, 0);

	// NOTE: Set resources
	context->PSSetShaderResources(0, 1, &shaderResourceView);

	// NOTE: Set up input assembly for fullscreen quad
	m_fullscreenQuad.render(device);

	// NOTE: Unbind target and resources
	renderTargetView = 0;
	shaderResourceView = 0;
	context->OMSetRenderTargets(1, &renderTargetView, 0);
	context->PSSetShaderResources(0, 1, &shaderResourceView);
}

void PostProcessing::D3DBloom::apply_gaussian_blur(const D3DDevice & device)
{
	const float CLEAR_COLOR[4] = { 0,0,0,1 };
	ID3D11DeviceContext * context = device.get_context();
	ID3D11Buffer * blurDataBuffer = m_blurBuffer.get_data();

	// NOTE: Set shaders and constant buffer
	m_blurShader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	m_blurShader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);
	context->PSSetConstantBuffers(0, 1, &blurDataBuffer);

	BlurData blurData;
	blurData.size.x = m_blurStrength / static_cast<float>(m_renderTarget0.get_width());
	blurData.size.y = m_blurStrength / static_cast<float>(m_renderTarget0.get_height());

	for (uint32_t pass = 0; pass < m_blurPasses; ++pass)
	{
		ID3D11RenderTargetView * horizontalRenderTargetView = m_renderTarget0.get_rtv();
		ID3D11ShaderResourceView * horizontalShaderResourceView = m_renderTarget1.get_srv();
		ID3D11RenderTargetView * verticalRenderTargetView = m_renderTarget1.get_rtv();
		ID3D11ShaderResourceView * verticalShaderResourceView = m_renderTarget0.get_srv();

		// NOTE: Horizontal pass
		blurData.direction.x = 1.0f;
		blurData.direction.y = 0.0f;
		m_blurBuffer.map_data(context, &blurData);
		
		context->PSSetShaderResources(0, 1, &verticalShaderResourceView);
		context->OMSetRenderTargets(1, &verticalRenderTargetView, 0);

		m_fullscreenQuad.render(device);

		verticalRenderTargetView = 0;
		verticalShaderResourceView = 0;

		context->PSSetShaderResources(0, 1, &verticalShaderResourceView);
		context->OMSetRenderTargets(1, &verticalRenderTargetView, 0);

		// NOTE: Vertical pass
		blurData.direction.x = 0.0f;
		blurData.direction.y = 1.0f;
		m_blurBuffer.map_data(context, &blurData);

		context->PSSetShaderResources(0, 1, &horizontalShaderResourceView);
		context->OMSetRenderTargets(1, &horizontalRenderTargetView, 0);

		m_fullscreenQuad.render(device);

		horizontalShaderResourceView = 0;
		horizontalRenderTargetView = 0;

		context->PSSetShaderResources(0, 1, &horizontalShaderResourceView);
		context->OMSetRenderTargets(1, &horizontalRenderTargetView, 0);
	}
}

void PostProcessing::D3DBloom::apply_merge(const D3DDevice & device, const D3DRenderTarget & src, ID3D11RenderTargetView * const dest)
{
	const uint32_t NUM_SRVS = 2;
	const float CLEAR_COLOR[4] = { 0, 0, 0, 1 };
	ID3D11DeviceContext * context = device.get_context();
	ID3D11RenderTargetView * renderTargetView = dest;
	ID3D11Buffer * blendDataBuffer = m_blendBuffer.get_data();
	ID3D11ShaderResourceView * shaderResourceViews[NUM_SRVS] =
	{
		m_renderTarget1.get_srv(),
		src.get_srv(),
	};

	// NOTE: Map the blending data
	BlendFactorData blendData;
	blendData.exposure = m_exposureTime;
	blendData.factor.x = m_bloomFactor;
	blendData.factor.y = 1.0f - m_bloomFactor;
	m_blendBuffer.map_data(context, &blendData);
		
	// NOTE: Set shaders, targets and resources
	context->ClearRenderTargetView(renderTargetView, CLEAR_COLOR);
	m_mergeShader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	m_mergeShader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);
	context->OMSetRenderTargets(1, &renderTargetView, 0);
	context->PSSetShaderResources(0, NUM_SRVS, shaderResourceViews);
	context->PSSetConstantBuffers(0, 1, &blendDataBuffer);

	// NOTE: Set up input assembly for fullscreen quad
	m_fullscreenQuad.render(device);


	// NOTE: Reset resources etc.
	renderTargetView = 0;
	for (uint32_t index = 0; index < NUM_SRVS; ++index)
	{
		shaderResourceViews[index] = 0;
	}

	context->OMSetRenderTargets(1, &renderTargetView, 0);
	context->PSSetShaderResources(0, 1, shaderResourceViews);
}
