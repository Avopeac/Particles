#include <Precompiled.h>

#include "ParticleEmitterShaders.h"

using namespace Direct3D;
using namespace Particles;

ParticleEmitterShaders::ParticleEmitterShaders(const D3DDevice &device) : m_device(device) 
{

}

ParticleEmitterShaders::~ParticleEmitterShaders() 
{

}

void ParticleEmitterShaders::add_emitter_shader(ParticleEmitterShape shape, const std::string &shader_path) 
{
	if (m_shaders[shape] == nullptr) 
	{
		std::unique_ptr<Direct3DResource::D3DShader> shader = std::unique_ptr<Direct3DResource::D3DShader>(new Direct3DResource::D3DShader);
		if (!shader->compile(m_device, shader_path, Direct3DResource::SHADER_TYPE_COMPUTE))
		{
			MessageBoxA(0, "Failed to load particle emitter shaders from path.", 0, 0);
			PostQuitMessage(EXIT_FAILURE);
		}

		m_shaders[shape] = std::move(shader);
	}
}

D3DShader * const Particles::ParticleEmitterShaders::get_emitter_shader(ParticleEmitterShape shape) const
{
	return m_shaders[shape].get();
}
