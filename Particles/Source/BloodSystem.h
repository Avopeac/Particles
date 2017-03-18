#pragma once

#include "D3DTimeBuffer.h"

#include "PerspectiveCamera.h"
#include "DeferredGeometryPass.h"

#include "ParticleSystemBase.h"
#include "ParticleSystemTextureArray.h"
#include "ParticleEmitterShaders.h"
#include "ParticleSystemSharedResources.h"
#include "ParticleEmitterBase.h"
#include "ParticleEmitterCone.h"
#include "ParticleEmitterCube.h"
#include "ParticleEmitterSphere.h"

namespace ParticleExamples
{
	using namespace Particles;
	using namespace Direct3DResource;

	class BloodSystem
	{
	public:
		BloodSystem(const D3DDevice &device, const ParticleEmitterShaders &shaders);
		~BloodSystem();
		BloodSystem(const BloodSystem &) = delete;
		BloodSystem(BloodSystem &&) = delete;
		BloodSystem &operator=(const BloodSystem &) = delete;
		BloodSystem &operator=(BloodSystem &&) = delete;

		ParticleSystemBase * create(const XMFLOAT3 &position,
			DeferredGeometryPass * const deferred,
			D3DTimeBuffer * const time,
			PerspectiveCamera * const camera,
			ParticleSystemSharedResources * const resources,
			ParticleSystemTextureArray * const textures);

		void set_velocity(const XMFLOAT3 &velocity);

	private:

		ParticleSystemBase system;
		ParticleEmitterSphere blood;

	};
}
