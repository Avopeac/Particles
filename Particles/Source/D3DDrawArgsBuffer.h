#pragma once

#include "D3DDevice.h"

namespace Direct3DResource
{

	using namespace Direct3D;

	class D3DDrawArgsBuffer
	{
		
		ComPtr<ID3D11Buffer> m_pDrawArgsBuffer;
		ComPtr<ID3D11UnorderedAccessView> m_pDrawArgsUnorderedAccessView;

	public:
		D3DDrawArgsBuffer();
		~D3DDrawArgsBuffer();
		D3DDrawArgsBuffer(const D3DDrawArgsBuffer &) = delete;
		D3DDrawArgsBuffer(D3DDrawArgsBuffer &&) = delete;
		D3DDrawArgsBuffer &operator=(const D3DDrawArgsBuffer &) = delete;
		D3DDrawArgsBuffer &operator=(D3DDrawArgsBuffer &&) = delete;
			
		bool create(const D3DDevice &device);

		ID3D11Buffer * get_buffer() const;

		ID3D11UnorderedAccessView * get_uav() const;

	protected:
				
	private:

	};

}
