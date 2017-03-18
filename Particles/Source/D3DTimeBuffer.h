#pragma once

#include "D3DDevice.h"
#include "D3DConstantBuffer.h"

namespace Direct3D
{
	using namespace DirectX;
	using namespace Direct3DResource;

	class D3DTimeBuffer 
	{

		float m_deltaTime, m_elapsedTime;
		D3DConstantBuffer<XMFLOAT4> m_buffer;

	public:
		D3DTimeBuffer();
		~D3DTimeBuffer();
		D3DTimeBuffer(const D3DTimeBuffer &) = delete;
		D3DTimeBuffer(D3DTimeBuffer &&) = delete;
		D3DTimeBuffer &operator=(const D3DTimeBuffer &) = delete;
		D3DTimeBuffer &operator=(D3DTimeBuffer &&) = delete;

		// NOTE: Initialize the buffer
		bool initialize(const D3DDevice &device);

		// NOTE: Map new time data to buffer
		void update(const D3DDevice &device, float deltaTime, float elapsedTime);

		// NOTE: Get the constant buffer
		const D3DConstantBuffer<XMFLOAT4> &get_buffer() const;

		float get_delta_time() const;

		float get_elapsed_time() const;

	private:

	};
}

