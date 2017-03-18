#pragma once

#include <Precompiled.h>

namespace Particles
{
	using namespace DirectX;

	// NOTE: Need to be 16-byte aligned
	struct Particle
	{
		XMFLOAT3 position;
		XMFLOAT3 velocity;
		XMFLOAT3 acceleration;
		float lifetime;
		float age;
		float texture;
		float collisions;
		float culled;
		float painting;
		float sleeping;
		float index;
	};
}
