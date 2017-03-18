#include <Precompiled.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")
#pragma comment(lib, "DirectXTex.lib")

#ifndef WIN32_MEAN_AND_LEAN
#define WIN32_MEAN_AND_LEAN
#endif

// NOTE: Platform
#include "Win32Input.h"

// NOTE: Device
#include "D3DDevice.h"
#include "D3DTimeBuffer.h"

// NOTE: Direct3D resources
#include "D3DMesh.h"
#include "D3DShader.h"
#include "D3DStructuredBuffer.h"

// NOTE: Rendering
#include "DeferredGeometryPass.h"
#include "DeferredLightPass.h"
#include "DebugBoundingBox.h"

// NOTE: Post-processing
#include "D3DBloom.h"
#include "D3DFXAA.h"
#include "D3DFullscreenQuad.h"
#include "D3DRenderToTexture.h"

// NOTE: Assets and loading
#include "MeshAsset.h"
#include "MeshTexture.h"
#include "ModelImporter.h"
#include "MeshTexture.h"
#include "MeshTextureStore.h"

// NOTE: Particles
#include "ParticleEmitterShaders.h"
#include "ParticleEmitterSphere.h"
#include "ParticleEmitterCube.h"
#include "ParticleEmitterCone.h"
#include "ParticleSystemBase.h"
#include "Particle.h"
#include "ParticleEmitterShape.h"
#include "ParticleSystemTextureArray.h"
#include "ParticleSystemSharedResources.h"
//#include "ParticlePainting.h"
#include "ParticlePaintingPrealloc.h"

// NOTE: Particle examples
#include "FireSystem.h"
#include "BloodSystem.h"
#include "TestSystem.h"

using namespace Win32;
using namespace DirectX;

bool windowHasFocus = true;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (uMsg)
	{
	case WM_CLOSE:
	{
		DestroyWindow(hWnd);
	}
	break;

	case WM_DESTROY:
	{
		PostQuitMessage(EXIT_SUCCESS);
	}
	break;

	case WM_SETFOCUS:
	{
		windowHasFocus = true;
	}
	break;

	case WM_KILLFOCUS:
	{
		windowHasFocus = false;
	}
	break;

	default:
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	}
	return result;
}

static MSG window_proc_intercept(Win32Input * input)
{
	MSG message;
	while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
	{
		WPARAM currentMessage = message.wParam;
		WPARAM previousMessage = message.lParam;
		UINT messageContent = message.message;
		switch (messageContent)
		{
		case WM_KEYUP:
		case WM_KEYDOWN:
		case WM_SYSKEYUP:
		case WM_SYSKEYDOWN:
		{
			BYTE virtualKey = currentMessage % 0xFF;
			if (messageContent == WM_KEYDOWN)
			{
				input->set_keyboard_key_state(virtualKey, KeyState::KEY_DOWN);
			}
			else if (messageContent == WM_KEYUP)
			{
				input->set_keyboard_key_state(virtualKey, KeyState::KEY_UP);
			}

			// NOTE: WM_SYSKEYDOWN has bit 29 set if ALT was down previously
			if ((previousMessage & (1 << 29)) && currentMessage == VK_F4)
			{
				PostQuitMessage(EXIT_SUCCESS);
			}
		}
		break;

		case WM_LBUTTONUP:
		{
			input->set_mouse_button_state(MouseButton::LEFT_BUTTON, ButtonState::BUTTON_UP);
		}
		break;

		case WM_MBUTTONUP:
		{
			input->set_mouse_button_state(MouseButton::MIDDLE_BUTTON, ButtonState::BUTTON_UP);
		}
		break;

		case WM_RBUTTONUP:
		{
			input->set_mouse_button_state(MouseButton::RIGHT_BUTTON, ButtonState::BUTTON_UP);
		}
		break;

		case WM_LBUTTONDOWN:
		{
			input->set_mouse_button_state(MouseButton::LEFT_BUTTON, ButtonState::BUTTON_DOWN);
		}
		break;

		case WM_MBUTTONDOWN:
		{
			input->set_mouse_button_state(MouseButton::MIDDLE_BUTTON, ButtonState::BUTTON_DOWN);
		}
		break;

		case WM_RBUTTONDOWN:
		{
			input->set_mouse_button_state(MouseButton::RIGHT_BUTTON, ButtonState::BUTTON_DOWN);
		}
		break;

		default:
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
		}
	}

	return message;
}

