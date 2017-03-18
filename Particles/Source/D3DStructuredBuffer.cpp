#include <Precompiled.h>

#include "D3DStructuredBuffer.h"

using namespace Direct3D;
using namespace Direct3DResource;

Direct3DResource::D3DStructuredBuffer::D3DStructuredBuffer() :
	m_hasAppend(false),
	m_hasCounter(false),
	m_elementCount(0),
	m_elementStride(0),
	m_bufferFormat(DXGI_FORMAT_UNKNOWN)
{

}

Direct3DResource::D3DStructuredBuffer::~D3DStructuredBuffer()
{

}

bool Direct3DResource::D3DStructuredBuffer::create(const D3DDevice & device, uint32_t elements, uint32_t stride, bool has_counter, bool has_append)
{
	if (elements == 0 || stride == 0) 
	{
		return false; 
	}

	m_bufferFormat = DXGI_FORMAT_UNKNOWN;
	m_elementCount = elements;
	m_elementStride = stride;
	m_hasCounter = has_counter;
	m_hasAppend = has_append;
	
	if (!create_buffer(device) || !create_srv(device) || !create_uav(device))
	{
		return false;
	}
	
	if (m_hasCounter || m_hasAppend)
	{
		if (!create_counter_buffer(device))
		{
			return false;
		}
	}

	return true;
}

ID3D11UnorderedAccessView * const Direct3DResource::D3DStructuredBuffer::get_uav() const
{
	return m_pUnorderedAccessView.Get();
}

ID3D11ShaderResourceView * const Direct3DResource::D3DStructuredBuffer::get_srv() const
{
	return m_pShaderResourceView.Get();
}

ID3D11Buffer * const Direct3DResource::D3DStructuredBuffer::get_buffer() const
{
	return m_pBuffer.Get();
}

uint32_t Direct3DResource::D3DStructuredBuffer::get_counter(const D3DDevice &device) const
{

	ID3D11DeviceContext * context = device.get_context();
	if (m_hasCounter || m_hasAppend)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(mappedResource));
		context->CopyStructureCount(m_pCounterBuffer.Get(), 0, m_pUnorderedAccessView.Get());
		context->Map(m_pCounterBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedResource);
		uint32_t * ret_counter = (uint32_t *)mappedResource.pData;
		context->Unmap(m_pCounterBuffer.Get(), 0);
		return *ret_counter;
	}

	return 0;
}

bool Direct3DResource::D3DStructuredBuffer::create_srv(const D3DDevice &device)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Buffer.NumElements = m_elementCount;
	desc.Format = m_bufferFormat;
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	HRESULT hr = device.get_device()->CreateShaderResourceView(m_pBuffer.Get(), &desc, m_pShaderResourceView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create raw structured buffer shader resource view.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DStructuredBuffer::create_uav(const D3DDevice &device)
{
	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format = m_bufferFormat;
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.NumElements = m_elementCount;
	desc.Buffer.Flags = 0;

	if (m_hasCounter && !m_hasAppend) {
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	}
	else if (!m_hasCounter && m_hasAppend) {
		desc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	}

	HRESULT hr = device.get_device()->CreateUnorderedAccessView(m_pBuffer.Get(), &desc, m_pUnorderedAccessView.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create raw structured buffer unordered access view.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DStructuredBuffer::create_buffer(const D3DDevice &device)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = m_elementStride * m_elementCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = m_elementStride;
	HRESULT hr = device.get_device()->CreateBuffer(&desc, 0, m_pBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create raw structured buffer.", 0, 0);
		return false;
	}
	
	return true;
}

bool Direct3DResource::D3DStructuredBuffer::create_counter_buffer(const D3DDevice &device)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ByteWidth = sizeof(uint32_t);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	HRESULT hr = device.get_device()->CreateBuffer(&desc, 0, m_pCounterBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create raw structured buffer counter buffer.", 0, 0);
		return false;
	}

	return true;
}
