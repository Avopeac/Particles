#pragma once
#include "D3DDevice.h"
#include "D3DShader.h"
#include "ParticleEmitterShape.h"

namespace Particles 
{
	using namespace Direct3D;
	using namespace Direct3DResource;

	class ParticleEmitterShaders 
	{
		const D3DDevice &m_device;
		ParticleEmitterShape m_shapes[ParticleEmitterShape::SHAPE_COUNT];
		std::unique_ptr<D3DShader> m_shaders[ParticleEmitterShape::SHAPE_COUNT];

	public:
		ParticleEmitterShaders(const Direct3D::D3DDevice &device);
		~ParticleEmitterShaders();
		ParticleEmitterShaders(const ParticleEmitterShaders &) = delete;
		ParticleEmitterShaders(ParticleEmitterShaders &&) = delete;
		ParticleEmitterShaders &operator=(const ParticleEmitterShaders &) = delete;
		ParticleEmitterShaders &operator=(ParticleEmitterShaders &&) = delete;
		
		// NOTE: Add the shader for a specific emitter
		void add_emitter_shader(ParticleEmitterShape shape, const std::string &shaderPath);
		
		// NOTE: Get the shader for the specified emitter shape
		D3DShader * const get_emitter_shader(ParticleEmitterShape shape) const;

	private:
	
	};
}
