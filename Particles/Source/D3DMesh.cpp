#include <Precompiled.h>

#include "D3DMesh.h"

using namespace Direct3D;
using namespace Direct3DResource;

uint32_t D3DMesh::g_meshNumber = 0;

D3DMesh::D3DMesh() :
	m_vertexCount(0), 
	m_indexCount(0),
	m_pAdditionalTexture(0),
	m_meshNumber(g_meshNumber++) 
{
	m_worldTransform = XMMatrixIdentity();
	m_texcoordTransform = XMVectorZero();
}

D3DMesh::~D3DMesh() 
{

}

D3DMesh::D3DMesh(const D3DMesh &other)
{
	assert(this != &other);
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

D3DMesh::D3DMesh(D3DMesh &&other)
{
	assert(this != &other);
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

void Direct3DResource::D3DMesh::set_world_transform(const XMMATRIX & transform)
{
	m_worldTransform = transform;
}

uint32_t Direct3DResource::D3DMesh::get_index_count() const
{
	return m_indexCount;
}

uint32_t Direct3DResource::D3DMesh::get_mesh_number() const
{
	return m_meshNumber;
}

void Direct3DResource::D3DMesh::set_additional_texture(ID3D11ShaderResourceView * const srv) const
{
	m_pAdditionalTexture = srv;
}

ID3D11ShaderResourceView * const Direct3DResource::D3DMesh::get_additional_texture() const
{
	return m_pAdditionalTexture;
}

const XMMATRIX & Direct3DResource::D3DMesh::get_world_transform() const
{
	return m_worldTransform;
}

const MeshMaterial & Direct3DResource::D3DMesh::get_material() const
{
	return m_meshMaterial;
}

const Math::BoundingBox & Direct3DResource::D3DMesh::get_bounding_box() const
{
	return m_boundingBox;
}

const XMVECTOR & Direct3DResource::D3DMesh::get_texcoord_transform() const
{
	return m_texcoordTransform;
}

const std::vector<TextureDescriptor>& Direct3DResource::D3DMesh::get_textures() const
{
	return m_textureDescriptors;
}

bool D3DMesh::initialize(const D3DDevice &device, const MeshAsset &mesh_asset) {

	// NOTE: Create vertex buffer
	{
		m_vertexCount = mesh_asset.get_vertex_count();
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&data, sizeof(data));
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = sizeof(MeshVertex) * m_vertexCount;
		desc.Usage = D3D11_USAGE_DEFAULT;
		data.pSysMem = &mesh_asset.get_vertices()[0];
		HRESULT hr = device.get_device()->CreateBuffer(&desc, &data, m_pVertexBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxA(0, "Could not create vertex buffer.", 0, 0);
			return false;
		}
	}

	// NOTE: Create index buffer
	{
		m_indexCount = mesh_asset.get_index_count();
		D3D11_BUFFER_DESC desc;
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&desc, sizeof(desc));
		ZeroMemory(&data, sizeof(data));
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = sizeof(uint32_t) * m_indexCount;
		desc.Usage = D3D11_USAGE_DEFAULT;
		data.pSysMem = &mesh_asset.get_indices()[0];
		HRESULT hr = device.get_device()->CreateBuffer(&desc, &data, m_pIndexBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxA(0, "Could not create index buffer.", 0, 0);
			return false;
		}
	}

	m_worldTransform = mesh_asset.get_world_transform();
	m_texcoordTransform = mesh_asset.get_texcoord_transform();
	m_meshMaterial = mesh_asset.get_material();

	m_boundingBox = mesh_asset.get_bounding_box();
	m_boundingBox.transform(m_worldTransform);

	// NOTE: Save texture information so that the texture hash can be queried from the texture store
	m_textureDescriptors = mesh_asset.get_texture_descriptors();

	return true;
}

void D3DMesh::render(const D3DDevice &device) const
{
	uint32_t stride = sizeof(MeshVertex);
	uint32_t offset = 0;
	device.get_context()->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);
	device.get_context()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	device.get_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}