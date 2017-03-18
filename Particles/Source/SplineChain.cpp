#include <Precompiled.h>

#include "SplineChain.h"

Math::SplineChain::SplineChain() :
	m_splineLooping(false),
	m_currentSplineIndex(0)
{

}

Math::SplineChain::~SplineChain()
{

}

Math::SplineChain::SplineChain(const SplineChain & other)
{
	assert(this != &other);
	m_splineLooping = other.m_splineLooping;
	m_splineChain = other.m_splineChain;
	m_currentSplineIndex = other.m_currentSplineIndex;
}

Math::SplineChain::SplineChain(SplineChain && other)
{
	assert(this != &other);
	m_splineLooping = other.m_splineLooping;
	m_splineChain = other.m_splineChain;
	m_currentSplineIndex = other.m_currentSplineIndex;
}

bool Math::SplineChain::empty() const
{
	return m_splineChain.empty();
}

void Math::SplineChain::set_looping(bool looping)
{
	m_splineLooping = looping;
}

void Math::SplineChain::add_spline(const Spline & spline)
{
	m_splineChain.push_back(spline);
}

DirectX::XMVECTOR Math::SplineChain::update(float deltaTime)
{
	// NOTE: Spline interpolation for particle system
	bool atEnd = false;
	XMVECTOR splinePosition;
	if (!m_splineChain.empty())
	{
		if (m_splineLooping && (m_currentSplineIndex > m_splineChain.size() - 1))
		{
			m_currentSplineIndex = 0;
			for (auto &spline : m_splineChain)
			{
				spline.reset();
			}
		}

		if (m_currentSplineIndex < m_splineChain.size())
		{
			splinePosition = m_splineChain[m_currentSplineIndex].update(deltaTime, atEnd);
			if (atEnd && m_currentSplineIndex < m_splineChain.size())
			{
				++m_currentSplineIndex;
			}
		}
	}

	return splinePosition;
}
