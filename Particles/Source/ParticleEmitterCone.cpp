#include <Precompiled.h>

#include "ParticleEmitterCone.h"

using namespace Particles;
using namespace Direct3D;

ParticleEmitterCone::ParticleEmitterCone(const D3DDevice &device, const ParticleEmitterShaders &shaders) : ParticleEmitterBase(device, shaders, ParticleEmitterShape::SHAPE_CONE) 
{
	m_shaderDataBuffer.create(device);
	m_pEmitterShapeBuffer = m_shaderDataBuffer.get_data();
}

ParticleEmitterCone::~ParticleEmitterCone() 
{

}

ID3D11Buffer * const ParticleEmitterCone::map_emitter_specific_data()
{
	m_shaderData.rotation = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_eulerAngles.x), XMConvertToRadians(m_eulerAngles.y), XMConvertToRadians(m_eulerAngles.z));
	m_shaderData.angle = XMConvertToRadians(m_angle);
	m_shaderData.height = m_height;
	m_shaderDataBuffer.map_data(m_device.get_context(), &m_shaderData);
	return m_shaderDataBuffer.get_data();
}

void Particles::ParticleEmitterCone::set_cone_rotation(const XMFLOAT3 & eulerAngles)
{
	m_eulerAngles = eulerAngles;
}

void Particles::ParticleEmitterCone::set_cone_angle(float angle)
{
	m_angle = angle;
}

void Particles::ParticleEmitterCone::set_cone_height(float height)
{
	m_height = height;
}
