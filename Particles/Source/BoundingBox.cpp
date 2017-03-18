#include <Precompiled.h>

#include "BoundingBox.h"

using namespace Math;

Math::BoundingBox::BoundingBox()
{
	ZeroMemory(&m_center, sizeof(m_center));
	ZeroMemory(&m_minimum, sizeof(m_minimum));
	ZeroMemory(&m_maximum, sizeof(m_maximum));
}

Math::BoundingBox::~BoundingBox()
{

}

Math::BoundingBox::BoundingBox(const BoundingBox &other)
{
	assert(this != &other);
	m_center = other.m_center;
	m_minimum = other.m_minimum;
	m_maximum = other.m_maximum;
}

Math::BoundingBox::BoundingBox(BoundingBox &&other)
{
	assert(this != &other);
	m_center = other.m_center;
	m_minimum = other.m_minimum;
	m_maximum = other.m_maximum;
}

void Math::BoundingBox::set(const XMVECTOR & minimum, const XMVECTOR & maximum)
{
	m_minimum = minimum;
	m_maximum = maximum;
	m_center = 0.5f * (maximum + minimum);
}

void Math::BoundingBox::transform(const XMMATRIX &affineTransform)
{
	m_center = XMVector4Transform(m_center, affineTransform);
	m_minimum = XMVector4Transform(m_minimum, affineTransform);
	m_maximum = XMVector4Transform(m_maximum, affineTransform);
}

XMVECTOR Math::BoundingBox::get_center() const
{
	return m_center;
}

XMVECTOR Math::BoundingBox::get_minimum() const
{
	return m_minimum;
}

XMVECTOR Math::BoundingBox::get_maximum() const
{
	return m_maximum;
}

XMVECTOR Math::BoundingBox::get_point(bool minX, bool minY, bool minZ) const
{
	float x = minX ? XMVectorGetX(m_minimum) : XMVectorGetX(m_maximum);
	float y = minY ? XMVectorGetY(m_minimum) : XMVectorGetY(m_maximum);
	float z = minZ ? XMVectorGetZ(m_minimum) : XMVectorGetZ(m_maximum);
	return XMVectorSet(x, y, z, 1);
}

bool Math::BoundingBox::intersects(const BoundingBox & other) const
{
	XMVECTOR minimum = other.get_minimum();
	XMVECTOR maximum = other.get_maximum();
	
	if ((XMVectorGetX(m_minimum) <= XMVectorGetX(maximum) && XMVectorGetX(m_maximum) >= XMVectorGetX(minimum)) &&
		(XMVectorGetY(m_minimum) <= XMVectorGetY(maximum) && XMVectorGetY(m_maximum) >= XMVectorGetY(minimum)) &&
		(XMVectorGetZ(m_minimum) <= XMVectorGetZ(maximum) && XMVectorGetZ(m_maximum) >= XMVectorGetZ(minimum)))
	{
		return true; 
	}

	return false;
}
