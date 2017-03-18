#include <Precompiled.h>

#include "ParticleEmitterCube.h"

using namespace Particles;
using namespace Direct3D;

ParticleEmitterCube::ParticleEmitterCube(const D3DDevice &device, const ParticleEmitterShaders &shaders) : ParticleEmitterBase(device, shaders, ParticleEmitterShape::SHAPE_CUBE) 
{
	m_shaderDataBuffer.create(device);
	m_pEmitterShapeBuffer = m_shaderDataBuffer.get_data();
}

ParticleEmitterCube::~ParticleEmitterCube() 
{

}

ID3D11Buffer * const ParticleEmitterCube::map_emitter_specific_data()
{
	m_shaderData.rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_eulerAngles.x), XMConvertToRadians(m_eulerAngles.y), XMConvertToRadians(m_eulerAngles.z));
	m_shaderData.lengths = m_sideLengths;
	m_shaderDataBuffer.map_data(m_device.get_context(), &m_shaderData);
	return m_shaderDataBuffer.get_data();
}

void Particles::ParticleEmitterCube::set_cube_rotation(const XMFLOAT3 & eulerAngles)
{
	m_eulerAngles = eulerAngles;
}

void Particles::ParticleEmitterCube::set_cube_lengths(const XMFLOAT3 & lengths)
{
	m_sideLengths = lengths;
}
