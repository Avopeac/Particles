#include <Precompiled.h>

#include "FireSystem.h"

using namespace ParticleExamples;

ParticleExamples::FireSystem::FireSystem(const D3DDevice &device, const ParticleEmitterShaders &shaders) :
	sparks(device, shaders),
	fire(device, shaders),
	smoke(device, shaders),
	system(device, 40000, ParticleSystemBlendMode::BLEND_ADDITIVE)
{

}

ParticleExamples::FireSystem::~FireSystem()
{

}

ParticleSystemBase * ParticleExamples::FireSystem::create(const XMFLOAT3 &position, 
	DeferredGeometryPass * const deferred,
	D3DTimeBuffer * const time, 
	PerspectiveCamera * const camera,
	ParticleSystemSharedResources * const resources, 
	ParticleSystemTextureArray * const textures)
{

	// NOTE: Create smoke particle emitter
	{
		smoke.set_volume(true);
		smoke.set_paused(false);
		smoke.set_cone_angle(10.0f);
		smoke.set_cone_height(0.2f);
		smoke.set_cone_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f));
		smoke.set_constant_acceleration(XMFLOAT3(0.0f, 0.0f, 0.0f));
		smoke.set_lifetime(3.0f);
		smoke.set_spawn_time(0.5f);
		smoke.set_rate(1.0f);
		smoke.set_speed(2.0f);
		smoke.set_position(XMFLOAT3(0.0f, 0.0f, 0.0f));
		smoke.set_initial_velocity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		smoke.set_collision_behaviour(false, 0.0f, 0.0f);
		smoke.set_stretching_behaviour(false, 0.0f);
		smoke.set_sleeping_behaviour(false, 0);
		smoke.set_rotation(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
		smoke.set_size(XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.8f, 0.8f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(0.3f, 0.3f));
		smoke.set_color(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	}
	
	// NOTE: Create fire particle emitter
	{
		fire.set_cone_height(1.0f);
		fire.set_cone_angle(15.0f);
		fire.set_cone_rotation(XMFLOAT3(180, 0, 0));
		fire.set_volume(true);
		fire.set_paused(false);
		fire.set_collision_behaviour(true, 0.1f, 0.2f);
		fire.set_stretching_behaviour(false, 0.0f);
		fire.set_sleeping_behaviour(false, 0);
		fire.set_painting_behaviour(true, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		fire.set_constant_acceleration(XMFLOAT3(0.0f, 6.0f, 0.0f));
		fire.set_initial_velocity(XMFLOAT3(0.0f, -0.4f, 0.0f));
		fire.set_lifetime(1.0f);
		fire.set_spawn_time(0.0f);
		fire.set_rate(25.0f);
		fire.set_speed(0.00f);
		fire.set_position(XMFLOAT3(0.0f, 0.4f, 0.0f));
		fire.set_rotation(XMFLOAT4(0.0f, 1.14f, 2.14f, 3.14f));
		fire.set_size(XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.1f, 0.1f), XMFLOAT2(0.05f, 0.05f), XMFLOAT2(0.0f, 0.0f));
		fire.set_color(XMFLOAT4(15.0f, 15.0f, 25.0f, 0.0f), XMFLOAT4(15.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(15.0f, 15.0f, 0.0f, 1.0f), XMFLOAT4(15.0f, 0.0f, 0.0f, 0.0f));
	}

	// NOTE: Create sparks particle emitter
	{
		sparks.set_volume(true);
		sparks.set_paused(false);
		sparks.set_cone_angle(45.0f);
		sparks.set_cone_height(0.2f);
		sparks.set_cone_rotation(XMFLOAT3(180.0f, 0.0f, 180.0f));
		sparks.set_constant_acceleration(XMFLOAT3(0.0f, -9.82f, 0.0f));
		sparks.set_lifetime(2.0f);
		sparks.set_spawn_time(0.3f);
		sparks.set_rate(3.0f);
		sparks.set_speed(2.0f);
		sparks.set_position(XMFLOAT3(0.0f, 0.6f, 0.0f));
		sparks.set_initial_velocity(XMFLOAT3(0.0f, 2.0f, 0.0f));
		sparks.set_collision_behaviour(true, 0.99f, 0.6f);
		sparks.set_painting_behaviour(false, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.1f));
		sparks.set_stretching_behaviour(true, 1.0f);
		sparks.set_sleeping_behaviour(true, 100);
		sparks.set_rotation(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
		sparks.set_size(XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.005f, 0.005f), XMFLOAT2(0.005f, 0.005f), XMFLOAT2(0.000f, 0.000f));
		sparks.set_color(XMFLOAT4(5.0f, 5.0f, 15.0f, 0.0f), XMFLOAT4(5.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(5.0f, 5.0f, 0.0f, 1.0f), XMFLOAT4(5.0f, 0.0f, 0.0f, 0.0f));
	}
	
	// NOTE: Set up particle system
	{
		system.set_position(position);
		system.set_time_buffer(time);
		system.set_camera(camera);
		system.set_shared_resources(resources);
		system.set_texture_array(textures);
		system.set_deferred_renderer(deferred);
		
		system.add_emitter(&fire, "Textures/fire_particle.png");
		system.add_emitter(&smoke, "Textures/smoke.png");
		system.add_emitter(&sparks, "Textures/white_square.png");

		Math::BoundingBox particleSystemBoundingBox;
		particleSystemBoundingBox.set(XMVectorSet(-0.5f, -1.0f, -0.5f, 1), XMVectorSet(0.5f, 1.0f, 0.5f, 1));
		system.set_bounding_box(particleSystemBoundingBox);

		Math::SplineChain chain;
		chain.add_spline(Math::Spline(XMVectorSet(position.x, position.y, 1.0f - position.z, 1), XMVectorSet(-5 + position.x, position.y, position.z, 1), XMVectorSet(-10 + position.x, position.y, position.z, 1), XMVectorSet(-15 + position.x, position.y, position.z, 1), 10));
		chain.add_spline(Math::Spline(XMVectorSet(-15 + position.x, position.y, position.z, 1), XMVectorSet(-10 + position.x, position.y, position.z, 1), XMVectorSet(-5 + position.x, position.y, position.z, 1), XMVectorSet(position.x, position.y, 1.0f - position.z, 1), 10));
		chain.set_looping(true);
		system.set_spline_chain(chain);

		if (!system.initialize())
		{
			return nullptr;
		}
	}

	return &system;
}
