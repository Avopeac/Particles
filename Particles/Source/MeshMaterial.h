#pragma once
#include <precompiled.h>

namespace Mesh
{
	using namespace DirectX;

	class MeshMaterial
	{
		float m_specularPower;
		XMFLOAT3 m_ambientColor;
		XMFLOAT3 m_diffuseColor;
		XMFLOAT3 m_specularColor;

	public:
		MeshMaterial();
		~MeshMaterial();
		MeshMaterial(const MeshMaterial &);
		MeshMaterial(MeshMaterial &&);

		MeshMaterial &operator=(const MeshMaterial &other)
		{
			if (this != &other)
			{
				m_specularPower = other.m_specularPower;
				m_ambientColor = other.m_ambientColor;
				m_diffuseColor = other.m_diffuseColor;
				m_specularColor = other.m_specularColor;
			}

			return *this;
		}

		MeshMaterial &operator=(MeshMaterial &&other)
		{
			if (this != &other)
			{
				m_specularPower = other.m_specularPower;
				m_ambientColor = other.m_ambientColor;
				m_diffuseColor = other.m_diffuseColor;
				m_specularColor = other.m_specularColor;
			}

			return *this;
		}

		// NOTE: Set the ambient color
		void set_ambient_color(const XMFLOAT3 &ambient);

		// NOTE: Set the diffuse color
		void set_diffuse_color(const XMFLOAT3 &diffuse, float opacity);

		// NOTE: Set the specular color
		void set_specular_color(const XMFLOAT3 &specular, float specularity);

		// NOTE: Get the specular power
		float get_specular_power() const;

		// NOTE: Get ambient color
		const XMFLOAT3 &get_ambient_color() const;

		// NOTE: Get diffuse color
		const XMFLOAT3 &get_diffuse_color() const;

		// NOTE: Get specular color
		const XMFLOAT3 &get_specular_color() const;

	private:
	
	};
}
