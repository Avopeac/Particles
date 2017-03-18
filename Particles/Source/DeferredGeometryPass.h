#pragma once

#include "D3DMesh.h"
#include "D3DStructuredBuffer.h"
#include "D3DShader.h"
#include "D3DDevice.h"
#include "D3DConstantBuffer.h"
#include "D3DFullscreenQuad.h"
#include "D3DRenderTarget.h"
#include "D3DDepthStencil.h"
#include "D3DSamplerState.h"

#include "PerspectiveCamera.h"
#include "MeshAsset.h"
#include "MeshTextureStore.h"

namespace Rendering
{
	using namespace DirectX;
	using namespace Direct3D;
	using namespace Direct3DResource;

	// NOTE: All the G-Buffer targets
	enum GBuffer 
	{
		GBUFFER_DIFFUSE,
		GBUFFER_NORMAL_SPECULAR,
		GBUFFER_UNKNOWN,
		GBUFFER_COUNT = GBUFFER_UNKNOWN
	};

	struct MeshData
	{
		XMMATRIX meshWorldTransform;
		XMVECTOR uvNormalizeTransform;
		XMFLOAT4 paintTextureSize;
		XMFLOAT4 textures;
		XMFLOAT3 diffuseColor;
		float specularPower;
	};

	// The deferred geometry pass renders all meshes that are sent to it. It also writes to a UAV position atlas in the pixel shader. 
	// The position atlas contains the screen space positions of meshes in the scene.
	// The screen space positions are stored in the mesh's UV space. Each mesh has its own unique number, that number is used to offset into the position atlas.
	// It also takes several SRVs as input that has paint information available that maps 1:1 to the position atlas, as such painting can be applied to any mesh in the position atlas.
	// Author: Andreas Larsson
	// Date: 2016-12-11 (YYYY-MM-DD)
	class DeferredGeometryPass
	{
		uint32_t m_width;
		uint32_t m_height;
		D3D11_VIEWPORT m_viewport;
		std::string m_deferredShaderPath;
		D3DShader m_deferredShader;
		D3DRenderTarget m_targets[GBUFFER_COUNT];
		D3DDepthStencil m_depthStencil;
		D3DSamplerState m_samplerState;
		D3DSamplerState m_samplerLinearClampState;
		D3DConstantBuffer<MeshData> m_meshBuffer;
		ComPtr<ID3D11InputLayout> m_pInputLayout;
		mutable ID3D11ShaderResourceView * m_pSplatTextureShaderResourceView;

		MeshTexture noiseTexture; 
		
	public:
		DeferredGeometryPass();
		~DeferredGeometryPass();
		DeferredGeometryPass(const DeferredGeometryPass &) = delete;
		DeferredGeometryPass(DeferredGeometryPass &&) = delete;
		DeferredGeometryPass &operator=(const DeferredGeometryPass &) = delete;
		DeferredGeometryPass &operator=(DeferredGeometryPass &&) = delete;
		
		// NOTE: Set the actual render target size
		void set_target_size(uint32_t width, uint32_t height);

		// NOTE: Set deferred shader file path
		void set_shader_path(const std::string &path);

		// NOTE: Get any G-Buffer target based on the index
		const D3DRenderTarget &get_target(GBuffer index) const;

		// NOTE: Get the DSV of the deferred renderer
		const D3DDepthStencil &get_depth_stencil() const;

		// NOTE: Initialize the deferred renderer with the set parameters
		bool initialize(const D3DDevice &device);

		// NOTE: Render all meshes passed to the renderer
		void render(const D3DDevice &device, const PerspectiveCamera &camera, const MeshTextureStore &store, const std::vector<D3DMesh *> &meshes);
		
	private:
		bool create_input_layout(const D3DDevice &device);
	};
}
