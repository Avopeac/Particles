#pragma once

#include "MeshTexture.h"

namespace Mesh 
{
	using namespace Direct3D;
	using namespace DirectX;

	// The mesh texture store class contain mesh texture objects in an unordered map. The map uses a key value of the hashed texture file path.
	// Using the hash, the worst case O(n) of searching string keys is avoided and we can be guaranteed O(1) access to the texture.
	// Author: Andreas Larsson
	// Date: 2016-12-09 (YYYY-MM-DD)
	class MeshTextureStore 
	{
		std::unordered_map<size_t, std::unique_ptr<MeshTexture>> m_store;

	public:
		MeshTextureStore();
		~MeshTextureStore();
		MeshTextureStore(const MeshTextureStore &) = delete;
		MeshTextureStore(MeshTextureStore &&) = delete;
		MeshTextureStore &operator=(const MeshTextureStore &) = delete;
		MeshTextureStore &operator=(MeshTextureStore &&) = delete;

		// NOTE: Add the texture to the store
		void add_texture(TextureFileType fileType, size_t hash, const std::string &path);

		// NOTE: Load all the added textures, if textures are already loaded they are skipped
		bool load_added_textures(const D3DDevice &device);

		// NOTE: Get a texture based on hash
		const MeshTexture * get_texture(size_t hash) const;

		// NOTE: Does texture with the given hash exist in store?
		bool contains_texture(size_t hash) const;

	private:
		
	};
}
