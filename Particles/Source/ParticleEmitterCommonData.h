#pragma once

#include <Precompiled.h>

namespace Particles
{
	using namespace DirectX;

	// NOTE: Need to be 16-byte aligned
	struct ParticleEmitterCommonData
	{
		XMFLOAT3 emitterPosition;
		float lifetime;
		XMFLOAT3 velocity;
		float spawn;
		XMFLOAT3 acceleration;
		float emission;
		float texture;
		float index;
		float speed;
		float volume;
	};
}
