#pragma once

#include "D3DDevice.h"
#include "D3DTimeBuffer.h"
#include "D3DShader.h"
#include "D3DRenderTarget.h"
#include "D3DSamplerState.h"
#include "D3DFullscreenQuad.h"
#include "D3DDrawArgsBuffer.h"

#include "ParticleEmitterBase.h"
#include "ParticleEmitterShape.h"
#include "ParticleEmitterCommonData.h"
#include "ParticleSystemCounters.h"
#include "ParticleSystemTextureArray.h"
#include "ParticleSystemSharedResources.h"

#include "PerspectiveCamera.h"
#include "DeferredGeometryPass.h"

#include "Spline.h"
#include "SplineChain.h"

namespace Particles 
{
	using namespace Direct3D;
	using namespace Rendering;
	using namespace PostProcessing;
	using namespace Direct3DResource;

	class ParticleSystemBase
	{
		const D3DDevice &m_device;
		D3DTimeBuffer * m_timeBuffer;
		D3DRenderTarget * m_particleRenderTarget;
		PerspectiveCamera * m_camera;
		DeferredGeometryPass * m_deferredRenderer;

		D3DBlendState m_blendState;
		D3DShader m_particleInitShader;
		D3DShader m_particleUpdateShader;
		D3DShader m_particleRenderShader;
		D3DFullscreenQuad m_fullscreenQuad;
		D3DStructuredBuffer m_particleDeadBuffer;
		D3DStructuredBuffer m_particleRenderBuffer;
		D3DStructuredBuffer m_particleBuffer;
		D3DDrawArgsBuffer m_drawArgsBuffer;
		D3DConstantBuffer<ParticleSystemCounters> m_particleSystemBuffer;
		D3DConstantBuffer<GlobalParticleEmitterData> m_particleEmitterBuffer;

		const ParticleSystemTextureArray * m_particleTextureArray = 0;
		const ParticleSystemSharedResources * m_particleSharedResources = 0;

		XMFLOAT3 m_position;
		uint32_t m_particleCount = 0;
		uint32_t m_emitterCount = 0;
		std::vector<ParticleEmitterBase *> m_emitterPointers;
		Math::SplineChain m_splineChain;
		ParticleSystemCounters m_particleSystemData;
		GlobalParticleEmitterData m_globalParticleEmitterData;
		Math::BoundingBox m_originalBoundingBox;
		Math::BoundingBox m_updatedBoundingBox;
		ParticleSystemBlendMode m_blendMode;

	public:

		friend class ParticleEmitterBase;

		ParticleSystemBase(const D3DDevice &device, uint32_t particleCount, ParticleSystemBlendMode blendMode);
		~ParticleSystemBase();
		ParticleSystemBase(const ParticleSystemBase &) = delete;
		ParticleSystemBase(ParticleSystemBase &&) = delete;
		ParticleSystemBase &operator=(const ParticleSystemBase &) = delete;
		ParticleSystemBase &operator=(ParticleSystemBase &&) = delete;

		void set_texture_array(const ParticleSystemTextureArray * const);
		void set_shared_resources(const ParticleSystemSharedResources * const);
		void set_time_buffer(Direct3D::D3DTimeBuffer * const time);
		void set_camera(Rendering::PerspectiveCamera * const camera);
		void set_deferred_renderer(Rendering::DeferredGeometryPass * const deferred);
		void set_render_target(D3DRenderTarget * const target);
		void set_position(const XMFLOAT3 &position);
		const XMFLOAT3 &get_position();
		void set_bounding_box(const Math::BoundingBox &boundingBox);
		const Math::BoundingBox &get_bounding_box() const;
		const D3DStructuredBuffer &get_particles_buffer();
		const D3DRenderTarget &get_collision_target() const;
		ID3D11Buffer * const get_system_data() const;
		void set_spline_chain(const Math::SplineChain &splineChain);
		void add_emitter(ParticleEmitterBase * emitter, const std::string & texturePath);
		bool initialize();
		void emit(float delta_time);
		void update();
		void render();
		
	private:

	};
}
