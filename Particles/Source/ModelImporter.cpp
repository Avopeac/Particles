#include <Precompiled.h>

#include "ModelImporter.h"

using namespace Mesh;

Mesh::ModelImporter::ModelImporter(MeshTextureStore &store) :
	m_store(store)
{

}

Mesh::ModelImporter::~ModelImporter()
{

}

bool Mesh::ModelImporter::load(const std::string & file, std::vector<MeshAsset> & meshes)
{
	// NOTE: Converting to left-handed coordinate system because of DirectX, we always use triangles as primitives
	const aiScene * const scene = m_importer.ReadFile(file, aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_CalcTangentSpace);

	// NOTE: Check if valid scene
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
		return false;
	}
	
	// NOTE: Process each node in the scene graph recursively
	process_node(scene->mRootNode, scene, meshes);

	return true;
}

void Mesh::ModelImporter::process_node(const aiNode * const node, const aiScene * const scene, std::vector<MeshAsset> &meshes)
{

	uint32_t meshCount = node->mNumMeshes;
	uint32_t childCount = node->mNumChildren;

	// NOTE: Node can contain several meshes
	for (uint32_t index = 0; index < meshCount; ++index) 
	{
		uint32_t meshIndex = node->mMeshes[index];
		const aiMesh * const mesh = scene->mMeshes[meshIndex];
		meshes.push_back(process_mesh(mesh, scene));
	}

	// NOTE: Node can contain several child nodes
	for (uint32_t index = 0; index < childCount; ++index) 
	{
		const aiNode * const child_node = node->mChildren[index];
		process_node(child_node, scene, meshes);
	}
}

MeshAsset Mesh::ModelImporter::process_mesh(const aiMesh * const mesh, const aiScene * const scene)
{
	MeshAsset meshAsset;
	std::vector<uint32_t> colorIndices;
	std::vector<uint32_t> texcoordIndices;

	// NOTE: Mesh can have more than one color attribute
	for (uint32_t index = 0; index < mesh->GetNumColorChannels(); ++index) 
	{
		if (mesh->HasVertexColors(index)) {
			colorIndices.push_back(index);
		}
	}
	
	// NOTE: Mesh can have more than one texture coordinate attribute
	for (uint32_t index = 0; index < mesh->GetNumUVChannels(); ++index)
	{
		if (mesh->HasTextureCoords(index)) {
			texcoordIndices.push_back(index);
		}
	}

	// NOTE: For each vertex in the model file, add to mesh asset
	aiVector3D position = aiVector3D(0, 0, 0);
	aiVector3D normal = aiVector3D(0, 0, 0);
	aiVector3D tangent = aiVector3D(0, 0, 0);
	aiVector3D bitangent = aiVector3D(0, 0, 0);
	aiColor4D color = aiColor4D(1, 1, 1, 1);
	aiVector3D uv = aiVector3D(0, 0, 0);
	
	float minU = (std::numeric_limits<float>().max)();
	float maxU = std::numeric_limits<float>().lowest();
	float minV = minU;
	float maxV = maxU;

	float minX = (std::numeric_limits<float>().max)();
	float maxX = std::numeric_limits<float>().lowest();
	float minY = (std::numeric_limits<float>().max)();
	float maxY = std::numeric_limits<float>().lowest();
	float minZ = (std::numeric_limits<float>().max)();
	float maxZ = std::numeric_limits<float>().lowest();

	for (uint32_t index = 0; index < mesh->mNumVertices; ++index) 
	{
		position = mesh->mVertices[index];

		if (mesh->HasNormals()) 
		{
			normal = mesh->mNormals[index];
		}

		if (mesh->HasTangentsAndBitangents())
		{
			tangent = mesh->mTangents[index];
			bitangent = mesh->mBitangents[index];
		}
		
		// TODO: Add support for more color channels
		for (uint32_t channel : colorIndices) 
		{
			color = mesh->mColors[channel][index];
			break;
		}

		// TODO: Add support for more uv channels
		for (uint32_t channel : texcoordIndices) 
		{
			uv = mesh->mTextureCoords[channel][index];
			break;
		}

		minU = minU > uv.x ? uv.x : minU;
		minV = minV > uv.y ? uv.y : minV;
		maxU = maxU < uv.x ? uv.x : maxU;
		maxV = maxV < uv.y ? uv.y : maxV;

		minX = minX > position.x ? position.x : minX;
		minY = minY > position.y ? position.y : minY;
		minZ = minZ > position.z ? position.z : minZ;
		maxX = maxX < position.x ? position.x : maxX;
		maxY = maxY < position.y ? position.y : maxY;
		maxZ = maxZ < position.z ? position.z : maxZ;

		// TODO: Set the mesh vertex and add it to the mesh asset
		MeshVertex vertex;
		ZeroMemory(&vertex, sizeof(vertex));
		vertex.position = XMFLOAT3(position.x, position.y, position.z);
		vertex.normal = XMFLOAT3(normal.x, normal.y, normal.z);
		vertex.tangent = XMFLOAT3(tangent.x, tangent.y, tangent.z);
		vertex.bitangent = XMFLOAT3(bitangent.x, bitangent.y, bitangent.z);
		vertex.color = XMFLOAT3(color.r, color.g, color.b);
		vertex.uv = XMFLOAT2(uv.x, uv.y);
		meshAsset.add_vertex(vertex);
	}

	// NOTE: For each face in the mesh node add indices to mesh asset
	for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) 
	{
		aiFace face = mesh->mFaces[faceIndex];
		for (uint32_t index = 0; index < face.mNumIndices; ++index) 
		{
			uint32_t faceIndex = face.mIndices[index];
			meshAsset.add_indices(faceIndex);
		}
	}

	// NOTE: If the mesh node has materials, process it and add textures
	if (mesh->mMaterialIndex >= 0) 
	{
		const aiMaterial * const material = scene->mMaterials[mesh->mMaterialIndex];
		
		aiColor3D diffuse;
		aiColor3D specular;
		aiColor3D ambient;
		float transparency;
		float specularPower;

		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
		material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
		material->Get(AI_MATKEY_OPACITY, transparency);
		material->Get(AI_MATKEY_SHININESS, specularPower);

		MeshMaterial meshMaterial;
		meshMaterial.set_ambient_color(XMFLOAT3(ambient.r, ambient.g, ambient.b));
		meshMaterial.set_diffuse_color(XMFLOAT3(diffuse.r, diffuse.g, diffuse.b), transparency);
		meshMaterial.set_specular_color(XMFLOAT3(specular.r, specular.g, specular.b), specularPower);
		meshAsset.set_material(meshMaterial);

		std::vector<TextureDescriptor> diffuseTextures = process_material(material, aiTextureType_DIFFUSE);
		std::vector<TextureDescriptor> normalTextures = process_material(material, aiTextureType_NORMALS);
		std::vector<TextureDescriptor> displacementTextures = process_material(material, aiTextureType_DISPLACEMENT);
		std::vector<TextureDescriptor> heightTextures = process_material(material, aiTextureType_HEIGHT);
		std::vector<TextureDescriptor> specularTextures = process_material(material, aiTextureType_SPECULAR);
		std::vector<TextureDescriptor> shininessTextures = process_material(material, aiTextureType_SHININESS);
		std::vector<TextureDescriptor> maskTextures = process_material(material, aiTextureType_OPACITY);

		for (auto &t : diffuseTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : normalTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : heightTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : displacementTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : specularTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : shininessTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : maskTextures)
		{
			meshAsset.add_texture_descriptor(t);
		}

		for (auto &t : meshAsset.get_texture_descriptors())
		{

			std::string extension = t.path.substr(t.path.find_last_of('.') + 1);
			TextureFileType type = TextureFileType::TEXTURE_FILE_UNKNOWN;
			
			for (uint32_t index = 0; index < NUM_TGA_EXTENSIONS; ++index)
			{
				if (extension == TGA_EXTENSIONS[index])
				{
					type = TextureFileType::TEXTURE_FILE_TGA;
					break;
				}
			}

			if (type == TextureFileType::TEXTURE_FILE_UNKNOWN)
			{
				for (uint32_t index = 0; index < NUM_WIC_EXTENSIONS; ++index)
				{
					if (extension == WIC_EXTENSIONS[index])
					{
						type = TextureFileType::TEXTURE_FILE_WIC;
						break;
					}
				}
			}

			if (type == TextureFileType::TEXTURE_FILE_UNKNOWN)
			{
				for (uint32_t index = 0; index < NUM_DDS_EXTENSIONS; ++index)
				{
					if (extension == DDS_EXTENSIONS[index])
					{
						type = TextureFileType::TEXTURE_FILE_DDS;
						break;
					}
				}
			}

			m_store.add_texture(type, t.hash, t.path);
		}
	}
	
	// NOTE: Scale and bias UV between 0 and 1
	minU = minU < 0.0f ? -minU : 0.0f;
	minV = minV < 0.0f ? -minV : 0.0f;
	maxU += minU;
	maxV += minV;
	maxU = maxU > 1.0f ? maxU : 1.0f;
	maxV = maxV > 1.0f ? maxV : 1.0f;
	meshAsset.set_texcoord_transform(XMVectorSet(maxU, maxV, minU, minV));

	// NOTE: Set bounding box
	Math::BoundingBox boundingBox;
	boundingBox.set(XMVectorSet(minX, minY, minZ, 1), XMVectorSet(maxX, maxY, maxZ, 1));
	meshAsset.set_bounding_box(boundingBox);

	return meshAsset;
}

