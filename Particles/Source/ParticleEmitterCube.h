#pragma once
#include "ParticleEmitterBase.h"
#include "ParticleEmitterShape.h"

namespace Particles 
{
	struct CubeParticleEmitterData 
	{
		XMMATRIX rotation;
		XMFLOAT3 lengths;
	};

	class ParticleEmitterCube : public ParticleEmitterBase 
	{
		XMFLOAT3 m_eulerAngles;
		XMFLOAT3 m_sideLengths;
		CubeParticleEmitterData m_shaderData;
		Direct3DResource::D3DConstantBuffer<CubeParticleEmitterData> m_shaderDataBuffer;

	public:
		ParticleEmitterCube(const Direct3D::D3DDevice &device, const ParticleEmitterShaders &shaders);
		~ParticleEmitterCube();
		ParticleEmitterCube(const ParticleEmitterCube &) = delete;
		ParticleEmitterCube(ParticleEmitterCube &&) = delete;
		ParticleEmitterCube &operator=(const ParticleEmitterCube &) = delete;
		ParticleEmitterCube &operator=(ParticleEmitterCube &&) = delete;

		// NOTE: Map shape specific data
		ID3D11Buffer * const map_emitter_specific_data() override;

		// NOTE: Set cube rotation
		void set_cube_rotation(const XMFLOAT3 &eulerAngles);

		// NOTE: Set side lengths for the cube
		void set_cube_lengths(const XMFLOAT3 &lengths);

	private:
		
	};
}