#pragma once
#include "ParticleEmitterBase.h"
#include "ParticleEmitterShape.h"

namespace Particles 
{
	struct SphereParticleEmitterData 
	{
		float radius;
		float padding[3];
	};

	class ParticleEmitterSphere : public ParticleEmitterBase 
	{
		float m_radius;
		SphereParticleEmitterData m_shaderData;
		Direct3DResource::D3DConstantBuffer<SphereParticleEmitterData> m_shaderDataBuffer;

	public:
		ParticleEmitterSphere(const Direct3D::D3DDevice &device, const ParticleEmitterShaders &shaders);
		~ParticleEmitterSphere();
		ParticleEmitterSphere(const ParticleEmitterSphere &) = delete;
		ParticleEmitterSphere(ParticleEmitterSphere &&) = delete;
		ParticleEmitterSphere& operator=(const ParticleEmitterSphere &) = delete;
		ParticleEmitterSphere& operator=(ParticleEmitterSphere &&) = delete;
		
		// NOTE: Map shape specific data
		ID3D11Buffer * const map_emitter_specific_data() override;

		// NOTE: Set sphere radius
		void set_radius(float radius);

	private:
		
	};
}
