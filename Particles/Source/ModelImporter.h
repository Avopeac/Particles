#pragma once

#include "BoundingBox.h"
#include "MeshAsset.h"
#include "MeshTexture.h"
#include "MeshTextureStore.h"

namespace Mesh 
{
	using namespace Assimp;
	using namespace Direct3D;

	// The mesh importer has the sole purpose of loading mesh data into a mesh asset structure from file.
	// Author: Andreas Larsson
	// Date: 2016-12-09 (YYYY-MM-DD)
	class ModelImporter 
	{
		Importer m_importer;
		MeshTextureStore &m_store;

	public:
		ModelImporter(MeshTextureStore &store);
		~ModelImporter();
		ModelImporter(const ModelImporter &) = delete;
		ModelImporter(ModelImporter &&) = delete;
		ModelImporter &operator=(const ModelImporter &) = delete;
		ModelImporter &operator=(ModelImporter &&) = delete;

		// NOTE: Load a model from file
		bool load(const std::string &file, std::vector<MeshAsset> &meshes);

	private:
		void process_node(const aiNode * const node, const aiScene * const scene, std::vector<MeshAsset> &meshes);
		MeshAsset process_mesh(const aiMesh * mesh, const aiScene * const scene);
		std::vector<TextureDescriptor> process_material(const aiMaterial * const material, aiTextureType type);
	};
}
