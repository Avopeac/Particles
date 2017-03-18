#pragma once

#include <Precompiled.h>

#include "D3DDevice.h"
#include "D3DShader.h"
#include "D3DConstantBuffer.h"

#include "BoundingBox.h"
#include "PerspectiveCamera.h"

namespace Rendering
{
	using namespace Math;
	using namespace DirectX;
	using namespace Direct3D;
	using namespace Direct3DResource;
	using namespace Microsoft::WRL;

	struct DebugVertex
	{
		XMFLOAT3 position;
	};

	class DebugBoundingBox
	{
		bool m_created;
		uint32_t m_indexCount;

		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;
		ComPtr<ID3D11InputLayout> m_pInputLayout;
		ComPtr<ID3D11RasterizerState> m_pRasterState;
 		D3DConstantBuffer<XMMATRIX> m_matrixBuffer;

	public:

		DebugBoundingBox();
		~DebugBoundingBox() = default;
		DebugBoundingBox(const DebugBoundingBox &) = delete;

		void render(const D3DDevice &device, const PerspectiveCamera &camera, const D3DShader &shader, const BoundingBox &box);

		bool create(const D3DDevice &device, const D3DShader &shader);

	private:

		bool create_index_buffer(const D3DDevice &device);

		bool create_vertex_buffer(const D3DDevice &device);

		bool create_input_layout(const D3DDevice &device, const D3DShader &shader);

		bool create_raster_state(const D3DDevice &device);

	};
}
