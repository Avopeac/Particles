#pragma once

namespace Direct3D 
{
	using namespace Microsoft::WRL;

	enum RasterCullMode { CULL_NONE, CULL_BACK };
	enum DepthTest { DEPTH_TEST_ON, DEPTH_TEST_OFF };

	class D3DDevice 
	{
		bool m_vsync;
		bool m_fullscreen;
		uint32_t m_height;
		uint32_t m_width;
		HWND m_pWindow;
		ComPtr<IDXGISwapChain> m_pSwapChain;
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pContext;
		ComPtr<ID3D11RasterizerState> m_pRasterCullNone;
		ComPtr<ID3D11RasterizerState> m_pRasterCullBack;
		ComPtr<ID3D11DepthStencilState> m_pDepthStencilStateEnabled;
		ComPtr<ID3D11DepthStencilState> m_pDepthStencilStateDisabled;
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		mutable D3D11_VIEWPORT m_currentViewport;
		mutable D3D11_VIEWPORT m_previousViewport;

	public:
		D3DDevice(HWND window, uint32_t width, uint32_t height);
		~D3DDevice();
		D3DDevice(const D3DDevice &) = delete;
		D3DDevice(D3DDevice &&) = delete;
		D3DDevice &operator=(const D3DDevice &) = delete;
		D3DDevice &operator=(D3DDevice &&) = delete;

		// NOTE: Set the state of the depth (z) testing
		void set_depth_test(DepthTest test) const;

		// NOTE: Set the current culling mode to be used by the rasterizer
		void set_cull_mode(RasterCullMode mode) const;

		// NOTE: Set the current viewport to be used by the rasterizer
		void set_viewport(const D3D11_VIEWPORT &viewport) const;

		// NOTE: Presents the back buffer to the screen
		void present();

		// NOTE: Initializes the device
		bool initialize(bool vsync, bool fullscreen);

		// NOTE: Returns true if vertical sync is on
		bool is_vsync() const;

		// NOTE: Returns true if the window is fullscreen
		bool is_fullscreen() const;

		// NOTE: Returns the width of the window
		uint32_t get_width() const;

		// NOTE: Returns the height of the window
		uint32_t get_height() const;

		// NOTE: Get the underlying D3D11 device
		ID3D11Device * get_device() const;

		// NOTE: Get the underlying D3D11 immediate device context
		ID3D11DeviceContext * get_context() const;

		// NOTE: Get the RTV for the back buffer
		ID3D11RenderTargetView * get_rtv() const;

		// NOTE: Get the current viewport set in the rasterizer
		const D3D11_VIEWPORT &get_current_viewport() const;

		// NOTE: Get the previous viewport that was set in the rasterizer
		const D3D11_VIEWPORT &get_previous_viewport() const;

	private:
		bool create_swap_chain();
		bool create_cull_none_rs();
		bool create_cull_back_rs();
		bool create_depth_test_enabled();
		bool create_depth_test_disabled();
	};
}
