#pragma once

#include "MeshTexture.h"
#include "MeshMaterial.h"
#include "BoundingBox.h"

namespace Mesh
{

	using namespace Direct3D;
	using namespace DirectX;

	// NOTE: Assuming we only need these attributes for a mesh vertex
	struct MeshVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 bitangent;
		XMFLOAT3 color;
		XMFLOAT2 uv;
	};

	// The mesh asset contains mesh data gathered from file by the model importer.
	// Author: Andreas Larsson
	// Date: 2016-12-09 (YYYY-MM-DD)
	class MeshAsset
	{
		uint32_t m_vertexCount;
		uint32_t m_indexCount;
		uint32_t m_textureCount;
		XMMATRIX m_worldTransform;
		XMVECTOR m_texcoordTransform;
		MeshMaterial m_meshMaterial;
		Math::BoundingBox m_boundingBox;
		std::vector<MeshVertex> m_vertices;
		std::vector<uint32_t> m_indices;
		std::vector<TextureDescriptor> m_textureDescriptors;

	public:
		MeshAsset();
		~MeshAsset();
		MeshAsset(const MeshAsset &);
		MeshAsset(MeshAsset &&);
		
		MeshAsset &operator=(const MeshAsset &other)
		{
			if (this != &other)
			{
				m_vertexCount = other.m_vertexCount;
				m_indexCount = other.m_indexCount;
				m_textureCount = other.m_textureCount;
				m_vertices = other.m_vertices;
				m_indices = other.m_indices;
				m_textureDescriptors = other.m_textureDescriptors;
				m_meshMaterial = other.m_meshMaterial;
				m_worldTransform = other.m_worldTransform;
				m_texcoordTransform = other.m_texcoordTransform;
				m_boundingBox = other.m_boundingBox;
			}

			return *this;
		}

		MeshAsset &operator=(MeshAsset &&other) 
		{
			if (this != &other) 
			{
				m_vertexCount = other.m_vertexCount;
				m_indexCount = other.m_indexCount;
				m_textureCount = other.m_textureCount;
				m_vertices = other.m_vertices;
				m_indices = other.m_indices;
				m_textureDescriptors = other.m_textureDescriptors;
				m_meshMaterial = other.m_meshMaterial;
				m_worldTransform = other.m_worldTransform;
				m_texcoordTransform = other.m_texcoordTransform;
				m_boundingBox = other.m_boundingBox;
			}

			return *this;
		}

		// NOTE: Set the local to world space transform
		void set_world_transform(const XMMATRIX &transform);

		// NOTE: Returns the local to world space transform
		const XMMATRIX &get_world_transform() const;

		// NOTE: Returns the number of vertices in the mesh
		uint32_t get_vertex_count() const;

		// NOTE: Returns the number of indices in the mesh
		uint32_t get_index_count() const;

		// NOTE: Returns the number of texture descriptors in the mesh
		uint32_t get_texture_count() const;

		// NOTE: Returns a vector of the vertices in the mesh
		const std::vector<MeshVertex> &get_vertices() const;

		// NOTE: Returns a vector of the indices in the mesh
		const std::vector<uint32_t> &get_indices() const;

		// NOTE: Returns a vector of the texture descriptors of the mesh
		const std::vector<TextureDescriptor> &get_texture_descriptors() const;

		// NOTE: Add a vertex to the mesh
		void add_vertex(const MeshVertex &v);

		// NOTE: Add an index to the mesh
		void add_indices(uint32_t i);

		// NOTE: Add a texture descriptor to the mesh
		void add_texture_descriptor(const TextureDescriptor &t);

		// NOTE: Set the material description for the mesh
		void set_material(const MeshMaterial &material);

		// NOTE: Returns the mesh material description
		const MeshMaterial &get_material() const;

		// NOTE: Set the texture coordinate transform to scale and bias the UV coordinates to fit in [0, 1]
		void set_texcoord_transform(const XMVECTOR &transform);

		// NOTE: Get the texture coordinate transform
		const XMVECTOR &get_texcoord_transform() const;

		void set_bounding_box(const Math::BoundingBox &boundingBox);

		const Math::BoundingBox &get_bounding_box() const;

	private:
	
	};
}