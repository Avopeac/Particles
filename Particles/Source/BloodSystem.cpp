#include <Precompiled.h>
#include "BloodSystem.h"

using namespace ParticleExamples;

ParticleExamples::BloodSystem::BloodSystem(const D3DDevice & device, const ParticleEmitterShaders & shaders) :
	blood(device, shaders),
	system(device, 1024 * 1024, ParticleSystemBlendMode::BLEND_MULTIPLICATIVE)
{

}

ParticleExamples::BloodSystem::~BloodSystem()
{

}

ParticleSystemBase * ParticleExamples::BloodSystem::create(const XMFLOAT3 & position, DeferredGeometryPass * const deferred, D3DTimeBuffer * const time, PerspectiveCamera * const camera, ParticleSystemSharedResources * const resources, ParticleSystemTextureArray * const textures)
{
	// NOTE: Create blood particle emitter
	{
		blood.set_volume(true);
		blood.set_paused(false);
		blood.set_radius(0.01f);
		blood.set_constant_acceleration(XMFLOAT3(0.0f, -12.82f, 0.0f));
		blood.set_lifetime(3.0f);
		blood.set_spawn_time(0.0f);
		blood.set_rate(20.0f);
		blood.set_speed(1.0f);
		blood.set_position(XMFLOAT3(0.0f, 0.0f, 0.0f));
		blood.set_initial_velocity(XMFLOAT3(0.0f, 1.0f, 0.0f));
		blood.set_collision_behaviour(true, 0.4f, 0.5f);
		blood.set_rotation(XMFLOAT4(0.0f, 1.14f, 2.14f, 3.14f));
		blood.set_stretching_behaviour(true, 1.0f);
		blood.set_painting_behaviour(true, XMFLOAT4(0.05f, 0.0f, 0.0f, 0.7f));
		blood.set_size(XMFLOAT2(0.005f, 0.005f), XMFLOAT2(0.01f, 0.01f), XMFLOAT2(0.009f, 0.000f), XMFLOAT2(0.005f, 0.005f));
		blood.set_color(XMFLOAT4(0.15f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.15f, 0.0f, 0.12f, 1.0f), XMFLOAT4(0.15f, 0.0f, 0.05f, 1.0f), XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f));
	}

	// NOTE: Set up particle system
	{
		system.set_position(position);
		system.set_time_buffer(time);
		system.set_camera(camera);
		system.set_shared_resources(resources);
		system.set_texture_array(textures);
		system.set_deferred_renderer(deferred);
		system.add_emitter(&blood, "Textures/splat_particle.png");

		Math::BoundingBox particleSystemBoundingBox;
		particleSystemBoundingBox.set(XMVectorSet(-1.0f, -2.0f, -1.0f, 1), XMVectorSet(1.0f, 2.0f, 1.0f, 1));
		system.set_bounding_box(particleSystemBoundingBox);

		if (!system.initialize())
		{
			return nullptr;
		}
	}

	return &system;
}

void ParticleExamples::BloodSystem::set_velocity(const XMFLOAT3 & velocity)
{
	blood.set_initial_velocity(velocity);
}
