#include <Precompiled.h>

#include "D3DShader.h"
#include "D3DFullscreenQuad.h"

using namespace Direct3D;
using namespace PostProcessing;
using namespace Direct3DResource;

PostProcessing::D3DFullscreenQuad::D3DFullscreenQuad() :
	m_vertexCount(0),
	m_indexCount(0)
{

}

PostProcessing::D3DFullscreenQuad::~D3DFullscreenQuad()
{

}

uint32_t PostProcessing::D3DFullscreenQuad::get_index_count() const
{
	return m_indexCount;
}

bool D3DFullscreenQuad::initialize(const D3DDevice &device, const D3DShader &shader)
{
	if (!create_vertex_buffer(device) ||
		!create_index_buffer(device) || 
		!create_input_layout(device, shader)) 
	{ 
		return false; 
	}

	return true;
}

void D3DFullscreenQuad::render(const D3DDevice &device) const
{
	uint32_t offsets = 0;
	uint32_t stride = sizeof(FullscreenQuadVertex);
	ID3D11DeviceContext * context = device.get_context();
	context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offsets);
	context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_pInputLayout.Get());
	context->DrawIndexed(m_indexCount, 0, 0);
}

bool D3DFullscreenQuad::create_input_layout(const D3DDevice &device, const D3DShader &shader)
{
	const uint32_t NUM_ATTRIBS = 2;

	D3D11_INPUT_ELEMENT_DESC desc[NUM_ATTRIBS];
	ZeroMemory(desc, sizeof(desc));
	desc[0].SemanticName = "POSITION";
	desc[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[1].SemanticName = "TEXCOORD";
	desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	ID3DBlob * vertexShaderData = shader.get_data(ShaderType::SHADER_TYPE_VERTEX);
	HRESULT hr= device.get_device()->CreateInputLayout(desc, NUM_ATTRIBS, 
		vertexShaderData->GetBufferPointer(), vertexShaderData->GetBufferSize(), m_pInputLayout.GetAddressOf());
	if (FAILED(hr)) 
	{
		MessageBoxA(0, "Failed to create input layout for fullscreen quad.", 0, 0);
		return false;
	}

	return true;
}

bool D3DFullscreenQuad::create_vertex_buffer(const D3DDevice &device)
{
	const uint32_t NUM_VERTICES = 4;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = NUM_VERTICES * sizeof(FullscreenQuadVertex);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	FullscreenQuadVertex vertices[NUM_VERTICES];
	{
		vertices[0].position = XMFLOAT4(-1, -1, 0, 1);
		vertices[0].uv = XMFLOAT2(0, 1);
		vertices[1].position = XMFLOAT4(1, -1, 0, 1);
		vertices[1].uv = XMFLOAT2(1, 1);
		vertices[2].position = XMFLOAT4(-1, 1, 0, 1);
		vertices[2].uv = XMFLOAT2(0, 0);
		vertices[3].position = XMFLOAT4(1, 1, 0, 1);
		vertices[3].uv = XMFLOAT2(1, 0);
	}

	m_vertexCount = NUM_VERTICES;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = vertices;

	HRESULT hr = device.get_device()->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create vertex buffer for fullscreen quad.", 0, 0);
		return false;
	}
	
	return true;
}

bool D3DFullscreenQuad::create_index_buffer(const D3DDevice &device)
{
	const uint32_t NUM_INDICES = 6;
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = NUM_INDICES * sizeof(uint32_t);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	uint32_t indices[NUM_INDICES];
	{
		indices[0] = 0;
		indices[1] = 2;
		indices[2] = 1;
		indices[3] = 1;
		indices[4] = 2;
		indices[5] = 3;
	}
	m_indexCount = NUM_INDICES;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = indices;

	HRESULT hr = device.get_device()->CreateBuffer(&desc, &data, m_pIndexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Could not create index buffer for fullscreen quad.", 0, 0);
		return false;
	}
	
	return true;
}
