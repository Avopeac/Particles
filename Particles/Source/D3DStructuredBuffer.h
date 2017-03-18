#pragma once

#include "D3DDevice.h"

namespace Direct3DResource 
{
	using namespace Direct3D;

	class D3DStructuredBuffer 
	{
		DXGI_FORMAT m_bufferFormat;
		bool m_hasCounter;
		bool m_hasAppend;
		uint32_t m_elementCount;
		uint32_t m_elementStride;
		ComPtr<ID3D11Buffer> m_pCounterBuffer;
		ComPtr<ID3D11Buffer> m_pBuffer;
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		ComPtr<ID3D11UnorderedAccessView> m_pUnorderedAccessView;

	public:
		D3DStructuredBuffer();
		~D3DStructuredBuffer();
		D3DStructuredBuffer(const D3DStructuredBuffer &) = delete;
		D3DStructuredBuffer(D3DStructuredBuffer &&) = delete;
		D3DStructuredBuffer &operator=(const D3DStructuredBuffer &) = delete;
		D3DStructuredBuffer &operator=(D3DStructuredBuffer &&) = delete;

		// NOTE: Create the structured buffer
		bool create(const D3DDevice &device, uint32_t elements, uint32_t stride, bool has_counter, bool has_append);

		// NOTE: Returns the hidden counter for append structured buffers or counter structured buffers
		uint32_t get_counter(const D3DDevice &device) const;

		// NOTE: Returns the UAV for the structured buffer
		ID3D11UnorderedAccessView * const get_uav() const;

		// NOTE: Returns the SRV for the structured buffer
		ID3D11ShaderResourceView * const get_srv() const;

		// NOTE: Return the base buffer for the structured buffer
		ID3D11Buffer * const get_buffer() const;

	private:
		bool create_srv(const D3DDevice &device);
		bool create_uav(const D3DDevice &device);
		bool create_buffer(const D3DDevice &device);
		bool create_counter_buffer(const D3DDevice &device);
	};
}
