#pragma once

#include "D3DRenderTarget.h"

#include "MeshAsset.h"

namespace Direct3DResource 
{
	using namespace Mesh;
	using namespace DirectX;

	class D3DMesh 
	{
		static uint32_t g_meshNumber;
		uint32_t m_meshNumber;
		uint32_t m_vertexCount;
		uint32_t m_indexCount;
		XMMATRIX m_worldTransform;
		XMVECTOR m_texcoordTransform;
		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;
		std::vector<TextureDescriptor> m_textureDescriptors;
		MeshMaterial m_meshMaterial;
		Math::BoundingBox m_boundingBox;

		// NOTE: Ugly hack, sorry const-erwissers.
		mutable ID3D11ShaderResourceView * m_pAdditionalTexture;

	public:
		D3DMesh();
		~D3DMesh();
		D3DMesh(const D3DMesh &);
		D3DMesh(D3DMesh &&);

		D3DMesh &operator=(const D3DMesh &other) 
		{
			if (this != &other)
			{
				m_meshNumber = other.m_meshNumber;
				m_vertexCount = other.m_vertexCount;
				m_indexCount = other.m_indexCount;
				m_worldTransform = other.m_worldTransform;
				m_texcoordTransform = other.m_texcoordTransform;
				m_textureDescriptors = other.m_textureDescriptors;
				m_meshMaterial = other.m_meshMaterial;
				m_pVertexBuffer = other.m_pVertexBuffer;
				m_pIndexBuffer = other.m_pIndexBuffer;
				m_pAdditionalTexture = other.m_pAdditionalTexture;
				m_boundingBox = other.m_boundingBox;
			}

			return *this;
		}

		D3DMesh &operator=(D3DMesh &&other)
		{
			if (this != &other)
			{
				m_meshNumber = other.m_meshNumber;
				m_vertexCount = other.m_vertexCount;
				m_indexCount = other.m_indexCount;
				m_worldTransform = other.m_worldTransform;
				m_texcoordTransform = other.m_texcoordTransform;
				m_textureDescriptors = other.m_textureDescriptors;
				m_meshMaterial = other.m_meshMaterial;
				m_pVertexBuffer = other.m_pVertexBuffer;
				m_pIndexBuffer = other.m_pIndexBuffer;
				m_pAdditionalTexture = other.m_pAdditionalTexture;
				m_boundingBox = other.m_boundingBox;
			}
			
			return *this;
		}
			
		// NOTE: Set the transform from local to world space
		void set_world_transform(const XMMATRIX &transform);
		
		// NOTE: Initialize vertex and index buffer from mesh asset
		bool initialize(const D3DDevice &device, const MeshAsset &mesh_asset);

		// NOTE: Render the mesh
		void render(const D3DDevice &device) const;

		// NOTE: Returns the index count of the mesh
		uint32_t get_index_count() const;

		// NOTE: Returns the unique mesh number
		uint32_t get_mesh_number() const;

		// NOTE: Set an additional diffuse texture (for paint rendering)
		void set_additional_texture(ID3D11ShaderResourceView * const srv) const;

		ID3D11ShaderResourceView * const get_additional_texture() const;

		// NOTE: Returns the world transform for this mesh
		const XMMATRIX &get_world_transform() const;

		// NOTE: Returns the mesh material
		const MeshMaterial &get_material() const;

		const Math::BoundingBox &get_bounding_box() const;

		// NOTE: Returns the texture coordinate transform which scales and biases the coordinates to UV space in [0, 1]
		const XMVECTOR &get_texcoord_transform() const;

		// NOTE: Returns the texture descriptors used to fetch textures from a texture storage class
		const std::vector<TextureDescriptor> &get_textures() const;

	private:
		
	};

}
