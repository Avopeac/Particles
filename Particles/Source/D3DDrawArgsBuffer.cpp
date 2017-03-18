#include <Precompiled.h>

#include "D3DDrawArgsBuffer.h"

using namespace Direct3DResource;

Direct3DResource::D3DDrawArgsBuffer::D3DDrawArgsBuffer()
{

}

Direct3DResource::D3DDrawArgsBuffer::~D3DDrawArgsBuffer()
{

}

bool Direct3DResource::D3DDrawArgsBuffer::create(const D3DDevice &device)
{
	// NOTE: Create indirect arguments buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.ByteWidth = 5 * sizeof(uint32_t);
		desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		desc.Usage = D3D11_USAGE_DEFAULT;
		HRESULT hr = device.get_device()->CreateBuffer(&desc, 0, m_pDrawArgsBuffer.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxA(0, "Failed to create indirect arguments buffer.", 0, 0);
			return false;
		}
	}

	// NOTE: Create indirect arguments UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_UINT;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = 5;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		HRESULT hr = device.get_device()->CreateUnorderedAccessView(m_pDrawArgsBuffer.Get(), &desc, m_pDrawArgsUnorderedAccessView.GetAddressOf());
		if (FAILED(hr))
		{
			MessageBoxA(0, "Failed to create indirect arguments uav.", 0, 0);
			return false;
		}
	}

	return true;
}

ID3D11Buffer * Direct3DResource::D3DDrawArgsBuffer::get_buffer() const
{
	return m_pDrawArgsBuffer.Get();
}

ID3D11UnorderedAccessView * Direct3DResource::D3DDrawArgsBuffer::get_uav() const
{
	return m_pDrawArgsUnorderedAccessView.Get();
}
