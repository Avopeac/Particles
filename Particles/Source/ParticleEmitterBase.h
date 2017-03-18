#pragma once
#include "D3DDevice.h"
#include "D3DShader.h"
#include "D3DConstantBuffer.h"
#include "ParticleEmitterShaders.h"
#include "ParticleEmitterShape.h"
#include "ParticleEmitterCommonData.h"
#include "ParticleEmitterGlobalData.h"
#include "SplineChain.h"

namespace Particles 
{
	
	using namespace DirectX;
	using namespace Direct3D;
	using namespace Direct3DResource;

	class ParticleEmitterBase 
	{
	public:
		ParticleEmitterBase(const Direct3D::D3DDevice &device, const ParticleEmitterShaders &shaders, ParticleEmitterShape shape);
		virtual ~ParticleEmitterBase();
		ParticleEmitterBase(const ParticleEmitterBase &) = delete;
		ParticleEmitterBase(ParticleEmitterBase &&) = delete;
		ParticleEmitterBase &operator=(const ParticleEmitterBase &) = delete;
		ParticleEmitterBase &operator=(ParticleEmitterBase &&) = delete;
				
		D3DShader * get_shader() const;
		ParticleEmitterShape get_shape() const;
		void set_texture_index(float index);
		void set_emitter_index(float index);
		const GlobalParticleEmitterDataSingle &get_global_emitter_data() const;
		const ParticleEmitterCommonData &get_emitter_data() const;
		bool is_paused() const;
		void set_volume(bool volume);
		void set_paused(bool pause);
		void set_speed(float speed);
		void set_spawn_time(float time);
		void set_lifetime(float time);
		void set_rate(float rate);
		void set_position(const XMFLOAT3 &position);
		void set_parent_position(const XMFLOAT3 &position);
		void set_initial_velocity(const XMFLOAT3 &velocity);
		void set_constant_acceleration(const XMFLOAT3 &acceleration);
		void set_painting_behaviour(bool painting, const XMFLOAT4 &color);
		void set_sleeping_behaviour(bool sleeping, uint32_t collisions);
		void set_collision_behaviour(bool collisions, float dampening, float restitution);
		void set_stretching_behaviour(bool stretching, float factor);
		void set_spline_chain(const Math::SplineChain &splineChain);
		void set_rotation(const XMFLOAT4 &rotations);
		void set_color(const XMFLOAT4 &color0, const XMFLOAT4 &color1, const XMFLOAT4 &color2, const XMFLOAT4 &color3);
		void set_size(const XMFLOAT2 &size0, const XMFLOAT2 &size1, const XMFLOAT2 &size2, const XMFLOAT2 &size3);

		ID3D11Buffer * const map_emitter_common_data(float delta_time);
		virtual ID3D11Buffer * const map_emitter_specific_data() = 0;

	protected:
		bool m_paused = false;
		XMFLOAT3 m_position;
		XMFLOAT3 m_parentPosition;
		D3DShader * m_pShader;
		const D3DDevice &m_device;
		ParticleEmitterShape m_emitterShape;
		ComPtr<ID3D11Buffer> m_pEmitterShapeBuffer;
		ParticleEmitterCommonData m_emitterProperties;
		D3DConstantBuffer<ParticleEmitterCommonData> m_emitterBuffer;
		GlobalParticleEmitterDataSingle m_globalEmitterProperties;
		Math::SplineChain m_splineChain;

	private:

	};
}
