#include <Precompiled.h>

#include "D3DDevice.h"
#include "D3DDepthStencil.h"

using namespace Direct3D;

Direct3D::D3DDevice::D3DDevice(HWND window, uint32_t width, uint32_t height) :
	m_pWindow(window), 
	m_width(width), 
	m_height(height),
	m_vsync(false),
	m_fullscreen(false)
{

}

Direct3D::D3DDevice::~D3DDevice()
{
	// NOTE: Some quirk with D3D11 that doesn't release swap chain properly if fullscreen
	if (m_pSwapChain) 
	{
		m_pSwapChain->SetFullscreenState(false, 0);
	}
}

bool Direct3D::D3DDevice::initialize(bool vsync, bool fullscreen)
{
	m_vsync = vsync;
	m_fullscreen = fullscreen;
	m_currentViewport.TopLeftX = 0.0f;
	m_currentViewport.TopLeftY = 0.0f;
	m_currentViewport.MinDepth = 0.0f;
	m_currentViewport.MaxDepth = 1.0f;
	m_currentViewport.Width = static_cast<float>(m_width);
	m_currentViewport.Height = static_cast<float>(m_height);
	m_previousViewport = m_currentViewport;

	if (!create_swap_chain())
	{
		return false;
	}

	if (!create_cull_back_rs() || !create_cull_none_rs())
	{
		return false;
	}

	if (!create_depth_test_disabled() || !create_depth_test_enabled())
	{
		return false;
	}

	// NOTE: Set the initial render target
	m_pContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), 0);
	m_pContext->OMSetDepthStencilState(m_pDepthStencilStateEnabled.Get(), 1);
	// NOTE: Set the initial raster state
	m_pContext->RSSetState(m_pRasterCullBack.Get());
	m_pContext->RSSetViewports(1, &m_currentViewport);

	return true;
}

void Direct3D::D3DDevice::present()
{
	m_pSwapChain->Present(m_vsync ? 1 : 0, 0);
}

ID3D11Device * Direct3D::D3DDevice::get_device() const 
{ 
	return m_pDevice.Get();
}

ID3D11DeviceContext * Direct3D::D3DDevice::get_context() const 
{ 
	return m_pContext.Get();
}

ID3D11RenderTargetView * Direct3D::D3DDevice::get_rtv() const 
{
	return m_pRenderTargetView.Get();
}

void Direct3D::D3DDevice::set_depth_test(DepthTest test) const
{
	if (test == DepthTest::DEPTH_TEST_ON)
	{
		m_pContext->OMSetDepthStencilState(m_pDepthStencilStateEnabled.Get(), 0xFF);
	}
	else if (test == DepthTest::DEPTH_TEST_OFF)
	{
		m_pContext->OMSetDepthStencilState(m_pDepthStencilStateDisabled.Get(), 0);
	}
}

void Direct3D::D3DDevice::set_cull_mode(RasterCullMode mode) const
{
	if (mode == RasterCullMode::CULL_NONE)
	{
		m_pContext->RSSetState(m_pRasterCullNone.Get());
	}
	else if (mode == RasterCullMode::CULL_BACK)
	{
		m_pContext->RSSetState(m_pRasterCullBack.Get());
	}
}

void Direct3D::D3DDevice::set_viewport(const D3D11_VIEWPORT & viewport) const
{
	m_previousViewport = m_currentViewport;
	m_currentViewport = viewport;
	m_pContext->RSSetViewports(1, &m_currentViewport);
}

bool Direct3D::D3DDevice::is_vsync() const 
{
	return m_vsync; 
}

bool Direct3D::D3DDevice::is_fullscreen() const
{ 
	return m_fullscreen; 
}

uint32_t Direct3D::D3DDevice::get_width() const 
{ 
	return m_width; 
}

uint32_t Direct3D::D3DDevice::get_height() const 
{
	return m_height; 
}

const D3D11_VIEWPORT & Direct3D::D3DDevice::get_current_viewport() const 
{ 
	return m_currentViewport; 
}

const D3D11_VIEWPORT & Direct3D::D3DDevice::get_previous_viewport() const 
{ 
	return m_previousViewport; 
}

bool Direct3D::D3DDevice::create_swap_chain()
{
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Width = m_width;
	desc.BufferDesc.Height = m_height;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.SampleDesc.Count = 1;
	desc.OutputWindow = m_pWindow;
	desc.Windowed = !m_fullscreen;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	// NOTE: Assume vertical sync at 60 FPS
	if (m_vsync)
	{
		desc.BufferDesc.RefreshRate.Numerator = 60;
		desc.BufferDesc.RefreshRate.Denominator = 1;
	}

	UINT flags = 0;
	#if defined(DEBUG) || defined(_DEBUG)
		flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	D3D_FEATURE_LEVEL feature = D3D_FEATURE_LEVEL_11_0;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, &feature, 1, 
		D3D11_SDK_VERSION, &desc, m_pSwapChain.GetAddressOf(), m_pDevice.GetAddressOf(), 0, m_pContext.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Error creating D3D11 device and swap chain.", 0, 0);
		return false;
	}

	ComPtr<ID3D11Texture2D> buffer; 
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(buffer.GetAddressOf()));
	hr = m_pDevice->CreateRenderTargetView(buffer.Get(), 0, m_pRenderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed creating RTV for back buffer", 0, 0);
		return false;
	}

	return true;
}

bool Direct3D::D3DDevice::create_cull_none_rs()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_NONE;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	HRESULT hr = m_pDevice->CreateRasterizerState(&desc, m_pRasterCullNone.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create rasterizer state.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3D::D3DDevice::create_cull_back_rs()
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = false;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	HRESULT hr = m_pDevice->CreateRasterizerState(&desc, m_pRasterCullBack.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create rasterizer state.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3D::D3DDevice::create_depth_test_enabled()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	HRESULT hr = m_pDevice->CreateDepthStencilState(&desc, m_pDepthStencilStateEnabled.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create enabled depth stencil state.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3D::D3DDevice::create_depth_test_disabled()
{
	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D11_COMPARISON_NEVER;
	desc.StencilEnable = false;
	desc.StencilReadMask = 0x00;
	desc.StencilWriteMask = 0x00;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	HRESULT hr = m_pDevice->CreateDepthStencilState(&desc, m_pDepthStencilStateDisabled.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create disabled depth stencil state.", 0, 0);
		return false;
	}

	return true;
}
