#include <Precompiled.h>

#include "Particle.h"
#include "ParticleSystemBase.h"
#include "ParticleEmitterCube.h"
#include "ParticleEmitterSphere.h"
#include "ParticleEmitterCone.h"

using namespace Direct3D;
using namespace Particles;

ParticleSystemBase::ParticleSystemBase(const D3DDevice & device, uint32_t particleCount, ParticleSystemBlendMode blendMode) :
	m_device(device),
	m_timeBuffer(0),
	m_particleRenderTarget(0),
	m_camera(0),
	m_deferredRenderer(0),
	m_emitterCount(0),
	m_particleCount(particleCount),
	m_blendMode(blendMode)
{ 
	ZeroMemory(&m_position, sizeof(m_position));
	ZeroMemory(&m_globalParticleEmitterData, sizeof(m_globalParticleEmitterData));
	ZeroMemory(&m_particleSystemData, sizeof(m_particleSystemData));
}

ParticleSystemBase::~ParticleSystemBase() 
{

}

bool ParticleSystemBase::initialize() 
{
	if (!m_particleInitShader.compile(m_device, "Shaders/particles/particle_init.hlsl", SHADER_TYPE_COMPUTE) ||
		!m_particleUpdateShader.compile(m_device, "Shaders/particles/particle_update.hlsl", SHADER_TYPE_COMPUTE) ||
		!m_particleRenderShader.compile(m_device, "Shaders/particles/particle_render.hlsl", SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL))
	{
		return false;
	}

	// NOTE: Our fullscreen passes have the same vertex layout, as such one of the VS/PS shaders can be used.
	if (!m_fullscreenQuad.initialize(m_device, m_particleRenderShader))
	{
		return false;
	}

	if (!m_particleDeadBuffer.create(m_device, m_particleCount, sizeof(uint32_t), false, true) ||
		!m_particleRenderBuffer.create(m_device, m_particleCount, sizeof(uint32_t), true, false) ||
		!m_particleBuffer.create(m_device, m_particleCount, sizeof(Particle), false, false) ||
		!m_particleEmitterBuffer.create(m_device) ||
		!m_particleSystemBuffer.create(m_device) ||
		!m_drawArgsBuffer.create(m_device))
	{
		return false;
	}

	const uint32_t NUM_UAVS = 1;
	const uint32_t UAV_INITIAL_COUNTS[NUM_UAVS] = { 0 };
	ID3D11DeviceContext * context = m_device.get_context();
	ID3D11UnorderedAccessView * unorderedAccessView = m_particleDeadBuffer.get_uav();
	// NOTE: Initialize shader shall have largest amount of threads per group
	uint32_t threadGroupCount = m_particleCount / 1024;
	threadGroupCount = threadGroupCount == 0 ? 1 : threadGroupCount;
	m_particleEmitterBuffer.map_data(context, &m_globalParticleEmitterData);
	m_particleInitShader.bind_shader(m_device, ShaderType::SHADER_TYPE_COMPUTE);
	context->CSSetUnorderedAccessViews(0, NUM_UAVS, &unorderedAccessView, UAV_INITIAL_COUNTS);
	context->Dispatch(threadGroupCount, 1, 1);
	return true;
}

