#pragma once
#include "D3DDevice.h"

namespace Mesh 
{

	using namespace Direct3D;
	using namespace DirectX;

	enum TextureType 
	{
		TEXTURE_TYPE_DIFFUSE,
		TEXTURE_TYPE_SPECULAR,
		TEXTURE_TYPE_NORMAL,
		TEXTURE_TYPE_MASK,
	};

	struct TextureDescriptor 
	{
		std::size_t hash;
		std::string path;
		TextureType type;
	};

	enum TextureFileType 
	{
		TEXTURE_FILE_DDS,
		TEXTURE_FILE_WIC,
		TEXTURE_FILE_TGA,
		TEXTURE_FILE_UNKNOWN,
	};

	const uint32_t NUM_WIC_EXTENSIONS = 4;
	static const std::string WIC_EXTENSIONS[NUM_WIC_EXTENSIONS] = 
	{
		"jpg",
		"JPG",
		"png",
		"PNG",
	};

	const uint32_t NUM_TGA_EXTENSIONS = 2;
	static const std::string TGA_EXTENSIONS[NUM_TGA_EXTENSIONS] =
	{
		"tga",
		"TGA",
	};

	const uint32_t NUM_DDS_EXTENSIONS = 2;
	static const std::string DDS_EXTENSIONS[NUM_DDS_EXTENSIONS] =
	{
		"dds",
		"DDS",
	};

	// The mesh texture class has several texture attributes, when the attributes are set to valid values the load function can be called successfully.
	// These mesh textures can be added to the texture store and be looked up via the supplied hash.
	// The hash is just a std::hash of the std::string path.
	// Author: Andreas Larsson
	// Date: 2016-12-09 (YYYY-MM-DD)
	class MeshTexture
	{
		bool m_loaded;
		size_t m_texturePathHash;
		std::string m_texturePath;
		XMFLOAT4 m_textureSize;
		TextureFileType m_supportedFileType;
		ComPtr<ID3D11Resource> m_pResource;
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;

	public:
		MeshTexture();
		~MeshTexture();
		MeshTexture(const MeshTexture &) = delete;
		MeshTexture(MeshTexture &&) = delete;
		MeshTexture &operator=(const MeshTexture &) = delete;
		MeshTexture &operator=(MeshTexture &&) = delete;

		// NOTE: Load a texture from file 
		bool load(const D3DDevice &device);

		// NOTE: Set the path for the texture (used to create hash)
		void set_path(const std::string &path);

		// NOTE: Set texture file type based on what is supported in DirectXTex
		void set_filetype(TextureFileType filetype);

		// NOTE: If the texture has been loaded previously
		bool is_loaded() const;

		// NOTE: Get the hash for the texture
		size_t get_hash() const;
		
		// NOTE: Get the texture size
		const XMFLOAT4 &get_size() const;

		// NOTE: Get the texture path
		const std::string &get_path() const;

		ID3D11ShaderResourceView * get_srv() const;

	private:
		std::wstring string_to_wchar_t(const std::string & str);
		
	};
}
