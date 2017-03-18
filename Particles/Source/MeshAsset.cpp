#include <Precompiled.h>

#include "MeshAsset.h"

using namespace Mesh;

MeshAsset::MeshAsset() :
	m_vertexCount(0), 
	m_indexCount(0), 
	m_textureCount(0) 
{
	m_worldTransform = XMMatrixIdentity();
	m_texcoordTransform = XMVectorZero();
}

MeshAsset::~MeshAsset()
{

}

MeshAsset::MeshAsset(const MeshAsset &other)
{
	assert(this != &other);
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

MeshAsset::MeshAsset(MeshAsset &&other)
{
	assert(this != &other);
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

void Mesh::MeshAsset::set_world_transform(const XMMATRIX & transform)
{
	m_worldTransform = transform;
}

const XMMATRIX & Mesh::MeshAsset::get_world_transform() const
{
	return m_worldTransform;
}

uint32_t Mesh::MeshAsset::get_vertex_count() const
{
	return m_vertexCount;
}

uint32_t Mesh::MeshAsset::get_index_count() const
{
	return m_indexCount;
}

uint32_t Mesh::MeshAsset::get_texture_count() const
{
	return m_textureCount;
}

const std::vector<MeshVertex>& Mesh::MeshAsset::get_vertices() const
{
	return m_vertices;
}

const std::vector<uint32_t>& Mesh::MeshAsset::get_indices() const
{
	return m_indices;
}

const std::vector<TextureDescriptor>& Mesh::MeshAsset::get_texture_descriptors() const
{
	return m_textureDescriptors;
}

void Mesh::MeshAsset::add_vertex(const MeshVertex & v)
{
	m_vertices.push_back(v);
	++m_vertexCount;
}

void Mesh::MeshAsset::add_indices(uint32_t i)
{
	m_indices.push_back(i);
	++m_indexCount;
}

void Mesh::MeshAsset::add_texture_descriptor(const TextureDescriptor & t)
{
	m_textureDescriptors.push_back(t);
	++m_textureCount;
}

void Mesh::MeshAsset::set_material(const MeshMaterial & material)
{
	m_meshMaterial = material;
}

const MeshMaterial & Mesh::MeshAsset::get_material() const
{
	return m_meshMaterial;
}

void Mesh::MeshAsset::set_texcoord_transform(const XMVECTOR & transform)
{
	m_texcoordTransform = transform;
}

const XMVECTOR & Mesh::MeshAsset::get_texcoord_transform() const
{
	return m_texcoordTransform;
}

void Mesh::MeshAsset::set_bounding_box(const Math::BoundingBox & aabb)
{
	m_boundingBox = aabb;
}

const Math::BoundingBox & Mesh::MeshAsset::get_bounding_box() const
{
	return m_boundingBox;
}
