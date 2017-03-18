#include <Precompiled.h>

#include "D3DShader.h"

using namespace Direct3D;
using namespace Direct3DResource;

Direct3DResource::D3DShader::D3DShader()
{

}

Direct3DResource::D3DShader::~D3DShader()
{

}

bool Direct3DResource::D3DShader::compile(const D3DDevice &device, const std::string &fileName, uint32_t shaderFlags)
{
	m_fileName = fileName;

	if (shaderFlags & SHADER_TYPE_VERTEX)
	{
		if (!compile_vertex_shader(ShaderModel::VS_5_0, device)) 
		{
			return false;
		}
	}

	if (shaderFlags & SHADER_TYPE_PIXEL) 
	{
		if (!compile_pixel_shader(ShaderModel::PS_5_0, device)) 
		{
			return false;
		}
	}

	if (shaderFlags & SHADER_TYPE_COMPUTE) 
	{
		if (!compile_compute_shader(ShaderModel::CS_5_0, device)) 
		{
			return false;
		}
	}

	if (shaderFlags & SHADER_TYPE_HULL) 
	{
		if (!compile_hull_shader(ShaderModel::HS_5_0, device)) 
		{
			return false;
		}
	}

	if (shaderFlags & SHADER_TYPE_DOMAIN) 
	{
		if (!compile_domain_shader(ShaderModel::DS_5_0, device)) 
		{
			return false;
		}
	}

	return true;
}

void Direct3DResource::D3DShader::bind_shader(const D3DDevice & device, ShaderType type) const
{
	ID3D11DeviceContext * context = device.get_context();

	switch (type)
	{
		default: 

		case SHADER_TYPE_VERTEX:
		{
			context->VSSetShader(m_shaderPrograms.vertex.Get(), 0, 0);
		} 
		break;

		case SHADER_TYPE_PIXEL:
		{
			context->PSSetShader(m_shaderPrograms.pixel.Get(), 0, 0);
		}
		break;

		case SHADER_TYPE_COMPUTE:
		{
			context->CSSetShader(m_shaderPrograms.compute.Get(), 0, 0);
		}
		break;

		case SHADER_TYPE_GEOMETRY:
		{
			context->GSSetShader(m_shaderPrograms.geometry.Get(), 0, 0);
		}
		break;

		case SHADER_TYPE_HULL:
		{
			context->HSSetShader(m_shaderPrograms.hull.Get(), 0, 0);
		}
		break;

		case SHADER_TYPE_DOMAIN:
		{
			context->DSSetShader(m_shaderPrograms.domain.Get(), 0, 0);
		}
		break;
	}
}

void Direct3DResource::D3DShader::unbind_shader(const D3DDevice & device, ShaderType type) const
{
	ID3D11DeviceContext * context = device.get_context();

	switch (type)
	{
		default:

		case SHADER_TYPE_VERTEX:
		{
			context->VSSetShader(0, 0, 0);
		}
		break;

		case SHADER_TYPE_PIXEL:
		{
			context->PSSetShader(0, 0, 0);
		}
		break;

		case SHADER_TYPE_COMPUTE:
		{
			context->CSSetShader(0, 0, 0);
		}
		break;

		case SHADER_TYPE_GEOMETRY:
		{
			context->GSSetShader(0, 0, 0);
		}
		break;

		case SHADER_TYPE_HULL:
		{
			context->HSSetShader(0, 0, 0);
		}
		break;

		case SHADER_TYPE_DOMAIN:
		{
			context->DSSetShader(0, 0, 0);
		}
		break;
	}
}

ID3DBlob * Direct3DResource::D3DShader::get_data(ShaderType type) const
{
	ID3DBlob * data = 0;

	switch (type)
	{
		default:

		case SHADER_TYPE_VERTEX:
		{
			data = m_shaderData.vertex.Get();
		}
		break;

		case SHADER_TYPE_PIXEL:
		{
			data = m_shaderData.pixel.Get();
		}
		break;

		case SHADER_TYPE_COMPUTE:
		{
			data = m_shaderData.compute.Get();
		}
		break;

		case SHADER_TYPE_GEOMETRY:
		{
			data = m_shaderData.geometry.Get();
		}
		break;

		case SHADER_TYPE_HULL:
		{
			data = m_shaderData.hull.Get();
		}
		break;

		case SHADER_TYPE_DOMAIN:
		{
			data = m_shaderData.domain.Get();
		}
		break;
	}

	return data;
}

