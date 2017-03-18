#pragma once

#include <Precompiled.h>

namespace Particles
{
	using namespace DirectX;

	// NOTE: Need to be 16-byte aligned
	struct GlobalParticleEmitterDataSingle
	{
		XMFLOAT4 colorPoint0;
		XMFLOAT4 colorPoint1;
		XMFLOAT4 colorPoint2;
		XMFLOAT4 colorPoint3;
		XMFLOAT2 sizePoint0;
		XMFLOAT2 sizePoint1;
		XMFLOAT2 sizePoint2;
		XMFLOAT2 sizePoint3;
		XMFLOAT4 rotationPoints;
		float collisionsEnabled;
		float collisionDampening;
		float restitutionFactor;
		float paintingEnabled;
		XMFLOAT4 paintColor;
		float sleepingEnabled;
		float sleepingCollisionFrames;
		float stretchingEnabled;
		float stretchFactor;
	};

	const uint32_t MAX_EMITTERS_PER_PARTICLE_SYSTEM = 8;

	// NOTE: Need to be 16-byte aligned
	struct GlobalParticleEmitterData
	{
		GlobalParticleEmitterDataSingle emitters[MAX_EMITTERS_PER_PARTICLE_SYSTEM];
	};
}

