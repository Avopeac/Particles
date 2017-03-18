#include <Precompiled.h>

#include "PerspectiveCamera.h"

using namespace Direct3D;
using namespace Win32;
using namespace Rendering;

Rendering::PerspectiveCamera::PerspectiveCamera() :
	m_fieldOfView(0),
	m_aspectRatio(0),
	m_nearDistance(0),
	m_farDistance(0),
	m_nearHeight(0),
	m_nearWidth(0),
	m_farHeight(0),
	m_farWidth(0),
	m_dirty(false) 
{
	ZeroMemory(&m_position, sizeof(m_position));
	ZeroMemory(&m_eulerAngles, sizeof(m_eulerAngles));
	ZeroMemory(&m_matrixData, sizeof(m_matrixData));
	m_normalMatrix = XMMatrixIdentity();
	m_viewMatrix = XMMatrixIdentity();
	m_projectionMatrix = XMMatrixIdentity();
	m_viewProjectionMatrix= XMMatrixIdentity();
	m_previousViewProjectionMatrix = XMMatrixIdentity();
}

Rendering::PerspectiveCamera::~PerspectiveCamera()
{

}

void Rendering::PerspectiveCamera::update(Win32Input &input, ID3D11DeviceContext * context, float pixels_per_second, float dt)
{
	// NOTE: Rebuild the view frustum
	if (input.get_keyboard_key_state(true, WIN32_VK_F) == KeyState::KEY_DOWN)
	{
		m_frustumDebug = true;
	}

	move_camera(input, pixels_per_second, dt);
	rotate_camera(input, dt);

	if (m_dirty) 
	{
		// NOTE: The original orthonormal base is known
		XMFLOAT3 up = XMFLOAT3(0, 1, 0);
		XMFLOAT3 forward = XMFLOAT3(0, 0, 1);
		XMVECTOR upVector = XMLoadFloat3(&up);
		XMVECTOR forwardVector = XMLoadFloat3(&forward);
		XMVECTOR positionVector = XMLoadFloat3(&m_position);

		float yaw = XMConvertToRadians(m_eulerAngles.x);
		float pitch = XMConvertToRadians(m_eulerAngles.y);
		float roll = XMConvertToRadians(m_eulerAngles.z);
		XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

		// NOTE: Create new orthonormal base for the view matrix
		forwardVector = XMVector3TransformCoord(forwardVector, rotationMatrix);
		upVector = XMVector3TransformCoord(upVector, rotationMatrix);
		m_baseLookAtVector = forwardVector;
		m_baseUpVector = upVector;
		m_baseRightVector = XMVector3Cross(upVector, forwardVector);
		forwardVector = XMVectorAdd(positionVector, forwardVector);

		// NOTE: Compute matrices
		m_previousViewProjectionMatrix = m_viewProjectionMatrix;
		m_viewMatrix= XMMatrixLookAtLH(positionVector, forwardVector, upVector);
		m_viewProjectionMatrix= XMMatrixMultiply(m_viewMatrix, m_projectionMatrix);

		// NOTE: Upload matrix data to constant buffer
		m_matrixData;
		m_matrixData.position = m_position;
		m_matrixData.view = m_viewMatrix;
		m_matrixData.normal = XMMatrixInverse(0, XMMatrixTranspose(m_viewMatrix));
		m_matrixData.projection = m_projectionMatrix;
		m_matrixData.viewProjection = m_viewProjectionMatrix;
		m_matrixData.previousViewProjection = m_previousViewProjectionMatrix;
		m_matrixData.inverseProjection = XMMatrixInverse(0, m_projectionMatrix);
		m_matrixData.inverseView = XMMatrixInverse(0, m_viewMatrix);
		m_matrixData.inverseViewProjection = XMMatrixInverse(0, m_viewProjectionMatrix);
		m_matrixBuffer.map_data(context, &m_matrixData);

		if (!m_frustumDebug)
		{
			update_frustum();
		}

		m_dirty = false;
	}
}

ID3D11Buffer * Rendering::PerspectiveCamera::get_matrix_buffer() const
{
	return m_matrixBuffer.get_data();
}