void ParticleSystemBase::emit(float deltaTime) 
{
	// NOTE: Update particle system position
	if (!m_splineChain.empty())
	{
		XMStoreFloat3(&m_position, m_splineChain.update(deltaTime));
	}

	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	m_updatedBoundingBox = m_originalBoundingBox;
	m_updatedBoundingBox.transform(translation);

	const uint32_t NUM_CBUF = 4;
	const uint32_t NUM_UAVS = 2;
	const uint32_t UAV_INITIAL_COUNTS[NUM_UAVS] = { (uint32_t)-1, 0 };
	ID3D11DeviceContext * context = m_device.get_context();
	ID3D11UnorderedAccessView * unorderedAccessViews[NUM_UAVS] = 
	{ 
		m_particleDeadBuffer.get_uav(),
		m_particleBuffer.get_uav(),
	};

	for (const auto &emitter: m_emitterPointers) 
	{
		// NOTE: Do not process on pause
		if (emitter->is_paused()) 
		{
			continue; 
		}

		ID3D11Buffer * constantBuffers[NUM_CBUF] =
		{
			m_timeBuffer->get_buffer().get_data(),
			m_particleSystemBuffer.get_data(),
			emitter->map_emitter_common_data(deltaTime),
			emitter->map_emitter_specific_data(),
		};

		const ParticleEmitterCommonData &emitterGeneralData = emitter->get_emitter_data();
		emitter->get_shader()->bind_shader(m_device, ShaderType::SHADER_TYPE_COMPUTE);
		emitter->set_parent_position(m_position);

		// NOTE: Emitters should have maximum number of threads per group
		uint32_t threadGroupCount = static_cast<uint32_t>(emitterGeneralData.emission) / 1024;
		threadGroupCount = threadGroupCount == 0 ? 1 : threadGroupCount;
		context->CSSetConstantBuffers(0, NUM_CBUF, constantBuffers);
		context->CSSetUnorderedAccessViews(0, NUM_UAVS, unorderedAccessViews, UAV_INITIAL_COUNTS);
		context->Dispatch(threadGroupCount, 1, 1);
	}
}

void ParticleSystemBase::update() 
{
	const uint32_t NUM_UAVS = 4;
	const uint32_t NUM_CBUF = 4;
	const uint32_t NUM_SRVS = 2;
	uint32_t uavInitialCounts[NUM_UAVS] = { static_cast<uint32_t>(-1), 0,  0,  0, };
	
	ID3D11DeviceContext * context = m_device.get_context();
	ID3D11SamplerState * samplerState = m_particleSharedResources->get_sampler_state()->get_state();
	
	ID3D11UnorderedAccessView * unorderedAccessViews[NUM_UAVS] =
	{
		m_particleDeadBuffer.get_uav(),
		m_particleBuffer.get_uav(),
		m_particleRenderBuffer.get_uav(),
		m_drawArgsBuffer.get_uav(),
	};

	ID3D11Buffer * constantBuffers[NUM_CBUF] =
	{
		m_timeBuffer->get_buffer().get_data(),
		m_camera->get_matrix_buffer(),
		m_particleEmitterBuffer.get_data(),
		m_particleSystemBuffer.get_data(),
	};

	ID3D11ShaderResourceView * shaderResourceViews[NUM_SRVS] =
	{
		m_deferredRenderer->get_depth_stencil().get_srv(),
		m_deferredRenderer->get_target(Rendering::GBuffer::GBUFFER_NORMAL_SPECULAR).get_srv(),
	};

	m_particleUpdateShader.bind_shader(m_device, ShaderType::SHADER_TYPE_COMPUTE);
	context->VSSetShader(0, 0, 0);
	context->PSSetShader(0, 0, 0);
	context->CSSetUnorderedAccessViews(0, NUM_UAVS, unorderedAccessViews, uavInitialCounts);
	context->CSSetShaderResources(0, NUM_SRVS, shaderResourceViews);
	context->CSSetConstantBuffers(0, NUM_CBUF, constantBuffers);
	// NOTE: Update shader has a lower amount of threads per thread group since there's quite a lot of branching, this reduces the execution time from using maximum threads per group
	uint32_t threadGroupCount = static_cast<uint32_t>(m_particleCount) / 128;
	threadGroupCount = threadGroupCount == 0 ? 1 : threadGroupCount;
	context->CSSetSamplers(0, 1, &samplerState);
	context->Dispatch(threadGroupCount, 1, 1);

	// NOTE: Reset register
	for (uint32_t uav = 0; uav < NUM_UAVS; ++uav) 
	{
		unorderedAccessViews[uav] = 0;
	}

	for (uint32_t srv = 0; srv < NUM_SRVS; ++srv) 
	{ 
		shaderResourceViews[srv] = 0;
	}

	context->CSSetUnorderedAccessViews(0, NUM_UAVS, unorderedAccessViews, 0);
	context->CSSetShaderResources(0, NUM_SRVS, shaderResourceViews);
	context->CSSetShader(0, 0, 0);

	// NOTE: Collect the counter values for the read write buffers
	m_particleSystemData.renderCount = m_particleRenderBuffer.get_counter(m_device);
	m_particleSystemData.deadCount = m_particleDeadBuffer.get_counter(m_device);
	m_particleSystemData.maxCount = m_particleCount;
	m_particleSystemBuffer.map_data(context, &m_particleSystemData);
}

