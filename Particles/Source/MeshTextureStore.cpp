#include <Precompiled.h>

#include "MeshTextureStore.h"

using namespace Mesh;

Mesh::MeshTextureStore::MeshTextureStore()
{

}

Mesh::MeshTextureStore::~MeshTextureStore()
{

}

void Mesh::MeshTextureStore::add_texture(TextureFileType filetype, size_t hash, const std::string &path)
{
	// NOTE: Do not add textures that are already in the map
	if (contains_texture(hash)) 
	{
		return;
	}

	// NOTE: Set mesh texture attributes and insert in map
	std::unique_ptr<MeshTexture> texture = std::unique_ptr<MeshTexture>(new MeshTexture());
	texture->set_filetype(filetype);
	texture->set_path(path);
	m_store[hash] = std::move(texture);
}

bool Mesh::MeshTextureStore::load_added_textures(const D3DDevice &device)
{
	// NOTE: The mesh texture has a "loaded" flag if it has previously been loaded successfully, calling this will not load them again
	bool success = true;
	for (auto &ptr : m_store)
	{
		if (!ptr.second->is_loaded() && !ptr.second->load(device))
		{
			success = false;
			break;
		}
	}

	return success;
}

const MeshTexture * Mesh::MeshTextureStore::get_texture(size_t hash) const
{
	// NOTE: Fetch the texture with the supplied hash
	MeshTexture * texture = 0;
	if (m_store.find(hash) != m_store.end())
	{
		texture = m_store.at(hash).get();
	}

	return texture;
}

bool Mesh::MeshTextureStore::contains_texture(size_t hash) const
{
	return m_store.find(hash) != m_store.end();
}
