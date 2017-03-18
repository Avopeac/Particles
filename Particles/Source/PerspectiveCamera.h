#pragma once

#include "D3DDevice.h"
#include "D3DConstantBuffer.h"

#include "BoundingBox.h"
#include "Win32Input.h"

namespace Rendering
{

	using namespace DirectX;
	using namespace Direct3DResource;
	using namespace Direct3D;

	// NOTE: The position of the matrix buffer is in world-space coordinates
	struct MatrixBuffer
	{
		XMFLOAT3 position;
		XMMATRIX normal;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX viewProjection;
		XMMATRIX previousViewProjection;
		XMMATRIX inverseProjection;
		XMMATRIX inverseView;
		XMMATRIX inverseViewProjection;
	};


	enum FrustumPlane
	{
		FRUSTUM_PLANE_TOP,
		FRUSTUM_PLANE_NEAR,
		FRUSTUM_PLANE_LEFT,
		FRUSTUM_PLANE_RIGHT,
		FRUSTUM_PLANE_BOTTOM,
		FRUSTUM_PLANE_FAR,
		FRUSTUM_PLANE_UNKNOWN,
		FRUSTUM_PLANE_COUNT = FRUSTUM_PLANE_UNKNOWN,
	};

	enum FrustumCorner
	{
		FRUSTUM_CORNER_FAR_TOP_LEFT,
		FRUSTUM_CORNER_FAR_TOP_RIGHT,
		FRUSTUM_CORNER_FAR_BOTTOM_LEFT,
		FRUSTUM_CORNER_FAR_BOTTOM_RIGHT,
		FRUSTUM_CORNER_NEAR_TOP_LEFT,
		FRUSTUM_CORNER_NEAR_TOP_RIGHT,
		FRUSTUM_CORNER_NEAR_BOTTOM_LEFT,
		FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT,
		FRUSTUM_CORNER_UNKNOWN,
		FRUSTUM_CORNER_COUNT = FRUSTUM_CORNER_UNKNOWN,
	};

	// Basic camera class, with a perspective projection. This supplies the shader stages with matrix transforms.
	// Author: Andreas Larsson
	// Date: 2016-12-09 (YYYY-MM-DD)
	class PerspectiveCamera
	{
		bool m_dirty;
		float m_fieldOfView;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;
		float m_nearHeight;
		float m_nearWidth;
		float m_farHeight;
		float m_farWidth;
		XMFLOAT3 m_position;
		XMFLOAT3 m_eulerAngles;
		
		XMMATRIX m_normalMatrix;
		XMMATRIX m_viewMatrix;
		XMMATRIX m_projectionMatrix;
		XMMATRIX m_viewProjectionMatrix;
		XMMATRIX m_previousViewProjectionMatrix;
		XMMATRIX m_inverseProjectionMatrix;
		XMMATRIX m_inverseViewMatrix;
		XMMATRIX m_inverseViewProjectionMatrix;

		XMVECTOR m_baseUpVector;
		XMVECTOR m_baseRightVector;
		XMVECTOR m_baseLookAtVector;
		MatrixBuffer m_matrixData;
		D3DConstantBuffer<MatrixBuffer> m_matrixBuffer;
		XMVECTOR m_frustumCorners[FRUSTUM_CORNER_COUNT];
		XMVECTOR m_frustumPlanes[FRUSTUM_PLANE_COUNT];

		bool m_frustumDebug = false;

	public:
		PerspectiveCamera();
		~PerspectiveCamera();
		PerspectiveCamera(const PerspectiveCamera &) = delete;
		PerspectiveCamera(PerspectiveCamera &&) = delete;
		PerspectiveCamera &operator=(const PerspectiveCamera &) = delete;
		PerspectiveCamera &operator=(PerspectiveCamera &&) = delete;

		// NOTE: Creates the matrix buffer used for shader stages, FOV is vertical FOV
		bool create(const D3DDevice &device, float fieldOfView, float width, float height, float min, float max);

		void set_position(const XMFLOAT3 &position);

		XMFLOAT3 get_position() const;

		XMVECTOR get_lookat() const;

		void set_rotation(const XMFLOAT3 &eulerAngles);

		void update(Win32::Win32Input &input, ID3D11DeviceContext * context, float pixels_per_second, float dt);

		ID3D11Buffer * get_matrix_buffer() const;

		bool intersects_frustum(const Math::BoundingBox &boundingBox) const;

	private:
		void move_camera(Win32::Win32Input &input, float pixelsPerSecond, float deltaTime);
		void rotate_camera(Win32::Win32Input &input, float deltaTime);
		void update_frustum();
	};
}