void ParticleSystemBase::render()
{
	ID3D11SamplerState * samplerState = m_particleSharedResources->get_sampler_state()->get_state();
	ID3D11DeviceContext * context = m_device.get_context();

	// NOTE: Particle rendering
	{
		const uint32_t NUM_CBUFS = 3;
		const uint32_t NUM_SRVS = 2;
		const uint32_t NUM_RTVS = 2;

		m_particleRenderShader.bind_shader(m_device, ShaderType::SHADER_TYPE_VERTEX);
		m_particleRenderShader.bind_shader(m_device, ShaderType::SHADER_TYPE_PIXEL);
		
		ID3D11RenderTargetView * r0 = m_particleSharedResources->get_collision_render_target()->get_rtv();
		ID3D11RenderTargetView * r1 = m_particleSharedResources->get_color_render_target();
		ID3D11RenderTargetView * renderTargetViews[NUM_RTVS] = { r0, r1 };
		//context->OMSetRenderTargets(NUM_RTVS, renderTargetViews, m_deferredRenderer->get_depth_stencil().get_dsv());
		context->OMSetRenderTargets(NUM_RTVS, renderTargetViews, 0);
		
		// NOTE: Collect constant buffers
		ID3D11Buffer * b0 = m_camera->get_matrix_buffer();
		ID3D11Buffer * b1 = m_particleEmitterBuffer.get_data();
		ID3D11Buffer * b2 = m_particleSharedResources->get_target_dimension_buffer()->get_data();
		ID3D11Buffer * constantBuffers[NUM_CBUFS] = { b0, b1, b2 };
		context->PSSetConstantBuffers(0, NUM_CBUFS, constantBuffers);
		context->VSSetConstantBuffers(0, NUM_CBUFS, constantBuffers);

		// NOTE: Collect shader resource views
		ID3D11ShaderResourceView * vertexShaderSRV0 = m_particleBuffer.get_srv();
		ID3D11ShaderResourceView * vertexShaderSRV1 = m_particleRenderBuffer.get_srv();
		ID3D11ShaderResourceView * pixelShaderSRV0 = m_particleTextureArray->get_srv();
		ID3D11ShaderResourceView * pixelShaderSRV1 = m_deferredRenderer->get_depth_stencil().get_srv();
		ID3D11ShaderResourceView * vertexShaderResourceViews[NUM_SRVS] = { vertexShaderSRV0, vertexShaderSRV1 };
		ID3D11ShaderResourceView * pixelShaderResourceViews[NUM_SRVS] = { pixelShaderSRV0, pixelShaderSRV1 };
		context->VSSetShaderResources(0, NUM_SRVS, vertexShaderResourceViews);
		context->PSSetShaderResources(0, NUM_SRVS, pixelShaderResourceViews);
		

		if (m_blendMode != BLEND_NONE) {
			m_device.set_depth_test(DepthTest::DEPTH_TEST_OFF);
		}
		else {
			// If you would like to add DSV to the OM render targets
			m_device.set_depth_test(DepthTest::DEPTH_TEST_ON);
		}

		context->OMSetBlendState(m_particleSharedResources->get_render_blend_state(m_blendMode)->get_blend_state(), 0, ~0);
		context->IASetVertexBuffers(0, 0, 0, 0, 0);
		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
		context->IASetInputLayout(0);
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->PSSetSamplers(0, 1, &samplerState);
		context->RSSetViewports(1, &m_particleSharedResources->get_viewport());
		context->DrawInstancedIndirect(m_drawArgsBuffer.get_buffer(), 0);
		
		// NOTE: Reset registers
		for (uint32_t index = 0; index < NUM_SRVS; ++index)
		{ 
			vertexShaderResourceViews[index] = 0;
		}

		for (uint32_t index = 0; index < NUM_SRVS; ++index)
		{
			pixelShaderResourceViews[index] = 0;
		}

		for (uint32_t index = 0; index < NUM_RTVS; ++index)
		{ 
			renderTargetViews[index] = 0;
		}

		context->VSSetShaderResources(0, NUM_SRVS, vertexShaderResourceViews);
		context->PSSetShaderResources(0, NUM_SRVS, pixelShaderResourceViews);
		context->OMSetRenderTargets(NUM_RTVS, renderTargetViews, 0);
		context->OMSetBlendState(0, 0, ~0);
	}
}

