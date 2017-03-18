struct VertexIn 
{
	float4 vertexPosition : POSITION;
	float2 vertexUV : TEXCOORD0;
};

struct PixelIn 
{
	float4 pixelPosition : SV_POSITION;
	float2 pixelUV : TEXCOORD0;
};

cbuffer BlendFactorData : register(b0) 
{
	float2 factor;
	float exposure;
};

PixelIn VSEntry(VertexIn input)
{
	PixelIn output;
	output.pixelPosition = input.vertexPosition;
	output.pixelUV = input.vertexUV;
	return output;
}

Texture2D blendTexture0 : register(t0);
Texture2D blendTexture1 : register(t1);
SamplerState textureSampler : register(s0);

float4 PSEntry(PixelIn input) : SV_TARGET
{
	const float GAMMA = 1.0 / 2.2;
	float4 color0 = factor.x * blendTexture0.Sample(textureSampler, input.pixelUV);
	float4 color1 = factor.y * blendTexture1.Sample(textureSampler, input.pixelUV);
	float4 result = float4(color0.rgb + color1.rgb, 1.0);
	result.rgb = float3(1.0, 1.0, 1.0) - exp(-result.rgb * float3(exposure, exposure, exposure));
	result.rgb = pow(result.rgb, float3(GAMMA,GAMMA,GAMMA));
	return result;
}