bool Rendering::PerspectiveCamera::intersects_frustum(const Math::BoundingBox & boundingBox) const
{
	for (uint8_t index = 0; index < FRUSTUM_PLANE_COUNT; ++index)
	{
		int8_t outside = 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(false, false, false))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(false, false, true))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(false, true, false))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(false, true, true))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(true, false, false))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(true, false, true))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(true, true, false))) < 0.0f ? 1 : 0;
		outside += XMVectorGetX(XMPlaneDot(m_frustumPlanes[index], boundingBox.get_point(true, true, true))) < 0.0f ? 1 : 0;

		// NOTE: If none of the corners of the BoundingBox intersects camera frustum
		if(outside == 8)
		{
			return false;
		}
	}

	XMVECTOR maximum = boundingBox.get_maximum();
	XMVECTOR minimum = boundingBox.get_minimum();

	int8_t outside = 0;

	// NOTE: Check the maximum X point
	{
		for (uint8_t index = 0; index < FRUSTUM_CORNER_COUNT; ++index)
		{
			outside += XMVectorGetX(m_frustumCorners[index]) > XMVectorGetX(maximum) ? 1 : 0;
		}

		if (outside == FRUSTUM_CORNER_COUNT)
		{
			return false;
		}
	}

	// NOTE: Check the minimum X point
	{
		outside = 0;
		for (uint8_t index = 0; index < FRUSTUM_CORNER_COUNT; ++index)
		{
			outside += XMVectorGetX(m_frustumCorners[index]) < XMVectorGetX(minimum) ? 1 : 0;
		}

		if (outside == FRUSTUM_CORNER_COUNT)
		{
			return false;
		}

	}

	// NOTE: Check maximum Y point
	{
		outside = 0;
		for (uint8_t index = 0; index < FRUSTUM_CORNER_COUNT; ++index)
		{
			outside += XMVectorGetY(m_frustumCorners[index]) > XMVectorGetY(maximum) ? 1 : 0;
		}

		if (outside == FRUSTUM_CORNER_COUNT)
		{
			return false;
		}
	}

	// NOTE: Check minimum Y point
	{
		outside = 0;
		for (uint8_t index = 0; index < FRUSTUM_CORNER_COUNT; ++index)
		{
			outside += XMVectorGetY(m_frustumCorners[index]) < XMVectorGetY(minimum) ? 1 : 0;
		}

		if (outside == FRUSTUM_CORNER_COUNT)
		{
			return false;
		}
	}
	
	// NOTE: Check maximum Z point
	{
		outside = 0;
		for (uint8_t index = 0; index < FRUSTUM_CORNER_COUNT; ++index)
		{
			outside += XMVectorGetZ(m_frustumCorners[index]) > XMVectorGetZ(maximum) ? 1 : 0;
		}

		if (outside == FRUSTUM_CORNER_COUNT)
		{
			return false;
		}
	}

	// NOTE: Check minimum Z point
	{
		outside = 0;
		for (uint8_t index = 0; index < FRUSTUM_CORNER_COUNT; ++index)
		{
			outside += XMVectorGetZ(m_frustumCorners[index]) > XMVectorGetZ(maximum) ? 1 : 0;
		}

		if (outside == FRUSTUM_CORNER_COUNT)
		{
			return false;
		}
	}
	
	return true;
}

void Rendering::PerspectiveCamera::move_camera(Win32Input & input, float pixelsPerSecond, float deltaTime)
{
	XMVECTOR oldPosition = XMLoadFloat3(&m_position);
	XMVECTOR newPosition = XMLoadFloat3(&m_position);

	float moveSpeed = pixelsPerSecond * deltaTime;

	// NOTE: New position, left and right movement
	if (input.get_keyboard_key_state(true, WIN32_VK_A) == KeyState::KEY_DOWN)
	{
		newPosition = XMVectorSubtract(newPosition, XMVectorScale(m_baseRightVector, moveSpeed));
	}
	else if (input.get_keyboard_key_state(true, WIN32_VK_D) == KeyState::KEY_DOWN)
	{
		newPosition = XMVectorAdd(newPosition, XMVectorScale(m_baseRightVector, moveSpeed));
	}

	// NOTE: New position, forward and back movement
	if (input.get_keyboard_key_state(true, WIN32_VK_W) == KeyState::KEY_DOWN)
	{
		newPosition = XMVectorAdd(newPosition, XMVectorScale(m_baseLookAtVector, moveSpeed));
	}
	else if (input.get_keyboard_key_state(true, WIN32_VK_S) == KeyState::KEY_DOWN)
	{
		newPosition = XMVectorSubtract(newPosition, XMVectorScale(m_baseLookAtVector, moveSpeed));
	}

	// NOTE: New position, up and down movement
	if (input.get_keyboard_key_state(true, WIN32_VK_Q) == KeyState::KEY_DOWN)
	{
		newPosition = XMVectorAdd(newPosition, XMVectorScale(m_baseUpVector, moveSpeed));
	}
	else if (input.get_keyboard_key_state(true, WIN32_VK_E) == KeyState::KEY_DOWN)
	{
		newPosition = XMVectorSubtract(newPosition, XMVectorScale(m_baseUpVector, moveSpeed));
	}

	// NOTE: Need to update view matrix if there's movement
	if (XMVector3NotEqual(oldPosition, newPosition))
	{
		XMStoreFloat3(&m_position, newPosition);
		m_dirty = true;
	}
}

