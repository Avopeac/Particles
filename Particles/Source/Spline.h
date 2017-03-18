#pragma once

namespace Math
{
	using namespace DirectX;

	// A simple class for cubic spline interpolation.
	// Author: Andreas Larsson
	// Date: 2016-12-12 (YYYY-MM-DD)
	class Spline
	{
		float m_currentTime;
		float m_time;
		XMVECTOR m_point0;
		XMVECTOR m_point1;
		XMVECTOR m_point2;
		XMVECTOR m_point3;

	public:

		Spline(const XMVECTOR &point0, const XMVECTOR &point1, const XMVECTOR &point2, const XMVECTOR &point3, float time);
		Spline(float time);
		~Spline();
		Spline(const Spline &);
		Spline(Spline &&);

		Spline &operator=(const Spline &other)
		{
			if (this != &other)
			{
				m_currentTime = other.m_currentTime;
				m_time = other.m_time;
				m_point0 = other.m_point0;
				m_point1 = other.m_point1;
				m_point2 = other.m_point2;
				m_point3 = other.m_point3;
			}
			
			return *this;
		}

		Spline &operator=(Spline &&other)
		{
			if (this != &other)
			{
				m_currentTime = other.m_currentTime;
				m_time = other.m_time;
				m_point0 = other.m_point0;
				m_point1 = other.m_point1;
				m_point2 = other.m_point2;
				m_point3 = other.m_point3;
			}

			return *this;
		}

		void reset();

		XMVECTOR update(float deltaTime, bool &atEnd);

	protected:
		
	private:

		XMVECTOR interpolate();

	};
}
