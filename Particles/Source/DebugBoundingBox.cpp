#include <Precompiled.h>

#include "DebugBoundingBox.h"

using namespace DirectX;
using namespace Direct3D;
using namespace Direct3DResource;

Rendering::DebugBoundingBox::DebugBoundingBox() :
	m_created(false)
{
}

bool Rendering::DebugBoundingBox::create(const D3DDevice & device, const D3DShader &shader)
{
	if (m_created) {
		return false;
	}

	if (!m_matrixBuffer.create(device) ||
		!create_vertex_buffer(device) ||
		!create_index_buffer(device) ||
		!create_raster_state(device) ||
		!create_input_layout(device, shader)) {
		return false;
	}

	m_created = true;
	return m_created;
}

bool Rendering::DebugBoundingBox::create_index_buffer(const D3DDevice & device)
{
	uint32_t indices[36];

	indices[0] = 0; indices[1] = 1; indices[2] = 2;
	indices[3] = 0; indices[4] = 2; indices[5] = 3;
	indices[6] = 4; indices[7] = 5; indices[8] = 6;
	indices[9] = 4; indices[10] = 6; indices[11] = 7;
	indices[12] = 4; indices[13] = 5; indices[14] = 1;
	indices[15] = 4; indices[16] = 1; indices[17] = 0;
	indices[18] = 6; indices[19] = 7; indices[20] = 3;
	indices[21] = 6; indices[22] = 3; indices[23] = 2;
	indices[24] = 4; indices[25] = 0; indices[26] = 3;
	indices[27] = 4; indices[28] = 3; indices[29] = 7;
	indices[30] = 6; indices[31] = 2; indices[32] = 1;
	indices[33] = 6; indices[34] = 1; indices[35] = 5;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(indices);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &indices;
	
	HRESULT hr = device.get_device()->CreateBuffer(&desc, &data, m_pIndexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	m_indexCount = 36;

	return true;
}

bool Rendering::DebugBoundingBox::create_vertex_buffer(const D3DDevice & device)
{
	XMFLOAT3 vertices[8];
	vertices[0] = XMFLOAT3(-0.5f, 0.5f, -0.5f);
	vertices[1] = XMFLOAT3(-0.5f, 0.5f, 0.5f);
	vertices[2] = XMFLOAT3(0.5f, 0.5f, 0.5f);
	vertices[3] = XMFLOAT3(0.5f, 0.5f, -0.5f);
	vertices[4] = XMFLOAT3(-0.5f, -0.5f, -0.5f);
	vertices[5] = XMFLOAT3(-0.5f, -0.5f, 0.5f);
	vertices[6] = XMFLOAT3(0.5f, -0.5f, 0.5f);
	vertices[7] = XMFLOAT3(0.5f, -0.5f, -0.5f);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(vertices);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
	data.pSysMem = &vertices;

	HRESULT hr = device.get_device()->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

bool Rendering::DebugBoundingBox::create_input_layout(const D3DDevice & device, const D3DShader &shader)
{
	const uint8_t VERTEX_PROPERTIES = 1;

	D3D11_INPUT_ELEMENT_DESC desc[VERTEX_PROPERTIES];
	ZeroMemory(desc, sizeof(desc));
	desc[0].SemanticName = "POSITION";
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	ID3DBlob * vertexShaderData = shader.get_data(ShaderType::SHADER_TYPE_VERTEX);
	HRESULT hr = device.get_device()->CreateInputLayout(desc, VERTEX_PROPERTIES,
		vertexShaderData->GetBufferPointer(), vertexShaderData->GetBufferSize(), m_pInputLayout.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxA(0, "Failed to create input layout for deferred renderer.", 0, 0);
		return false;
	}
	
	return true;
}

bool Rendering::DebugBoundingBox::create_raster_state(const D3DDevice & device)
{
	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.FrontCounterClockwise = true;
	desc.CullMode = D3D11_CULL_NONE;
	desc.FillMode = D3D11_FILL_WIREFRAME;
	
	HRESULT hr = device.get_device()->CreateRasterizerState(&desc, m_pRasterState.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void Rendering::DebugBoundingBox::render(const D3DDevice & device, const PerspectiveCamera &camera,
	const D3DShader & shader, const Math::BoundingBox & box)
{
	if (!m_created) { 
		return;
	}
		
	ID3D11RasterizerState * state = 0;
	device.get_context()->RSGetState(&state);

	shader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	shader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);

	uint32_t stride = sizeof(DebugVertex);
	uint32_t offset = 0;
	device.get_context()->IASetInputLayout(m_pInputLayout.Get());
	device.get_context()->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	device.get_context()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	device.get_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX worldMatrix = XMMatrixAffineTransformation(box.get_maximum() - box.get_minimum(), 
		box.get_center(), XMVectorZero(), box.get_center());

	auto cameraData = camera.get_matrix_buffer();
	device.get_context()->VSSetConstantBuffers(0, 1, &cameraData);

	m_matrixBuffer.map_data(device.get_context(), &worldMatrix);
	auto bufferData = m_matrixBuffer.get_data();
	device.get_context()->VSSetConstantBuffers(1, 1, &bufferData);

	device.get_context()->RSSetState(m_pRasterState.Get());

	auto target = device.get_rtv();
	device.get_context()->OMSetRenderTargets(1, &target, 0);
	device.get_context()->DrawIndexed(m_indexCount, 0, 0);

	shader.unbind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
	shader.unbind_shader(device, ShaderType::SHADER_TYPE_PIXEL);

	target = 0;
	device.get_context()->OMSetRenderTargets(0, &target, 0);
	device.get_context()->RSSetState(state);

}
