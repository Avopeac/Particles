#pragma once

namespace Math
{
	using namespace DirectX;

	class BoundingBox
	{
		XMVECTOR m_center;
		XMVECTOR m_minimum;
		XMVECTOR m_maximum;
		
	public:

		BoundingBox();
		~BoundingBox();
		BoundingBox(const BoundingBox &);
		BoundingBox(BoundingBox &&);

		BoundingBox &operator=(const BoundingBox &other)
		{
			if (this != &other)
			{
				m_center = other.m_center;
				m_minimum = other.m_minimum;
				m_maximum = other.m_maximum;
			}

			return *this;
		}

		BoundingBox &operator=(BoundingBox &&other)
		{
			if (this != &other)
			{
				m_center = other.m_center;
				m_minimum = other.m_minimum;
				m_maximum = other.m_maximum;
			}

			return *this;
		}

		// NOTE: Apply an affine transformation to the bounding box, no rotation
		void transform(const XMMATRIX &affineTransformation);

		void set(const XMVECTOR &minimum, const XMVECTOR &maximum);
		
		bool intersects(const BoundingBox &other) const;

		XMVECTOR get_center() const;
		XMVECTOR get_minimum() const;
		XMVECTOR get_maximum() const;
		XMVECTOR get_point(bool minX, bool minY, bool minZ) const;

	protected:

	private:

	};
}
