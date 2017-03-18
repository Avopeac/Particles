#include <Precompiled.h>
#include "ParticlePaintingPrealloc.h"

using namespace Particles;

Particles::ParticlePaintingPrealloc::ParticlePaintingPrealloc(const D3DDevice & device, uint32_t allocations, 
	uint32_t width, uint32_t height, uint32_t paintsPerFrame) :
	m_device(device), m_paintAllocations(allocations), m_paintWidth(width), m_paintHeight(height), m_paintsPerFrame(paintsPerFrame) {
	ZeroMemory(&m_paintShaderViewport, sizeof(m_paintShaderViewport));
	ZeroMemory(&m_paint, sizeof(m_paint));
}

Particles::ParticlePaintingPrealloc::~ParticlePaintingPrealloc() { }

bool Particles::ParticlePaintingPrealloc::initialize(const std::string &paintShaderName)
{
	if (!m_paintShader.compile(m_device, paintShaderName, ShaderType::SHADER_TYPE_VERTEX | ShaderType::SHADER_TYPE_PIXEL)) {
		return false;
	}

	if (!m_bilinearSampler.initialize(m_device, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_NEVER, D3D11_FILTER_MIN_MAG_MIP_LINEAR, 1) ||
		!m_nearestSampler.initialize(m_device, D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP, D3D11_COMPARISON_NEVER, D3D11_FILTER_MIN_MAG_MIP_POINT, 1)) {
		return false;
	}

	if (!m_worldTransformBuffer.create(m_device) || !m_texcoordTransformBuffer.create(m_device) || !create_input_layout()) {
		return false;
	}

	// Allocate paint textures
	m_paint.reserve(m_paintAllocations);
	for (uint32_t i = 0; i < m_paintAllocations; ++i) {
		m_paint.emplace_back();
		if (!create_target(m_paint[i], m_paintWidth, m_paintHeight)) {
			return false;
		}
	}

	return true;
}

void Particles::ParticlePaintingPrealloc::render(const PerspectiveCamera &camera, const std::vector<D3DMesh *>& meshes,
	const DeferredGeometryPass & deferred, const D3DRenderTarget * collisionTarget, const std::vector<Math::BoundingBox> boxes)
{
	if (!collisionTarget) {
		return;
	}

	ID3D11DeviceContext * context = m_device.get_context();
	D3D11_VIEWPORT old_viewport = m_device.get_current_viewport();

	m_device.set_viewport(m_paintShaderViewport);
	m_device.set_depth_test(DepthTest::DEPTH_TEST_OFF);
	m_paintShader.bind_shader(m_device, ShaderType::SHADER_TYPE_VERTEX);
	m_paintShader.bind_shader(m_device, ShaderType::SHADER_TYPE_PIXEL);

	ID3D11Buffer * cameraTransformDataBuffer = camera.get_matrix_buffer();
	context->VSSetConstantBuffers(0, 1, &cameraTransformDataBuffer);
	context->PSSetConstantBuffers(0, 1, &cameraTransformDataBuffer);
	
	ID3D11ShaderResourceView * shaderResourceView = collisionTarget->get_srv();
	context->PSSetShaderResources(0, 1, &shaderResourceView);
	
	ID3D11SamplerState * samplerStateLinear = m_bilinearSampler.get_state();
	ID3D11SamplerState * samplerStatePoint = m_nearestSampler.get_state();
	context->PSSetSamplers(0, 1, &samplerStateLinear);
	context->PSSetSamplers(1, 1, &samplerStatePoint);

	context->IASetInputLayout(m_pInputLayout.Get());

	std::vector<D3DMesh *> intersectingMeshes;
	for (uint32_t j = 0; j < meshes.size(); ++j) {
		for (uint32_t i = 0; i < boxes.size(); ++i) {
			if (meshes[j]->get_bounding_box().intersects(boxes[i])) {
				intersectingMeshes.push_back(meshes[j]);
			}
		}
	}

	// Increase this to get more frequent paint application at a performance penalty
	uint32_t stopIndex = static_cast<uint32_t>(intersectingMeshes.size());
	if (stopIndex > m_paintsPerFrame) {
		std::random_shuffle(intersectingMeshes.begin(), intersectingMeshes.end());
		stopIndex = m_paintsPerFrame;
	}
	
	// NOTE: For each intersected mesh, allocate texture (if need be) set as render target and paint
	for (uint32_t index = 0; index < stopIndex; ++index) {

		D3DMesh * mesh = intersectingMeshes[index];
		uint32_t meshNumber = mesh->get_mesh_number();

		// Get the texture for the current mesh
		PaintCollection * collection = 0;
		if (m_paintIndices.find(meshNumber) != m_paintIndices.end()) {
			collection = &m_paint[m_paintIndices[meshNumber]];
		} else {
			uint32_t textureIndex = get_free_texture();
			m_paintIndices[meshNumber] = textureIndex;
			collection = &m_paint[textureIndex];
			collection->used = true;
		}

		if (collection) {

			ID3D11RenderTargetView * renderTargetView = collection->target0.get_rtv();
			ID3D11ShaderResourceView * shaderResourceView = collection->target1.get_srv();

			// Swap textures to keep paint persistent between frames
			collection->currentRenderTargetView = renderTargetView;
			collection->currentShaderResourceView = shaderResourceView;
			if ((collection->swapCounter++ % 2) == 0) {
				renderTargetView = collection->target1.get_rtv();
				shaderResourceView = collection->target0.get_srv();
				collection->currentRenderTargetView = renderTargetView;
				collection->currentShaderResourceView = shaderResourceView;
			}

			// Set the viewport to cover the whole paint texture
			m_paintShaderViewport.Width = static_cast<float>(collection->target0.get_width());
			m_paintShaderViewport.Height = static_cast<float>(collection->target0.get_height());
			m_paintShaderViewport.MinDepth = 0.0f;
			m_paintShaderViewport.MaxDepth = 1.0f;
			m_paintShaderViewport.TopLeftX = 0.0f;
			m_paintShaderViewport.TopLeftY = 0.0f;

			context->RSSetViewports(1, &m_paintShaderViewport);
			context->OMSetRenderTargets(1, &renderTargetView, 0);
			context->PSSetShaderResources(1, 1, &shaderResourceView);

			XMMATRIX worldTransform = mesh->get_world_transform();
			m_worldTransformBuffer.map_data(context, &worldTransform);

			XMVECTOR texcoordTransform = mesh->get_texcoord_transform();
			m_texcoordTransformBuffer.map_data(context, &texcoordTransform);

			ID3D11Buffer * worldTransformDataBuffer = m_worldTransformBuffer.get_data();
			context->VSSetConstantBuffers(1, 1, &worldTransformDataBuffer);
			context->PSSetConstantBuffers(1, 1, &worldTransformDataBuffer);
			
			ID3D11Buffer * texcoordTransformDataBuffer = m_texcoordTransformBuffer.get_data();
			context->VSSetConstantBuffers(2, 1, &texcoordTransformDataBuffer);
			context->PSSetConstantBuffers(2, 1, &texcoordTransformDataBuffer);

			mesh->render(m_device);

			context->DrawIndexed(mesh->get_index_count(), 0, 0);

			mesh->set_additional_texture(collection->currentShaderResourceView);
		}
	}

	ID3D11ShaderResourceView * shaderResourceViews[2] = { 0, 0 };
	context->PSSetShaderResources(0, 2, shaderResourceViews);

	ID3D11RenderTargetView * renderTargetViews = 0;
	context->OMSetRenderTargets(1, &renderTargetViews, 0);

	m_paintShader.unbind_shader(m_device, ShaderType::SHADER_TYPE_VERTEX);
	m_paintShader.unbind_shader(m_device, ShaderType::SHADER_TYPE_PIXEL);
	m_device.set_depth_test(DepthTest::DEPTH_TEST_ON);
	m_device.set_viewport(old_viewport);
}

