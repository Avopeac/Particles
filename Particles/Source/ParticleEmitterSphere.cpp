#include <Precompiled.h>

#include "ParticleEmitterSphere.h"

using namespace Particles;
using namespace Direct3D;

ParticleEmitterSphere::ParticleEmitterSphere(const D3DDevice &device, const ParticleEmitterShaders &shaders) : ParticleEmitterBase(device, shaders, ParticleEmitterShape::SHAPE_SPHERE) 
{
	m_shaderDataBuffer.create(device);
	m_pEmitterShapeBuffer = m_shaderDataBuffer.get_data();
}

ParticleEmitterSphere::~ParticleEmitterSphere() 
{

}

ID3D11Buffer * const ParticleEmitterSphere::map_emitter_specific_data()
{
	m_shaderData.radius = m_radius;
	m_shaderDataBuffer.map_data(m_device.get_context(), &m_shaderData);
	return m_shaderDataBuffer.get_data();
}

void Particles::ParticleEmitterSphere::set_radius(float radius)
{
	m_radius = radius;
}