std::vector<TextureDescriptor> Mesh::ModelImporter::process_material(const aiMaterial * material, aiTextureType type)
{
	aiString path;
	TextureType descriptorType;
	TextureDescriptor texture;
	std::vector<TextureDescriptor> textures;
	
	// NOTE: Check texture type
	switch (type) 
	{
		default:
		
		case aiTextureType_DIFFUSE: 
		{ 
			descriptorType = TextureType::TEXTURE_TYPE_DIFFUSE; 
		} 
		break;

		case aiTextureType_DISPLACEMENT: 
		{
			descriptorType = TextureType::TEXTURE_TYPE_NORMAL; 
		}
		break;

		case aiTextureType_NORMALS:
		{
			descriptorType = TextureType::TEXTURE_TYPE_NORMAL;
		}
		break;

		case aiTextureType_HEIGHT:
		{
			descriptorType = TextureType::TEXTURE_TYPE_NORMAL;
		}
		break;

		case aiTextureType_SPECULAR: 
		{
			descriptorType = TextureType::TEXTURE_TYPE_SPECULAR; 
		} 
		break;

		case aiTextureType_SHININESS:
		{
			descriptorType = TextureType::TEXTURE_TYPE_SPECULAR;
		}
		break;
		
		case aiTextureType_OPACITY: 
		{
			descriptorType = TextureType::TEXTURE_TYPE_MASK; 
		} 
		break;
	}


	// NOTE: Load the textures for the texture type
	uint32_t textureCount = material->GetTextureCount(type);
	for (uint32_t index = 0; index < textureCount; ++index) 
	{
		material->GetTexture(type, index, &path);
		// NOTE: Save texture information to be stored in texture storage
		texture.path = path.C_Str();
		texture.hash = std::hash<std::string>{}(texture.path);
		texture.type = descriptorType;
		textures.push_back(texture);
	}

	return textures;
}
