#pragma once

#include "D3DShader.h"
#include "D3DDevice.h"
#include "D3DSamplerState.h"
#include "D3DFullscreenQuad.h"

#include "DeferredGeometryPass.h"
#include "PerspectiveCamera.h"

namespace Rendering
{
	using namespace DirectX;
	using namespace Direct3D;

	const uint32_t MAX_POINT_LIGHTS = 256;
	const uint32_t MAX_DIRECTIONAL_LIGHTS = 32;

	struct DirectionalLight
	{
		XMFLOAT3 direction;
		float padding0;
		XMFLOAT3 intensity;
		float padding1;
	};

	struct DirectionalLights
	{
		DirectionalLight lights[MAX_DIRECTIONAL_LIGHTS];
		XMFLOAT4 lightCount;
	};

	// The deferred light pass applies directional lighting to the scene information (passed by the G-buffer).
	// This does not support point lights at this moment, but could be added here.
	// Note that this is only deferred shading, this is no light pre-pass as in deferred lighting method.
	// Author: Andreas Larsson
	// Date: 2016-12-12 (YYYY-MM-DD)
	class DeferredLightPass 
	{
		bool m_directionalLightsDirty; 
		DirectionalLights m_directionalLightData;
		D3DSamplerState m_samplerState;
		PostProcessing::D3DFullscreenQuad m_fullscreenQuad;
		D3DShader m_directionalLightsShader;
		D3DConstantBuffer<DirectionalLights> m_directionalLightsBuffer;

	public:
		DeferredLightPass();
		~DeferredLightPass();
		DeferredLightPass(const DeferredLightPass &) = delete;
		DeferredLightPass(DeferredLightPass &&) = delete;
		DeferredLightPass &operator=(const DeferredLightPass &) = delete;
		DeferredLightPass &operator=(DeferredLightPass &&) = delete;

		// NOTE: Initialize the deferred lighting
		bool initialize(const D3DDevice &device, const std::string &directionalLightShader);

		// NOTE: Add a directional light source to the scene
		void add_directional_light(const XMFLOAT3 &direction, const XMFLOAT3 &intensity);

		// NOTE: Render the lighting
		void render(const D3DDevice &device, const PerspectiveCamera&camera, const DeferredGeometryPass &deferred, const D3DRenderTarget &target);

	private:
		
	};
}
