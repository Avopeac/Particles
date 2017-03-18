#pragma once

#include "D3DShader.h"
#include "D3DDevice.h"

namespace PostProcessing 
{
	using namespace DirectX;
	using namespace Direct3D;
	using namespace Direct3DResource;
	
	struct FullscreenQuadVertex 
	{
		XMFLOAT4 position;
		XMFLOAT2 uv;
	};

	class D3DFullscreenQuad 
	{
		uint32_t m_vertexCount;
		uint32_t m_indexCount;
		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;
		ComPtr<ID3D11InputLayout> m_pInputLayout;

	public:
		D3DFullscreenQuad();
		~D3DFullscreenQuad();
		D3DFullscreenQuad(const D3DFullscreenQuad &) = delete;
		D3DFullscreenQuad(D3DFullscreenQuad &&) = delete;
		D3DFullscreenQuad &operator=(const D3DFullscreenQuad &) = delete;
		D3DFullscreenQuad &operator=(D3DFullscreenQuad &&) = delete;
		
		// NOTE: Returns the number of indices for the quad
		uint32_t get_index_count() const;

		// NOTE: Initialize the quad, takes a shader that has the desired vertex attributes
		bool initialize(const D3DDevice &device, const D3DShader &shader);

		// NOTE: Set up the input assembly for rendering the quad
		void render(const D3DDevice &device) const;

	private:
		bool create_vertex_buffer(const D3DDevice &device);
		bool create_index_buffer(const D3DDevice &device);
		bool create_input_layout(const D3DDevice &device, const D3DShader &shader);
	};
}
