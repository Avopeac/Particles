#pragma once
#include "ParticleEmitterBase.h"
#include "ParticleEmitterShape.h"

namespace Particles
{
	struct ConeParticleEmitterData 
	{
		XMMATRIX rotation;
		float angle;
		float height;
		float padding[2];
	};

	class ParticleEmitterCone: public ParticleEmitterBase 
	{
		ConeParticleEmitterData m_shaderData;
		Direct3DResource::D3DConstantBuffer<ConeParticleEmitterData> m_shaderDataBuffer;

		XMFLOAT3 m_eulerAngles;
		float m_angle;
		float m_height;

	public:
		ParticleEmitterCone(const Direct3D::D3DDevice &device, const ParticleEmitterShaders &shaders);
		~ParticleEmitterCone();
		ParticleEmitterCone(const ParticleEmitterCone &) = delete;
		ParticleEmitterCone(ParticleEmitterCone &&) = delete;
		ParticleEmitterCone &operator=(const ParticleEmitterCone &) = delete;
		ParticleEmitterCone &operator=(ParticleEmitterCone &&) = delete;

		// NOTE: Set the rotation of the cone
		void set_cone_rotation(const XMFLOAT3 &eulerAngles);

		// NOTE: Set the angle of cone
		void set_cone_angle(float angle);

		// NOTE: Set the height of the cone
		void set_cone_height(float height);

		// NOTE: Map the emitter shape specific data
		ID3D11Buffer * const map_emitter_specific_data() override;

	protected:
				
	private:
		
	};
}
