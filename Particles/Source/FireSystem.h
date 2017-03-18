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

	class FireSystem
	{
	public:
		FireSystem(const D3DDevice &device, const ParticleEmitterShaders &shaders);
		~FireSystem();
		FireSystem(const FireSystem &) = delete;
		FireSystem(FireSystem &&) = delete;
		FireSystem &operator=(const FireSystem &) = delete;
		FireSystem &operator=(FireSystem &&) = delete;

		ParticleSystemBase * create(const XMFLOAT3 &position,
			DeferredGeometryPass * const deferred,
			D3DTimeBuffer * const time,
			PerspectiveCamera * const camera,
			ParticleSystemSharedResources * const resources,
			ParticleSystemTextureArray * const textures);

	private:

		ParticleSystemBase system;
		ParticleEmitterCone smoke;
		ParticleEmitterCone fire;
		ParticleEmitterCone sparks;
		
	};
}