void Rendering::PerspectiveCamera::rotate_camera(Win32Input & input, float deltaTime)
{
	XMFLOAT3 newEulerAngles = m_eulerAngles;
	newEulerAngles.x += 15.0f * input.get_mouse_delta_x() * deltaTime;
	newEulerAngles.y += 15.0f * input.get_mouse_delta_y() * deltaTime;
	XMVECTOR newEulerAnglesVector = XMLoadFloat3(&newEulerAngles);
	XMVECTOR oldEulerAnglesVector = XMLoadFloat3(&m_eulerAngles);

	// NOTE: Need to update view matrix if there's movement
	if (XMVector3NotEqual(oldEulerAnglesVector, newEulerAnglesVector)) 
	{
		XMStoreFloat3(&m_eulerAngles, newEulerAnglesVector);
		m_dirty = true;
	}
}

void Rendering::PerspectiveCamera::update_frustum()
{
	XMVECTOR eyePosition = XMVectorSet(m_position.x, m_position.y, m_position.z, 1);
	XMVECTOR nearCenter = eyePosition + m_baseLookAtVector * m_nearDistance;
	XMVECTOR farCenter = eyePosition + m_baseLookAtVector * m_farDistance;

	XMVECTOR nearHorizontalOffset = m_baseRightVector * m_nearWidth * 0.5f;
	XMVECTOR nearVerticalOffset = m_baseUpVector * m_nearHeight * 0.5f;
	XMVECTOR farHorizontalOffset = m_baseRightVector * m_farWidth * 0.5f;
	XMVECTOR farVerticalOffset = m_baseUpVector * m_farHeight * 0.5f;

	m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_LEFT] = nearCenter - nearHorizontalOffset + nearVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_LEFT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_LEFT], 1);
	m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_RIGHT] = nearCenter + nearHorizontalOffset + nearVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_RIGHT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_RIGHT], 1);
	m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_LEFT] = nearCenter - nearHorizontalOffset - nearVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_LEFT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_LEFT], 1);
	m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT] = nearCenter + nearHorizontalOffset - nearVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT], 1);
	m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_LEFT] = farCenter - farHorizontalOffset + farVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_LEFT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_LEFT], 1);
	m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_RIGHT] = farCenter + farHorizontalOffset + farVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_RIGHT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_RIGHT], 1);
	m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_LEFT] = farCenter - farHorizontalOffset - farVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_LEFT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_LEFT], 1);
	m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_RIGHT] = farCenter + farHorizontalOffset - farVerticalOffset;
	m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_RIGHT] = XMVectorSetW(m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_RIGHT], 1);

	m_frustumPlanes[FRUSTUM_PLANE_TOP] = XMPlaneFromPoints(m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_RIGHT], m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_RIGHT], m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_LEFT]);
	m_frustumPlanes[FRUSTUM_PLANE_BOTTOM] = XMPlaneFromPoints(m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_LEFT], m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_RIGHT], m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT]);
	m_frustumPlanes[FRUSTUM_PLANE_LEFT] = XMPlaneFromPoints(m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_LEFT], m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_LEFT], m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_LEFT]);
	m_frustumPlanes[FRUSTUM_PLANE_RIGHT] = XMPlaneFromPoints(m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT], m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_RIGHT], m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_RIGHT]);
	m_frustumPlanes[FRUSTUM_PLANE_NEAR] = XMPlaneFromPoints(m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_RIGHT], m_frustumCorners[FRUSTUM_CORNER_NEAR_TOP_LEFT], m_frustumCorners[FRUSTUM_CORNER_NEAR_BOTTOM_RIGHT]);
	m_frustumPlanes[FRUSTUM_PLANE_FAR] = XMPlaneFromPoints(m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_LEFT], m_frustumCorners[FRUSTUM_CORNER_FAR_TOP_RIGHT], m_frustumCorners[FRUSTUM_CORNER_FAR_BOTTOM_LEFT]);
}

void PerspectiveCamera::set_position(const XMFLOAT3 &position)
{
	m_dirty = true;
	m_position = position;
}

XMFLOAT3 Rendering::PerspectiveCamera::get_position() const
{
	return m_position;
}

XMVECTOR Rendering::PerspectiveCamera::get_lookat() const
{
	return m_baseLookAtVector;
}

void PerspectiveCamera::set_rotation(const XMFLOAT3 &eulerAngles)
{
	m_dirty = true;
	m_eulerAngles = eulerAngles;
}

bool PerspectiveCamera::create(const D3DDevice &device, float fieldOfView, float width, float height, float min, float max)
{
	if (!m_matrixBuffer.create(device)) 
	{
		return false;
	}

	m_aspectRatio = width / height;
	m_fieldOfView = fieldOfView;
	m_nearDistance = min;
	m_farDistance = max;
	float fovRadians = XMConvertToRadians(m_fieldOfView);
	m_nearHeight = 2.0f * tanf(fovRadians * 0.5f) * min;
	m_farHeight = 2.0f * tanf(fovRadians * 0.5f) * max;
	m_nearWidth = m_aspectRatio * m_nearHeight;
	m_farWidth = m_aspectRatio * m_farHeight;

	m_projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, m_aspectRatio, min, max);

	return true;
}