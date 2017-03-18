#pragma once
#include "Spline.h"

namespace Math
{
	class SplineChain
	{
		bool m_splineLooping;
		uint32_t m_currentSplineIndex;
		std::vector<Spline> m_splineChain;

	public:
		SplineChain();
		~SplineChain();
		SplineChain(const SplineChain &);
		SplineChain(SplineChain &&);

		SplineChain &operator=(const SplineChain &other)
		{
			if (this != &other)
			{
				m_splineLooping = other.m_splineLooping;
				m_splineChain = other.m_splineChain;
				m_currentSplineIndex = other.m_currentSplineIndex;
			}

			return *this;
		}

		SplineChain &operator=(SplineChain && other)
		{
			if (this != &other)
			{
				m_splineLooping = other.m_splineLooping;
				m_splineChain = other.m_splineChain;
				m_currentSplineIndex = other.m_currentSplineIndex;
			}

			return *this;
		}

		bool empty() const;
		void set_looping(bool looping);
		void add_spline(const Spline &spline);
		XMVECTOR update(float deltaTime);
	
	};
}