void Particles::ParticlePaintingPrealloc::set_paints_per_frame(uint32_t count)
{
	m_paintsPerFrame = count;
}

bool Particles::ParticlePaintingPrealloc::create_input_layout()
{
	const uint8_t VERTEX_PROPERTIES = 6;

	D3D11_INPUT_ELEMENT_DESC desc[VERTEX_PROPERTIES];
	ZeroMemory(desc, sizeof(desc));

	desc[0].SemanticName = "POSITION";
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	desc[1].SemanticName = "TEXCOORD";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	desc[2].SemanticName = "TEXCOORD";
	desc[2].SemanticIndex = 1;
	desc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	desc[3].SemanticName = "TEXCOORD";
	desc[3].SemanticIndex = 2;
	desc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	desc[4].SemanticName = "COLOR";
	desc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	desc[5].SemanticName = "TEXCOORD";
	desc[5].SemanticIndex = 3;
	desc[5].Format = DXGI_FORMAT_R32G32_FLOAT;
	desc[5].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	desc[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	ID3DBlob * vertexShaderData = m_paintShader.get_data(ShaderType::SHADER_TYPE_VERTEX);
	HRESULT hr = m_device.get_device()->CreateInputLayout(desc, VERTEX_PROPERTIES,
		vertexShaderData->GetBufferPointer(), vertexShaderData->GetBufferSize(), m_pInputLayout.GetAddressOf());
	if (FAILED(hr)) {
		MessageBoxA(0, "Failed to create input layout for particle paint renderer.", 0, 0);
		return false;
	}

	return true;
}

bool Particles::ParticlePaintingPrealloc::create_target(PaintCollection & collection, uint32_t width, uint32_t height)
{ 
	uint8_t paintFlags = RenderTargetFlags::ENABLE_RTV | RenderTargetFlags::ENABLE_SRV;
	if (!collection.target0.initialize(m_device, width, height, paintFlags, DXGI_FORMAT_R16G16B16A16_UNORM, 
			DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, 1) ||
		!collection.target1.initialize(m_device, width, height, paintFlags, DXGI_FORMAT_R16G16B16A16_UNORM, 
			DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, 1)) {
		return false;
	}

	collection.allocated = true;
	collection.used = false;
	collection.currentRenderTargetView = collection.target1.get_rtv();
	collection.currentShaderResourceView = collection.target0.get_srv();
	collection.swapCounter = 0;

	return true;
}

int32_t Particles::ParticlePaintingPrealloc::get_free_texture() {

	for (uint32_t i = 0; i < m_paint.size(); ++i) {
		if (!m_paint[i].used) {
			return static_cast<int32_t>(i);
		}
	}

	return -1;
}
