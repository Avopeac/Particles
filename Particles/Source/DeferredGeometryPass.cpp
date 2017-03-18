#include <Precompiled.h>

#include "DeferredGeometryPass.h"

using namespace Direct3D;
using namespace Rendering;

Rendering::DeferredGeometryPass::DeferredGeometryPass() :
	m_width(0),
	m_height(0),
	m_deferredShaderPath(""),
	m_pSplatTextureShaderResourceView(0)
{
	ZeroMemory(&m_viewport, sizeof(m_viewport));
}

Rendering::DeferredGeometryPass::~DeferredGeometryPass()
{

}

bool Rendering::DeferredGeometryPass::initialize(const D3DDevice &device)
{
	if (!m_deferredShader.compile(device, m_deferredShaderPath, SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL))
	{
		return false;
	}

	if (!m_depthStencil.initialize(device, m_width, m_height, 
		DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_R24_UNORM_X8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT))
	{
		return false;
	}

	uint8_t diffuseFlags = RenderTargetFlags::ENABLE_RTV | RenderTargetFlags::ENABLE_SRV;
	if (!m_targets[GBuffer::GBUFFER_DIFFUSE].initialize(device, m_width, m_height, diffuseFlags,
		DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, DXGI_FORMAT_R11G11B10_FLOAT, 1))
	{
		return false;
	}

	uint8_t normalSpecularFlags = RenderTargetFlags::ENABLE_RTV | RenderTargetFlags::ENABLE_SRV;
	if (!m_targets[GBuffer::GBUFFER_NORMAL_SPECULAR].initialize(device, m_width, m_height, normalSpecularFlags,
		DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM, 1))
	{
		return false;
	}

	if (!m_samplerState.initialize(device, D3D11_TEXTURE_ADDRESS_WRAP, 
			D3D11_TEXTURE_ADDRESS_WRAP, 
			D3D11_TEXTURE_ADDRESS_WRAP,
			D3D11_COMPARISON_NEVER,
			D3D11_FILTER_MIN_MAG_MIP_LINEAR, 1) ||
		!m_samplerLinearClampState.initialize(device, D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP,
			D3D11_TEXTURE_ADDRESS_CLAMP, 
			D3D11_COMPARISON_NEVER, 
			D3D11_FILTER_MIN_MAG_MIP_LINEAR, 1))
	{ 
		return false; 
	}

	if (!m_meshBuffer.create(device) ||
		!create_input_layout(device)) 
	{ 
		return false; 
	}

	noiseTexture.set_filetype(TextureFileType::TEXTURE_FILE_WIC);
	noiseTexture.set_path("Textures/noise.png");
	if (!noiseTexture.load(device))
	{
		return false;
	}

	// NOTE: Set the viewport, because render targets may be of different resolution than the back buffer
	m_viewport.Width = static_cast<float>(m_width);
	m_viewport.Height = static_cast<float>(m_height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	return true;
}

void Rendering::DeferredGeometryPass::render(const D3DDevice &device, const PerspectiveCamera &camera, const MeshTextureStore &store, const std::vector<D3DMesh *> &meshes)
{

	ID3D11DeviceContext * context = device.get_context();
	ID3D11Buffer * matrixBuffer = camera.get_matrix_buffer();
	ID3D11SamplerState * samplerState = m_samplerState.get_state();
	ID3D11SamplerState * clampSamplerState = m_samplerLinearClampState.get_state();

	// NOTE: Fill G-Buffer
	{
		const uint32_t NUM_SRVS = 5;
		const float RTV_CLEAR_COLOR[4] = { 0, 0, 0, 1 };
		const float UAV_CLEAR_COLOR[4] = { 0, 0, 0, 0 };

		ID3D11ShaderResourceView * shaderResourceViews[NUM_SRVS] = { 0, 0, 0, 0, 0 };
		ID3D11RenderTargetView * renderTargetViews[GBUFFER_COUNT] =
		{
			m_targets[GBuffer::GBUFFER_DIFFUSE].get_rtv(),
			m_targets[GBuffer::GBUFFER_NORMAL_SPECULAR].get_rtv(),
		};

		m_deferredShader.bind_shader(device, ShaderType::SHADER_TYPE_VERTEX);
		m_deferredShader.bind_shader(device, ShaderType::SHADER_TYPE_PIXEL);

		context->VSSetConstantBuffers(0, 1, &matrixBuffer);
		context->PSSetConstantBuffers(0, 1, &matrixBuffer);
		context->PSSetSamplers(0, 1, &samplerState);
		context->PSSetSamplers(1, 1, &clampSamplerState);
		context->IASetInputLayout(m_pInputLayout.Get());

		device.set_viewport(m_viewport);

		// NOTE: Clear all targets
		for (uint32_t buffer = 0; buffer < GBUFFER_COUNT; ++buffer)
		{
			context->ClearRenderTargetView(renderTargetViews[buffer], RTV_CLEAR_COLOR);
		}

		context->ClearDepthStencilView(m_depthStencil.get_dsv(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		context->OMSetRenderTargets(GBUFFER_COUNT, renderTargetViews, m_depthStencil.get_dsv());

		ID3D11ShaderResourceView * noiseShaderResourceView = noiseTexture.get_srv();
		context->PSSetShaderResources(5, 1, &noiseShaderResourceView);

		// NOTE: Draw all meshes
		for (uint32_t meshIndex = 0; meshIndex < meshes.size(); ++meshIndex)
		{
			const D3DMesh * mesh = meshes[meshIndex];
			MeshMaterial material = mesh->get_material();

			MeshData meshData;
			meshData.textures = XMFLOAT4(0, 0, 0, 0);
			meshData.specularPower = material.get_specular_power();
			meshData.diffuseColor = material.get_diffuse_color();
			meshData.meshWorldTransform = mesh->get_world_transform();
			meshData.uvNormalizeTransform = mesh->get_texcoord_transform();
			// TODO: Hard coded for now
			//meshData.paintTextureSize = XMFLOAT4(128, 128, 1.0f / 128.0f, 1.0f / 128.0f);
			//meshData.paintTextureSize = XMFLOAT4(256, 256, 1.0f / 256.0f, 1.0f / 256.0f);
			meshData.paintTextureSize = XMFLOAT4(512, 512, 1.0f / 512.0f, 1.0f / 512.0f);
			//meshData.paintTextureSize = XMFLOAT4(1024, 1024, 1.0f / 1024.0f, 1.0f / 1024.0f);

			for (TextureDescriptor descriptor : mesh->get_textures())
			{
				const MeshTexture * texture = store.get_texture(descriptor.hash);
								
				if (texture)
				{
					ID3D11ShaderResourceView * shaderResourceView = texture->get_srv();

					if (shaderResourceView)
					{
						switch (descriptor.type)
						{
							case TextureType::TEXTURE_TYPE_DIFFUSE:
							{
								context->PSSetShaderResources(0, 1, &shaderResourceView);
							}
							break;

							case TextureType::TEXTURE_TYPE_NORMAL:
							{
								context->PSSetShaderResources(1, 1, &shaderResourceView);
							}
							break;

							case TextureType::TEXTURE_TYPE_SPECULAR:
							{
								context->PSSetShaderResources(2, 1, &shaderResourceView);
							}
							break;

							case TextureType::TEXTURE_TYPE_MASK:
							{
								meshData.textures.x = 1.0f;
								context->PSSetShaderResources(3, 1, &shaderResourceView);
							}
							break;
						}
					}
				}
			}

			// NOTE: Get the paint texture
			ID3D11ShaderResourceView * paintShaderResourceView = mesh->get_additional_texture();
			meshData.textures.y = paintShaderResourceView ? 1.0f : 0.0f;
			context->PSSetShaderResources(4, 1, &paintShaderResourceView);

			m_meshBuffer.map_data(context, &meshData);
			ID3D11Buffer * meshDataBuffer = m_meshBuffer.get_data();
			context->VSSetConstantBuffers(1, 1, &meshDataBuffer);
			context->PSSetConstantBuffers(1, 1, &meshDataBuffer);
			
			mesh->render(device);
			context->DrawIndexed(mesh->get_index_count(), 0, 0);
			context->PSSetShaderResources(0, NUM_SRVS, shaderResourceViews);
		}

		for (uint32_t rtv = 0; rtv < GBUFFER_COUNT; ++rtv)
		{
			renderTargetViews[rtv] = 0;
		}

		context->OMSetRenderTargets(GBUFFER_COUNT, renderTargetViews, 0);
	}

}

bool Rendering::DeferredGeometryPass::create_input_layout(const D3DDevice &device)
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

	ID3DBlob * vertexShaderData = m_deferredShader.get_data(ShaderType::SHADER_TYPE_VERTEX);
	HRESULT hr= device.get_device()->CreateInputLayout(desc, VERTEX_PROPERTIES, 
		vertexShaderData->GetBufferPointer(), vertexShaderData->GetBufferSize(), m_pInputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create input layout for deferred renderer.", 0, 0);
		return false;
	}

	return true;
}

void Rendering::DeferredGeometryPass::set_target_size(uint32_t width, uint32_t height)
{
	m_width = width;
	m_height = height;
}

void Rendering::DeferredGeometryPass::set_shader_path(const std::string & path)
{
	m_deferredShaderPath = path;
}

const D3DRenderTarget & Rendering::DeferredGeometryPass::get_target(GBuffer index) const
{
	return m_targets[index];
}

const D3DDepthStencil & Rendering::DeferredGeometryPass::get_depth_stencil() const
{
	return m_depthStencil;
}
