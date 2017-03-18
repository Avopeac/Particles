#include <Precompiled.h>

#include "ParticleEmitterBase.h"

using namespace Particles;
using namespace Direct3D;

ParticleEmitterBase::ParticleEmitterBase(const D3DDevice &device, const ParticleEmitterShaders &shaders, ParticleEmitterShape shape) :
	m_device(device), 
	m_emitterShape(shape),
	m_paused(false),
	m_pShader(0)
{
	m_position = XMFLOAT3(0, 0, 0);
	m_parentPosition = XMFLOAT3(0, 0, 0);

	// NOTE: Zero out all properties
	ZeroMemory(&m_emitterProperties, sizeof(m_emitterProperties));
	ZeroMemory(&m_globalEmitterProperties, sizeof(m_globalEmitterProperties));

	// NOTE: Get emitter specific shader from particle emitter shader store
	m_pShader = shaders.get_emitter_shader(shape);

	// NOTE: Create the emitter buffer
	m_emitterBuffer.create(device);
}

ParticleEmitterBase::~ParticleEmitterBase()
{

}

D3DShader * Particles::ParticleEmitterBase::get_shader() const
{
	return m_pShader;
}

ParticleEmitterShape Particles::ParticleEmitterBase::get_shape() const
{
	return m_emitterShape;
}

void Particles::ParticleEmitterBase::set_texture_index(float index)
{
	m_emitterProperties.texture = index;
}

void Particles::ParticleEmitterBase::set_emitter_index(float index)
{
	m_emitterProperties.index = index;
}

const GlobalParticleEmitterDataSingle & Particles::ParticleEmitterBase::get_global_emitter_data() const
{
	return m_globalEmitterProperties;
}

const ParticleEmitterCommonData & Particles::ParticleEmitterBase::get_emitter_data() const
{
	return m_emitterProperties;
}

bool Particles::ParticleEmitterBase::is_paused() const
{
	return m_paused;
}

void Particles::ParticleEmitterBase::set_volume(bool volume)
{
	m_emitterProperties.volume = volume ? 1.0f : 0.0f;
}

void Particles::ParticleEmitterBase::set_paused(bool pause)
{
	m_paused = pause;
}

void Particles::ParticleEmitterBase::set_speed(float speed)
{
	m_emitterProperties.speed = speed;
}

void Particles::ParticleEmitterBase::set_spawn_time(float time)
{
	m_emitterProperties.spawn = time;
}

void Particles::ParticleEmitterBase::set_lifetime(float time)
{
	m_emitterProperties.lifetime = time;
}

void Particles::ParticleEmitterBase::set_rate(float rate)
{
	m_emitterProperties.emission = rate;
}

void Particles::ParticleEmitterBase::set_position(const XMFLOAT3 & position)
{
	m_position = position;
}

void Particles::ParticleEmitterBase::set_parent_position(const XMFLOAT3 & position)
{
	m_parentPosition = position;
}

void Particles::ParticleEmitterBase::set_initial_velocity(const XMFLOAT3 & velocity)
{
	m_emitterProperties.velocity = velocity;
}

void Particles::ParticleEmitterBase::set_constant_acceleration(const XMFLOAT3 & acceleration)
{
	m_emitterProperties.acceleration = acceleration;
}

void Particles::ParticleEmitterBase::set_painting_behaviour(bool painting, const XMFLOAT4 &color)
{
	m_globalEmitterProperties.paintingEnabled = painting ? 1.0f : 0.0f;
	m_globalEmitterProperties.paintColor = color;
}

void Particles::ParticleEmitterBase::set_sleeping_behaviour(bool sleeping, uint32_t collisions)
{
	m_globalEmitterProperties.sleepingEnabled = sleeping ? 1.0f : 0.0f;
	m_globalEmitterProperties.sleepingCollisionFrames = static_cast<float>(collisions);
}

void Particles::ParticleEmitterBase::set_collision_behaviour(bool collisions, float dampening, float restitution)
{
	m_globalEmitterProperties.collisionsEnabled = collisions ? 1.0f : 0.0f;
	m_globalEmitterProperties.collisionDampening = dampening;
	m_globalEmitterProperties.restitutionFactor = restitution;
}

void Particles::ParticleEmitterBase::set_stretching_behaviour(bool stretching, float factor)
{
	m_globalEmitterProperties.stretchingEnabled = stretching ? 1.0f : 0.0f;
	m_globalEmitterProperties.stretchFactor = factor;
}

void Particles::ParticleEmitterBase::set_spline_chain(const Math::SplineChain & splineChain)
{
	m_splineChain = splineChain;
}

void Particles::ParticleEmitterBase::set_color(const XMFLOAT4 & color0, const XMFLOAT4 & color1, const XMFLOAT4 & color2, const XMFLOAT4 & color3)
{
	m_globalEmitterProperties.colorPoint0 = color0;
	m_globalEmitterProperties.colorPoint1 = color1;
	m_globalEmitterProperties.colorPoint2 = color2;
	m_globalEmitterProperties.colorPoint3 = color3;
}

void Particles::ParticleEmitterBase::set_rotation(const XMFLOAT4 & rotations)
{
	m_globalEmitterProperties.rotationPoints = rotations;
}

void Particles::ParticleEmitterBase::set_size(const XMFLOAT2 & size0, const XMFLOAT2 & size1, const XMFLOAT2 & size2, const XMFLOAT2 & size3)
{
	m_globalEmitterProperties.sizePoint0 = size0;
	m_globalEmitterProperties.sizePoint1 = size1;
	m_globalEmitterProperties.sizePoint2 = size2;
	m_globalEmitterProperties.sizePoint3 = size3;
}

ID3D11Buffer * const ParticleEmitterBase::map_emitter_common_data(float deltaTime) 
{
	if (!m_splineChain.empty())
	{
		XMStoreFloat3(&m_position, m_splineChain.update(deltaTime));
	}

	// NOTE: Set emitter position relative to parent position
	m_emitterProperties.emitterPosition = XMFLOAT3(m_position.x + m_parentPosition.x, 
		m_position.y + m_parentPosition.y,
		m_position.z + m_parentPosition.z);

	// NOTE: Map the general emitter data
	m_emitterBuffer.map_data(m_device.get_context(), &m_emitterProperties);
	return m_emitterBuffer.get_data();
}