static float get_high_res_time_in_seconds()
{
	LARGE_INTEGER outFrequency;
	LARGE_INTEGER outCounter;
	QueryPerformanceFrequency(&outFrequency);
	QueryPerformanceCounter(&outCounter);
	return static_cast<float>(outCounter.QuadPart) / outFrequency.QuadPart;
}

// This program is made for thesis work in the area of computer graphics. 
// Its main purpose is to simulate a particle system with the abilities to paint objects in the scene upon particle collisions.
// Author: Andreas Larsson
// Date: 2016-12-12 (YYYY-MM-DD)
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command, int show_command)
{

	// NOTE: Enable console for this program
#if _DEBUG
	AllocConsole();
	AttachConsole(ATTACH_PARENT_PROCESS);
	FILE * console_out;
	freopen_s(&console_out, "conout$", "w", stdout);
#endif

	const char * CLASS_NAME = "Particles Window Class";
	const char * WINDOW_NAME = "Particles Engine";

	WNDCLASSA windowClass;
	ZeroMemory(&windowClass, sizeof(windowClass));
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = instance;
	windowClass.lpszClassName = CLASS_NAME;

	if (RegisterClassA(&windowClass))
	{
		const UINT32 WINDOW_WIDTH = 1920;
		const UINT32 WINDOW_HEIGHT = 1080;
		const float WINDOW_WIDTH_F = static_cast<float>(WINDOW_WIDTH);
		const float WINDOW_HEIGHT_F = static_cast<float>(WINDOW_HEIGHT);

		RECT windowRectangle;
		ZeroMemory(&windowRectangle, sizeof(windowRectangle));
		windowRectangle.bottom = 0;
		windowRectangle.left = 0;
		windowRectangle.right = WINDOW_WIDTH;
		windowRectangle.top = WINDOW_HEIGHT;

		AdjustWindowRectEx(&windowRectangle, WS_OVERLAPPEDWINDOW, false, 0);
		HWND window = CreateWindowA(CLASS_NAME, WINDOW_NAME, WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,
			windowRectangle.right - windowRectangle.left, windowRectangle.top - windowRectangle.bottom, 0, 0, instance, 0);
		if (window == nullptr)
		{
			return EXIT_FAILURE;
		}

		const bool VSYNC_ENABLED = false;
		const bool FULLSCREEN_ENABLED = false;
		Direct3D::D3DTimeBuffer timeBuffer;
		Direct3D::D3DDevice device(window, WINDOW_WIDTH, WINDOW_HEIGHT);
		// NOTE: Set up device
		{
			if (!device.initialize(VSYNC_ENABLED, FULLSCREEN_ENABLED))
			{
				return EXIT_FAILURE;
			}

			if (!timeBuffer.initialize(device))
			{
				return EXIT_FAILURE;
			}
		}

		// NOTE: Debug rendering shader
		/*Rendering::DebugBoundingBox box;
		Direct3DResource::D3DShader debugShader;
		if (!debugShader.compile(device, "Shaders/rendering/forward_debug.hlsl",
			Direct3DResource::ShaderType::SHADER_TYPE_VERTEX | Direct3DResource::ShaderType::SHADER_TYPE_PIXEL)) {
			return EXIT_FAILURE;
		}

		if (!box.create(device, debugShader)) {
			return EXIT_FAILURE;
		}*/

		Rendering::PerspectiveCamera camera;
		Rendering::DeferredGeometryPass deferredGeometry;
		Rendering::DeferredLightPass deferredLighting;
		// NOTE: Set up deferred shading and camera
		{
			// NOTE: Set up deferred geometry pass
			{
				deferredGeometry.set_shader_path("Shaders/rendering/deferred.hlsl");
				deferredGeometry.set_target_size(WINDOW_WIDTH, WINDOW_HEIGHT);
				if (!deferredGeometry.initialize(device))
				{
					return EXIT_FAILURE;
				}
			}

			// NOTE: Set up deferred light pass
			{
				if (!deferredLighting.initialize(device, "Shaders/rendering/deferred_directional_lighting.hlsl"))
				{
					return EXIT_FAILURE;
				}

				deferredLighting.add_directional_light(XMFLOAT3(1, 1, 1), XMFLOAT3(1.64f, 1.27f, 0.99f));
				deferredLighting.add_directional_light(XMFLOAT3(0, 1, 0), XMFLOAT3(0.16f, 0.20f, 0.28f));
				deferredLighting.add_directional_light(XMFLOAT3(-1, 1, -1), XMFLOAT3(0.4f, 0.28f, 0.20f));

			}

			const float NEAR_PLANE = 0.1f;
			const float FAR_PLANE = 500.0f;
			const float VERTICAL_FOV = 75.0f;
			if (!camera.create(device, VERTICAL_FOV, WINDOW_WIDTH_F, WINDOW_HEIGHT_F, NEAR_PLANE, FAR_PLANE))
			{
				return EXIT_FAILURE;
			}

			camera.set_position(XMFLOAT3(0, 1, 0));
		}

		Mesh::MeshTextureStore textureStore;
		Mesh::ModelImporter importer(textureStore);
		std::vector<Mesh::MeshAsset> meshAssets;
		std::vector<Direct3DResource::D3DMesh> meshes;
		// Load meshes
		{
			XMVECTOR zero = XMVectorZero();
			std::vector<Mesh::MeshAsset> sponza;
			if (importer.load("Models/sponza.obj", sponza))
			{
				for (auto &asset : sponza)
				{
					XMVECTOR scale = XMVectorSet(0.01f, 0.01f, 0.01f, 0.0f);
					XMMATRIX transform = XMMatrixAffineTransformation(scale, zero, zero, zero);
					asset.set_world_transform(transform);
				}
			}

			std::vector<Mesh::MeshAsset> dragon;
			if (importer.load("Models/stanford-dragon.obj", dragon))
			{
				for (auto &asset : dragon)
				{
					XMVECTOR scale = XMVectorSet(0.1f, 0.1f, 0.1f, 0.0f);
					XMMATRIX transform = XMMatrixAffineTransformation(scale, zero, zero, zero);
					asset.set_world_transform(transform);
				}
			}

			textureStore.load_added_textures(device);
			
			for (auto &m : sponza)
			{
				Direct3DResource::D3DMesh mesh;
				mesh.initialize(device, m);
				meshes.push_back(mesh);
			}

			for (auto &m : dragon)
			{
				Direct3DResource::D3DMesh mesh;
				mesh.initialize(device, m);
				meshes.push_back(mesh);
			}
		}

		Direct3DResource::D3DRenderTarget offscreenTarget0;
		Direct3DResource::D3DRenderTarget offscreenTarget1;
		// Create offscreen buffers
		{
			uint8_t target0_flags = Direct3DResource::RenderTargetFlags::ENABLE_RTV;
			target0_flags |= Direct3DResource::RenderTargetFlags::ENABLE_SRV;
			target0_flags |= Direct3DResource::RenderTargetFlags::ENABLE_UAV;
			if (!offscreenTarget0.initialize(device, WINDOW_WIDTH, WINDOW_HEIGHT, target0_flags,
				DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 1))
			{
				return EXIT_FAILURE;
			}

			uint8_t target1_flags = Direct3DResource::RenderTargetFlags::ENABLE_RTV;
			target1_flags |= Direct3DResource::RenderTargetFlags::ENABLE_SRV;
			target1_flags |= Direct3DResource::RenderTargetFlags::ENABLE_UAV;
			if (!offscreenTarget1.initialize(device, WINDOW_WIDTH, WINDOW_HEIGHT, target1_flags,
				DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 1))
			{
				return EXIT_FAILURE;
			}
		}

		PostProcessing::D3DBloom bloom;
		PostProcessing::D3DFXAA fxaa;
		PostProcessing::D3DRenderToTexture targetToScreen;
		// Initialize post processing
		{
			if (!targetToScreen.initialize(device, "Shaders/passthrough.hlsl") ||
				!bloom.initialize(device, 480, 270, "Shaders/threshold.hlsl", "Shaders/blur.hlsl", "Shaders/merge.hlsl") ||
				!fxaa.initialize(device, "Shaders/fxaa.hlsl"))
			{
				return EXIT_FAILURE;
			}

			bloom.set_bloom_factor(0.5f);
			bloom.set_blur_passes(3);
			bloom.set_blur_strength(1.0f);
			bloom.set_exposure_factor(2.0f);
		}

		// NOTE: Add all the resources for a particle system
		Particles::ParticleEmitterShaders particleEmitterShaders(device);
		Particles::ParticleSystemSharedResources particleSharedResources;
		Particles::ParticleSystemTextureArray particleTextureArray(64, 64);
		Particles::ParticlePaintingPrealloc particlePainting(device, 400, 512, 512, 5);
		if (!particleSharedResources.create(device, WINDOW_WIDTH, WINDOW_HEIGHT) ||
			!particlePainting.initialize("Shaders/particles/particle_paint.hlsl")) {
			return EXIT_FAILURE;
		}

		particleEmitterShaders.add_emitter_shader(Particles::ParticleEmitterShape::SHAPE_SPHERE, "Shaders/particles/particle_emit_sphere.hlsl");
		particleEmitterShaders.add_emitter_shader(Particles::ParticleEmitterShape::SHAPE_CUBE, "Shaders/particles/particle_emit_cube.hlsl");
		particleEmitterShaders.add_emitter_shader(Particles::ParticleEmitterShape::SHAPE_CONE, "Shaders/particles/particle_emit_cone.hlsl");
		particleTextureArray.add("Textures/white_square.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		particleTextureArray.add("Textures/smoke.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		particleTextureArray.add("Textures/particle.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		particleTextureArray.add("Textures/particle_2.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		particleTextureArray.add("Textures/particle_3.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		particleTextureArray.add("Textures/fire_particle.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		particleTextureArray.add("Textures/splat_particle.png", Mesh::TextureFileType::TEXTURE_FILE_WIC);
		if (!particleSharedResources.create(device, WINDOW_WIDTH, WINDOW_HEIGHT) ||
			!particleTextureArray.load(device) ||
			!particlePainting.initialize("Shaders/particles/particle_paint.hlsl"))
		{
			return EXIT_FAILURE;
		}

		ParticleExamples::FireSystem fire0(device, particleEmitterShaders);
		ParticleExamples::FireSystem fire1(device, particleEmitterShaders);
		ParticleExamples::BloodSystem blood0(device, particleEmitterShaders);
		Particles::ParticleSystemBase * fire0Ptr = fire0.create(XMFLOAT3(10, 0, 1), &deferredGeometry, &timeBuffer, &camera, &particleSharedResources, &particleTextureArray);
		Particles::ParticleSystemBase * fire1Ptr = fire1.create(XMFLOAT3(10, 0, -1), &deferredGeometry, &timeBuffer, &camera, &particleSharedResources, &particleTextureArray);
		Particles::ParticleSystemBase * blood0Ptr = blood0.create(XMFLOAT3(-7, 1.5f, 0), &deferredGeometry, &timeBuffer, &camera, &particleSharedResources, &particleTextureArray);

		std::vector<Particles::ParticleSystemBase *> particleSystems;
		particleSystems.push_back(fire0Ptr);
		particleSystems.push_back(fire1Ptr);

		// NOTE: Set cursor to center of client area
		POINT clientCenter;
		clientCenter.x = WINDOW_WIDTH / 2;
		clientCenter.y = WINDOW_HEIGHT / 2;
		ClientToScreen(window, &clientCenter);
		SetCursorPos(clientCenter.x, clientCenter.y);

		float time = 0.0;
		float currentTime = get_high_res_time_in_seconds();
		float timeAccumulator = 0.0;

		Win32Input input;
		for (;;)
		{
			// NOTE: Intercept the message pump, and then send the message to the default window procedure
			MSG message = window_proc_intercept(&input);
			if (message.message == WM_QUIT)
			{
				break;
			}

			POINT cursorPoint;
			GetCursorPos(&cursorPoint);
			ScreenToClient(window, &cursorPoint);

			float mouse_delta_x = static_cast<float>(cursorPoint.x) - WINDOW_WIDTH_F * 0.5f;
			float mouse_delta_y = static_cast<float>(cursorPoint.y) - WINDOW_HEIGHT_F * 0.5f;

			// NOTE: Reset mouse position to center of client area
			if (windowHasFocus)
			{
				clientCenter.x = WINDOW_WIDTH / 2;
				clientCenter.y = WINDOW_HEIGHT / 2;
				ClientToScreen(window, &clientCenter);
				SetCursorPos(clientCenter.x, clientCenter.y);
				ShowCursor(0);
			}

			// NOTE: Get new delta time
			float newTime = get_high_res_time_in_seconds();
			float frameTime = newTime - currentTime;
			currentTime = newTime;
			time += frameTime;

			// NOTE: Reset delta time if window was out of focus etc.
			if (frameTime > 1.0f)
			{
				frameTime = 1.0f / 60.0f;
			}

			timeBuffer.update(device, frameTime, time);

			// NOTE: Update input if window has focus, for windowed mode
			if (windowHasFocus)
			{
				input.set_mouse_delta(mouse_delta_x, mouse_delta_y);
				camera.update(input, device.get_context(), 5.0f, frameTime);
			}

			// NOTE: Frustum culling
			XMFLOAT3 cameraPosition = camera.get_position();
			XMVECTOR cameraPositionVector = XMLoadFloat3(&cameraPosition);
			std::vector<Mesh::D3DMesh *> visibleMeshes;
			std::vector<Particles::ParticleSystemBase *> visibleParticleSystems;

			for (auto &it : meshes)
			{
				if (camera.intersects_frustum(it.get_bounding_box())) 
				{
					visibleMeshes.push_back(&it);
				}
			}

			for (auto &it : particleSystems)
			{
				if (camera.intersects_frustum(it->get_bounding_box()))
				{
					visibleParticleSystems.push_back(it);
				}
			}
			
			// NOTE: Rendering
			{
				device.set_depth_test(Direct3D::DepthTest::DEPTH_TEST_ON);
				deferredGeometry.render(device, camera, textureStore, visibleMeshes);
				device.set_depth_test(Direct3D::DepthTest::DEPTH_TEST_OFF);
				deferredLighting.render(device, camera, deferredGeometry, offscreenTarget0);
			}

			// NOTE: Particles
			{
				particleSharedResources.begin(device, offscreenTarget0.get_rtv());
				for (auto it : visibleParticleSystems) { it->emit(frameTime); }
				for (auto it : visibleParticleSystems) { it->update(); }
				for (auto it : visibleParticleSystems) { it->render(); }

				if (input.get_keyboard_key_state(true, VK_SPACE))
				{
					const float distance = 1.0f;
					XMFLOAT3 cameraPosition = camera.get_position();
					XMVECTOR cameraPositionVec = XMLoadFloat3(&cameraPosition);
					cameraPositionVec = cameraPositionVec + distance * camera.get_lookat();
					XMFLOAT3 particleSystemPosition;
					XMStoreFloat3(&particleSystemPosition, cameraPositionVec);
					blood0Ptr->set_position(particleSystemPosition);
					blood0Ptr->emit(frameTime);
					blood0Ptr->update();
					blood0Ptr->render();
				}

				visibleParticleSystems.push_back(blood0Ptr);
			}

			// NOTE: Post
			{
				bloom.render(device, offscreenTarget1.get_rtv(), offscreenTarget0);
				fxaa.render(device, device.get_rtv(), offscreenTarget1);
			}

			// NOTE: Painting
			{
				std::vector<Math::BoundingBox> checkBoxes;
				for each (auto system in visibleParticleSystems) {
					checkBoxes.push_back(system->get_bounding_box());
				}

				particlePainting.set_paints_per_frame(120);
				particlePainting.render(camera, visibleMeshes, deferredGeometry,
					particleSharedResources.get_collision_render_target(), checkBoxes);
			}

			device.present();

			// NOTE: Save previous key states
			input.carry_current_keystate();
			input.carry_current_mouse_state();
		}

	 	PostQuitMessage(EXIT_SUCCESS);
	}

#if _DEBUG
	FreeConsole();
#endif

	return EXIT_SUCCESS;
}