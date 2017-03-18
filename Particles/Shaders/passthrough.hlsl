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

PixelIn VSEntry(VertexIn input)
{
	PixelIn output;
	output.pixelPosition = input.vertexPosition;
	output.pixelUV = input.vertexUV;
	return output;
}

Texture2D colorTexture : register(t0);
SamplerState textureSampler : register(s0);

float4 PSEntry(PixelIn input) : SV_TARGET
{
	return colorTexture.Sample(textureSampler, input.pixelUV);
}