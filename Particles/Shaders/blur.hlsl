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

cbuffer BlurData : register(b0) 
{
	float2 direction;
	float2 size;
};

PixelIn VSEntry(VertexIn input)
{
	PixelIn output;
	output.pixelPosition = input.vertexPosition;
	output.pixelUV = input.vertexUV;
	return output;
}

Texture2D thresholdTexture : register(t0);
SamplerState textureSampler : register(s0);

float4 PSEntry(PixelIn input) : SV_TARGET
{
	float4 sum = float4(0, 0, 0, 0);
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x - 4.0 * size.x * direction.x, input.pixelUV.y - 4.0 * size.y * direction.y)) * 0.0162162162;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x - 3.0 * size.x * direction.x, input.pixelUV.y - 3.0 * size.y * direction.y)) * 0.0540540541;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x - 2.0 * size.x * direction.x, input.pixelUV.y - 2.0 * size.y * direction.y)) * 0.1216216216;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x - size.x * direction.x, input.pixelUV.y - 1.0 * size.y * direction.y)) * 0.1945945946;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x, input.pixelUV.y)) * 0.2270270270;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x + size.x * direction.x, input.pixelUV.y + 1.0 * size.y * direction.y)) * 0.1945945946;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x + 2.0 * size.x * direction.x, input.pixelUV.y + 2.0 * size.y * direction.y)) * 0.1216216216;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x + 3.0 * size.x * direction.x, input.pixelUV.y + 3.0 * size.y * direction.y)) * 0.0540540541;
    sum += thresholdTexture.Sample(textureSampler, float2(input.pixelUV.x + 4.0 * size.x * direction.x, input.pixelUV.y + 4.0 * size.y * direction.y)) * 0.0162162162;
    return float4(sum.rgb, 1);
}