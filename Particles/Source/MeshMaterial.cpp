#include <Precompiled.h>

#include "MeshMaterial.h"

using namespace Mesh;
using namespace DirectX;

Mesh::MeshMaterial::MeshMaterial() :
	m_specularPower(1.0f)
{
	m_specularColor = XMFLOAT3(0, 0, 0);
	m_ambientColor = XMFLOAT3(0, 0, 0);
	m_diffuseColor = XMFLOAT3(0, 0, 0);
}

Mesh::MeshMaterial::~MeshMaterial()
{

}

Mesh::MeshMaterial::MeshMaterial(const MeshMaterial &other)
{
	assert(this != &other);
	m_specularPower = other.m_specularPower;
	m_ambientColor = other.m_ambientColor;
	m_diffuseColor = other.m_diffuseColor;
	m_specularColor = other.m_specularColor;
}

Mesh::MeshMaterial::MeshMaterial(MeshMaterial &&other)
{
	assert(this != &other);
	m_specularPower = other.m_specularPower;
	m_ambientColor = other.m_ambientColor;
	m_diffuseColor = other.m_diffuseColor;
	m_specularColor = other.m_specularColor;
}

void Mesh::MeshMaterial::set_ambient_color(const XMFLOAT3 & ambient)
{
	m_ambientColor = ambient;
}

void Mesh::MeshMaterial::set_diffuse_color(const XMFLOAT3 & diffuse, float opacity)
{
	m_diffuseColor = diffuse;
}

void Mesh::MeshMaterial::set_specular_color(const XMFLOAT3 & specular, float specularity)
{
	m_specularColor = specular;
	m_specularPower = specularity;
}

float Mesh::MeshMaterial::get_specular_power() const
{
	return m_specularPower;
}

const XMFLOAT3 & Mesh::MeshMaterial::get_ambient_color() const
{
	return m_ambientColor;
}

const XMFLOAT3 & Mesh::MeshMaterial::get_diffuse_color() const
{
	return m_diffuseColor;
}

const XMFLOAT3 & Mesh::MeshMaterial::get_specular_color() const
{
	return m_specularColor;
}
