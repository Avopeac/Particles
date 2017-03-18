#pragma once

#include "D3DDevice.h"

namespace Direct3DResource 
{
	using namespace Direct3D;

	static const char * ENTRY_POINT_VS = "VSEntry";
	static const char * ENTRY_POINT_PS = "PSEntry";
	static const char * ENTRY_POINT_CS = "CSEntry";
	static const char * ENTRY_POINT_GS = "GSEntry";
	static const char * ENTRY_POINT_HS = "HSEntry";
	static const char * ENTRY_POINT_DS = "DSEntry";

	static const uint32_t ERROR_FILE_SIZE = 128;
	static const char * ERROR_FILE_OUTPUT = "shader_error.txt";
	
	enum ShaderModel 
	{
		VS_5_0,
		PS_5_0,
		CS_5_0,
		GS_5_0,
		HS_5_0,
		DS_5_0,
	};

	enum ShaderType 
	{
		SHADER_TYPE_PIXEL = 1,
		SHADER_TYPE_VERTEX = 2,
		SHADER_TYPE_COMPUTE = 4,
		SHADER_TYPE_GEOMETRY = 8,
		SHADER_TYPE_HULL = 16,
		SHADER_TYPE_DOMAIN = 32,
	};

	struct ShaderPrograms
	{
		ComPtr<ID3D11VertexShader> vertex;
		ComPtr<ID3D11PixelShader> pixel;
		ComPtr<ID3D11ComputeShader> compute;
		ComPtr<ID3D11GeometryShader> geometry;
		ComPtr<ID3D11HullShader> hull;
		ComPtr<ID3D11DomainShader> domain;
	};

	struct ShaderData 
	{
		ComPtr<ID3DBlob> vertex;
		ComPtr<ID3DBlob> pixel;
		ComPtr<ID3DBlob> compute;
		ComPtr<ID3DBlob> geometry;
		ComPtr<ID3DBlob> hull;
		ComPtr<ID3DBlob> domain;
	};

	static const char * SHADER_MODEL[6] =
	{
		"vs_5_0",
		"ps_5_0",
		"cs_5_0",
		"gs_5_0",
		"hs_5_0",
		"ds_5_0",
	};

	class D3DShader 
	{
		std::string m_fileName;
		ShaderPrograms m_shaderPrograms;
		ShaderData m_shaderData;

	public:
		D3DShader();
		~D3DShader();
		D3DShader(const D3DShader &) = delete;
		D3DShader(D3DShader &&) = delete;
		D3DShader &operator=(const D3DShader &) = delete;
		D3DShader &operator=(D3DShader &&) = delete;

		// NOTE: Compile shaders with the given flags
		bool compile(const D3DDevice &device, const std::string &fileName, uint32_t shaderFlags);

		// NOTE: Bind the shader of specified type
		void bind_shader(const D3DDevice &device, ShaderType type) const;

		// NOTE: Unbind shader type
		void unbind_shader(const D3DDevice &device, ShaderType type) const;

		// NOTE: Get the shader data
		ID3DBlob * get_data(ShaderType type) const;

	private:
		bool compile_vertex_shader(ShaderModel model, const D3DDevice &device);
		bool compile_pixel_shader(ShaderModel model, const D3DDevice &device);
		bool compile_compute_shader(ShaderModel model, const D3DDevice &device);
		bool compile_geometry_shader(ShaderModel model, const D3DDevice &device);
		bool compile_hull_shader(ShaderModel model, const D3DDevice &device);
		bool compile_domain_shader(ShaderModel model, const D3DDevice &device);
		void output_shader_error(ID3DBlob * error_message);

		// NOTE: Taken from http://stackoverflow.com/questions/29227573/how-to-convert-cstring-to-wchar-t 2016-11-01 (YYYY-MM-DD)
		std::wstring string_to_wchar_t(const std::string &str);
	};
}