void Particles::ParticleSystemBase::set_spline_chain(const Math::SplineChain & splineChain)
{
	m_splineChain = splineChain;
}

void ParticleSystemBase::add_emitter(ParticleEmitterBase * emitter, const std::string &texturePath)
{	
	if (m_emitterCount < MAX_EMITTERS_PER_PARTICLE_SYSTEM - 1)
	{
		uint32_t emitterIndex = m_emitterCount++;
		emitter->set_emitter_index(static_cast<float>(emitterIndex));
		const GlobalParticleEmitterDataSingle &globalEmitterData = emitter->get_global_emitter_data();
		
		// NOTE: Copy emitter data to the system
		m_globalParticleEmitterData.emitters[emitterIndex] = globalEmitterData;

		// NOTE: Assign texture index
		int32_t index = m_particleTextureArray->find(texturePath);
		if (index >= 0)
		{
			emitter->set_texture_index(static_cast<float>(index));
		}
		
		m_emitterPointers.push_back(emitter);
	}
}

void Particles::ParticleSystemBase::set_texture_array(const ParticleSystemTextureArray * const particleTextureArray)
{
	m_particleTextureArray = particleTextureArray;
}

void Particles::ParticleSystemBase::set_shared_resources(const ParticleSystemSharedResources * const particleSharedResources)
{
	m_particleSharedResources = particleSharedResources;
}

void Particles::ParticleSystemBase::set_time_buffer(Direct3D::D3DTimeBuffer * const time)
{
	m_timeBuffer = time;
}

void Particles::ParticleSystemBase::set_camera(Rendering::PerspectiveCamera * const camera)
{
	m_camera = camera;
}

void Particles::ParticleSystemBase::set_deferred_renderer(Rendering::DeferredGeometryPass * const deferredRenderer)
{
	m_deferredRenderer = deferredRenderer;
}

void Particles::ParticleSystemBase::set_render_target(D3DRenderTarget * const particleRenderTarget)
{
	m_particleRenderTarget = particleRenderTarget;
}

void Particles::ParticleSystemBase::set_position(const XMFLOAT3 &position)
{
	m_position = position;
}

const XMFLOAT3 &Particles::ParticleSystemBase::get_position()
{
	return m_position;
}

void Particles::ParticleSystemBase::set_bounding_box(const Math::BoundingBox & boundingBox)
{
	m_originalBoundingBox = boundingBox;
	m_updatedBoundingBox = m_originalBoundingBox;
}

const Math::BoundingBox & Particles::ParticleSystemBase::get_bounding_box() const
{
	return m_updatedBoundingBox;
}

const D3DStructuredBuffer &Particles::ParticleSystemBase::get_particles_buffer()
{
	return m_particleBuffer;
}

const D3DRenderTarget &Particles::ParticleSystemBase::get_collision_target() const
{
	return *m_particleSharedResources->get_collision_render_target();
}

ID3D11Buffer * const Particles::ParticleSystemBase::get_system_data() const
{
	return m_particleSystemBuffer.get_data();
}
