#include <Precompiled.h>
#include "TestSystem.h"

using namespace ParticleExamples;

ParticleExamples::TestSystem::TestSystem(const D3DDevice & device, const ParticleEmitterShaders & shaders) :
	test(device, shaders), system(device, 3 * 1024 * 1024, ParticleSystemBlendMode::BLEND_NONE)
{
}

ParticleExamples::TestSystem::~TestSystem() {
}

ParticleSystemBase * ParticleExamples::TestSystem::create(const XMFLOAT3 & position, DeferredGeometryPass * const deferred, D3DTimeBuffer * const time, PerspectiveCamera * const camera, ParticleSystemSharedResources * const resources, ParticleSystemTextureArray * const textures)
{
	test.set_volume(true);
	test.set_paused(false);
	test.set_cube_lengths(XMFLOAT3(5, 5, 5));
	test.set_constant_acceleration(XMFLOAT3(0.0f, -9.82f, 0.0f));
	test.set_lifetime(0.001f);
	test.set_spawn_time(0.0f);
	test.set_rate(1000.0f);
	test.set_speed(1.0f);
	test.set_position(XMFLOAT3(0.0f, 0.0f, 0.0f));
	test.set_initial_velocity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	test.set_collision_behaviour(true, 0.2f, 0.5f);
	test.set_rotation(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	test.set_stretching_behaviour(true, 1.0f);
	test.set_painting_behaviour(true, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	test.set_size(XMFLOAT2(0.01f, 0.01f), XMFLOAT2(0.01f, 0.01f), XMFLOAT2(0.01f, 0.01f), XMFLOAT2(0.01f, 0.01f));
	test.set_color(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.1f, 0.0f, 1.0f, 1.0f));

	// NOTE: Set up particle system
	{
		system.set_position(position);
		system.set_time_buffer(time);
		system.set_camera(camera);
		system.set_shared_resources(resources);
		system.set_texture_array(textures);
		system.set_deferred_renderer(deferred);

		system.add_emitter(&test, "Textures/white_square.png");

		Math::BoundingBox particleSystemBoundingBox;
		particleSystemBoundingBox.set(XMVectorSet(-3.0f, -3.0f, -3.0f, 1), XMVectorSet(3.0f, 3.0f, 3.0f, 1));
		system.set_bounding_box(particleSystemBoundingBox);

		if (!system.initialize())
		{
			return nullptr;
		}
	}

	return &system;
}

void ParticleExamples::TestSystem::set_velocity(const XMFLOAT3 & velocity)
{
	test.set_initial_velocity(velocity);
}