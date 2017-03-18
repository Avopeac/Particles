#include <Precompiled.h>

#include "D3DTimeBuffer.h"

using namespace Direct3D;

Direct3D::D3DTimeBuffer::D3DTimeBuffer() 
{

}

Direct3D::D3DTimeBuffer::~D3DTimeBuffer()
{

}

bool Direct3D::D3DTimeBuffer::initialize(const D3DDevice &device)
{
	return m_buffer.create(device);
}

void Direct3D::D3DTimeBuffer::update(const D3DDevice &device, float deltaTime, float elapsedTime)
{
	m_deltaTime = deltaTime;
	m_elapsedTime = elapsedTime;
	XMFLOAT4 data = XMFLOAT4(deltaTime, elapsedTime, 0, 0);
	m_buffer.map_data(device.get_context(), &data);
}

const D3DConstantBuffer<XMFLOAT4>& Direct3D::D3DTimeBuffer::get_buffer() const
{
	return m_buffer;
}

float Direct3D::D3DTimeBuffer::get_delta_time() const
{
	return m_deltaTime;
}

float Direct3D::D3DTimeBuffer::get_elapsed_time() const
{
	return m_elapsedTime;
}
