#pragma once

#include "MeshTexture.h"

namespace Particles
{
	using namespace Mesh;

	struct TexturePair
	{
		TextureFileType type;
		std::string path;
	};

	class ParticleSystemTextureArray
	{
		uint32_t m_width, m_height;
		ComPtr<ID3D11Texture2D> m_pTexture;
		ComPtr<ID3D11ShaderResourceView> m_pTextureShaderResourceView;
		std::vector<size_t> m_hashes;
		std::vector<TexturePair> m_textures;

	public:
		ParticleSystemTextureArray(uint32_t width, uint32_t height);
		~ParticleSystemTextureArray();
		ParticleSystemTextureArray(const ParticleSystemTextureArray &) = delete;
		ParticleSystemTextureArray(ParticleSystemTextureArray &&) = delete;
		ParticleSystemTextureArray &operator=(const ParticleSystemTextureArray &&) = delete;
		ParticleSystemTextureArray &operator=(ParticleSystemTextureArray &&) = delete;

		ID3D11ShaderResourceView * get_srv() const;
		void add(const std::string &path, TextureFileType type);
		bool load(const D3DDevice &device);
		int32_t find(const std::string &path) const;

	private:

		std::wstring string_to_wchar_t(const std::string & str);
	};
}
