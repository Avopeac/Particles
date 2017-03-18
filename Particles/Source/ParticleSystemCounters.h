#pragma once

#include <Precompiled.h>

namespace Particles
{
	// NOTE: Need to be 16-byte aligned
	struct ParticleSystemCounters
	{
		uint32_t deadCount;
		uint32_t maxCount;
		uint32_t renderCount;
		uint32_t padding;
	};
}



