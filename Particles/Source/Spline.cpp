#include <Precompiled.h>

#include "Spline.h"

using namespace DirectX;
using namespace Math;

Math::Spline::Spline(float time) :
	m_time(time),
	m_currentTime(0.0f)
{
	ZeroMemory(&m_point0, sizeof(m_point0));
	ZeroMemory(&m_point1, sizeof(m_point1));
	ZeroMemory(&m_point2, sizeof(m_point2));
	ZeroMemory(&m_point3, sizeof(m_point3));
}

Math::Spline::Spline(const XMVECTOR & point0, const XMVECTOR & point1, const XMVECTOR & point2, const XMVECTOR & point3, float time) :
	Spline(time)
{
	m_point0 = point0;
	m_point1 = point1;
	m_point2 = point2;
	m_point3 = point3;
}

Math::Spline::~Spline()
{

}

Math::Spline::Spline(const Spline &other)
{
	assert(this != &other);
	m_currentTime = other.m_currentTime;
	m_time = other.m_time;
	m_point0 = other.m_point0;
	m_point1 = other.m_point1;
	m_point2 = other.m_point2;
	m_point3 = other.m_point3;
}

Math::Spline::Spline(Spline &&other)
{
	assert(this != &other);
	m_currentTime = other.m_currentTime;
	m_time = other.m_time;
	m_point0 = other.m_point0;
	m_point1 = other.m_point1;
	m_point2 = other.m_point2;
	m_point3 = other.m_point3;
}

XMVECTOR Math::Spline::interpolate()
{
	float t0 = m_currentTime / m_time;
	float i0 = 1.0f - t0;
	float i1 = i0 * i0;
	float t1 = t0 * t0;
	return m_point0 * (i1 * i0) + m_point1 * (3.0f * i1 * t0) + m_point2 * (3.0f * i0 * t1) + m_point3 * (t1 * t0);
}

void Math::Spline::reset()
{
	m_currentTime = 0.0f;
}

XMVECTOR Math::Spline::update(float deltaTime, bool &atEnd)
{
	atEnd = false;
	if (m_currentTime + deltaTime <= m_time)
	{
		m_currentTime += deltaTime;
	}
	else 
	{
		atEnd = true;
	}

	return interpolate();
}
