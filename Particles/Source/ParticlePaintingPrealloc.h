#pragma once

#include "D3DDevice.h"
#include "D3DMesh.h"
#include "D3DShader.h"
#include "D3DSamplerState.h"
#include "D3DConstantBuffer.h"

#include "PerspectiveCamera.h"
#include "DeferredGeometryPass.h"
#include "ParticleSystemBase.h"


namespace Particles
{
	using namespace Direct3D;
	using namespace Direct3DResource;
	using namespace Rendering;

	struct PaintCollection
	{
		bool allocated;
		bool used;
		uint32_t swapCounter;
		D3DRenderTarget target0;
		D3DRenderTarget target1;
		ID3D11ShaderResourceView * currentShaderResourceView;
		ID3D11RenderTargetView * currentRenderTargetView;
	};

	class ParticlePaintingPrealloc
	{
		const D3DDevice &m_device;
		
		D3D11_VIEWPORT m_paintShaderViewport;
		D3DShader m_paintShader;
		
		D3DSamplerState m_bilinearSampler;
		D3DSamplerState m_nearestSampler;
		
		D3DConstantBuffer<XMVECTOR> m_texcoordTransformBuffer;
		D3DConstantBuffer<XMMATRIX> m_worldTransformBuffer;
		
		ComPtr<ID3D11InputLayout> m_pInputLayout;
		
		uint32_t m_paintAllocations;
		uint32_t m_paintWidth;
		uint32_t m_paintHeight;
		uint32_t m_paintsPerFrame;
		std::vector<PaintCollection> m_paint;
		std::unordered_map<uint32_t, uint32_t> m_paintIndices;

	public:

		ParticlePaintingPrealloc(const D3DDevice &device, uint32_t allocations, 
			uint32_t width, uint32_t height, uint32_t paintsPerFrame);

		~ParticlePaintingPrealloc();

		ParticlePaintingPrealloc(const ParticlePaintingPrealloc &) = delete;

		ParticlePaintingPrealloc(ParticlePaintingPrealloc &&) = delete;

		ParticlePaintingPrealloc &operator=(const ParticlePaintingPrealloc &) = delete;

		ParticlePaintingPrealloc &operator=(ParticlePaintingPrealloc &&) = delete;

		bool initialize(const std::string &paintShaderName);

		void render(const PerspectiveCamera &camera, const std::vector<D3DMesh *>& meshes,
			const DeferredGeometryPass & deferred, const D3DRenderTarget * collisionTarget, 
			const std::vector<Math::BoundingBox> boxes);

		void set_paints_per_frame(uint32_t count);

	private:

		bool create_input_layout();

		bool create_target(PaintCollection &collection, uint32_t width, uint32_t height);

		int32_t get_free_texture();
	};
}
