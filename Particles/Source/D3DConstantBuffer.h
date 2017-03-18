#pragma once

#include <precompiled.h>

namespace Direct3DResource 
{
	template <typename T> class D3DConstantBuffer 
	{
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pBuffer;

	public:

		D3DConstantBuffer<T>()  
		{

		}

		~D3DConstantBuffer<T>() 
		{

		}

		D3DConstantBuffer(const D3DConstantBuffer<T> &other) 
		{
			assert(this != &other);
			m_pBuffer = other.m_pBuffer;
		}

		D3DConstantBuffer(D3DConstantBuffer<T> &&other)
		{
			assert(this != &other);
			m_pBuffer = other.m_pBuffer;
		}

		D3DConstantBuffer<T> &operator=(const D3DConstantBuffer<T> &other)
		{
			if (this != &other)
			{
				m_pBuffer = other.m_pBuffer;
			}

			return *this;
		}

		D3DConstantBuffer<T> &operator=(D3DConstantBuffer<T> &&other)
		{
			if (this != &other)
			{
				m_pBuffer = other.m_pBuffer;
			}

			return *this;
		}

		inline ID3D11Buffer * get_data() const 
		{
			return m_pBuffer.Get();
		}

		bool create(const Direct3D::D3DDevice &device)
		{
			D3D11_BUFFER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			desc.ByteWidth = sizeof(T);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			desc.Usage = D3D11_USAGE_DYNAMIC;

			HRESULT hr = device.get_device()->CreateBuffer(&desc, 0, m_pBuffer.GetAddressOf());
			if (FAILED(hr))
			{
				MessageBoxA(0, "Failed to create constant buffer.", 0, 0);
				return false;
			}

			return true;
		}

		bool map_data(ID3D11DeviceContext * context, T * type) const
		{
			D3D11_MAPPED_SUBRESOURCE data;
			ZeroMemory(&data, sizeof(data));
			HRESULT hr = context->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
			if (FAILED(hr))
			{
				MessageBoxA(0, "Failed to map data to constant buffer.", 0, 0);
				return false;
			}

			memcpy(data.pData, reinterpret_cast<void *>(type), sizeof(T));
			context->Unmap(m_pBuffer.Get(), 0);
			return true;
		}

	private:
		
	};
}