bool Direct3DResource::D3DShader::compile_vertex_shader(ShaderModel model, const D3DDevice &device)
{
	ComPtr<ID3DBlob> error;
	std::wstring fileName = string_to_wchar_t(m_fileName);
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		ENTRY_POINT_VS, SHADER_MODEL[model], 0, 0, m_shaderData.vertex.GetAddressOf(), error.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to compile vertex shader.", 0, 0);
		output_shader_error(error.Get());
		return false;
	}

	hr = device.get_device()->CreateVertexShader(m_shaderData.vertex.Get()->GetBufferPointer(), 
		m_shaderData.vertex.Get()->GetBufferSize(), 0, m_shaderPrograms.vertex.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create vertex shader.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DShader::compile_pixel_shader(ShaderModel model, const D3DDevice &device)
{
	ComPtr<ID3DBlob> error;
	std::wstring fileName = string_to_wchar_t(m_fileName);
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		ENTRY_POINT_PS, SHADER_MODEL[model], 0, 0, m_shaderData.pixel.GetAddressOf(), error.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to compile pixel shader.", 0, 0);
		output_shader_error(error.Get());
		return false;
	}

	hr = device.get_device()->CreatePixelShader(m_shaderData.pixel.Get()->GetBufferPointer(), 
		m_shaderData.pixel.Get()->GetBufferSize(), 0, m_shaderPrograms.pixel.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create pixel shader.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DShader::compile_compute_shader(ShaderModel model, const D3DDevice &device)
{
	ComPtr<ID3DBlob> error;
	std::wstring fileName = string_to_wchar_t(m_fileName);
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		ENTRY_POINT_CS, SHADER_MODEL[model], 0, 0, m_shaderData.compute.GetAddressOf(), error.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to compile compute shader.", 0, 0);
		output_shader_error(error.Get());
		return false;
	}

	hr = device.get_device()->CreateComputeShader(m_shaderData.compute.Get()->GetBufferPointer(),
		m_shaderData.compute.Get()->GetBufferSize(), 0, m_shaderPrograms.compute.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create compute shader.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DShader::compile_geometry_shader(ShaderModel model, const D3DDevice &device)
{
	ComPtr<ID3DBlob> error;
	std::wstring fileName = string_to_wchar_t(m_fileName);
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		ENTRY_POINT_CS, SHADER_MODEL[model], 0, 0, m_shaderData.compute.GetAddressOf(), error.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to compile geometry shader.", 0, 0);
		output_shader_error(error.Get());
		return false;
	}
	
	hr = device.get_device()->CreateGeometryShader(m_shaderData.geometry.Get()->GetBufferPointer(), 
		m_shaderData.geometry.Get()->GetBufferSize(), 0, m_shaderPrograms.geometry.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create geometry shader.", 0, 0);
		return false;
	}
	
	return true;
}

bool Direct3DResource::D3DShader::compile_hull_shader(ShaderModel model, const D3DDevice &device)
{
	ComPtr<ID3DBlob> error;
	std::wstring fileName = string_to_wchar_t(m_fileName);
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		ENTRY_POINT_HS, SHADER_MODEL[model], 0, 0, m_shaderData.hull.GetAddressOf(), error.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to compile hull shader.", 0, 0);
		output_shader_error(error.Get());
		return false;
	}

	hr = device.get_device()->CreateHullShader(m_shaderData.hull.Get()->GetBufferPointer(),
		m_shaderData.hull.Get()->GetBufferSize(), 0, m_shaderPrograms.hull.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create hull shader.", 0, 0);
		return false;
	}

	return true;
}

bool Direct3DResource::D3DShader::compile_domain_shader(ShaderModel model, const D3DDevice &device)
{
	ComPtr<ID3DBlob> error;
	std::wstring fileName = string_to_wchar_t(m_fileName);
	HRESULT hr = D3DCompileFromFile(fileName.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, 
		ENTRY_POINT_DS, SHADER_MODEL[model], 0, 0, m_shaderData.domain.GetAddressOf(), error.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to compile domain shader.", 0, 0);
		output_shader_error(error.Get());
		return false;
	}

	hr = device.get_device()->CreateDomainShader(m_shaderData.domain.Get()->GetBufferPointer(),
		m_shaderData.domain.Get()->GetBufferSize(), 0, m_shaderPrograms.domain.GetAddressOf());
	if (FAILED(hr))
	{
		MessageBoxA(0, "Failed to create domain shader.", 0, 0);
		return false;
	}

	return true;
}

void Direct3DResource::D3DShader::output_shader_error(ID3DBlob * error)
{
	char * compileErrors = (char*)(error->GetBufferPointer());
	unsigned long long buffer_size = error->GetBufferSize();
	
	std::ofstream fout(ERROR_FILE_OUTPUT);
	for (unsigned long long i = 0; i < buffer_size; i++) 
	{
		fout << compileErrors[i];
	}

	fout.close();
	MessageBoxA(0, "Error compiling shader. Read shader_error.txt for message.", m_fileName.c_str(), MB_OK);
}

std::wstring Direct3DResource::D3DShader::string_to_wchar_t(const std::string & str)
{
	std::wstring temp(str.length(), L' ');
	std::copy(str.begin(), str.end(), temp.begin());
	return temp;
}
