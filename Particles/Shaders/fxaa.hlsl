#define FXAA_REDUCE_MIN (1.0 / 256.0)
#define FXAA_REDUCE_MUL (1.0 / 8.0)
#define FXAA_SPAN_MAX 4.0

float3 FXAA(Texture2D t, SamplerState s, float2 texelSize, float2 uv)
{
	const float3 luma = float3(0.299, 0.587, 0.114);

	float3 rgbNW = t.Sample(s, uv + float2(-texelSize.x, -texelSize.y)).rgb;
	float3 rgbNE = t.Sample(s, uv + float2(texelSize.x, -texelSize.y)).rgb;
	float3 rgbSW = t.Sample(s, uv + float2(-texelSize.x, texelSize.y)).rgb;
	float3 rgbSE = t.Sample(s, uv + float2(texelSize.x, texelSize.y)).rgb;
	float3 rgbM = t.Sample(s, uv).rgb;
	float lumaNW = dot(rgbNW, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaM = dot(rgbM, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSE = dot(rgbSE, luma);

	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), min(lumaSW, lumaSE)));

	float2 direction;
	direction.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	direction.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float directionReduce = max((lumaNW + lumaSW + lumaNE + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	float reciprocalDirMin = 1.0 / (min(abs(direction.x), abs(direction.y)), directionReduce);

	direction = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), direction * reciprocalDirMin)) * texelSize;

	// NOTE: Do we need minus 0.5? DX11 samples in pixel middle anyway
	float3 rgbA0 = t.Sample(s, uv + direction * (1.0 / 3.0 - 0.5)).rgb;
	float3 rgbA1 = t.Sample(s, uv + direction * (2.0 / 3.0 - 0.5)).rgb;
	float3 rgbA = 0.5 * (rgbA0 + rgbA1);

	float3 rgbB0 = t.Sample(s, uv + direction * (0.0 / 3.0 - 0.5)).rgb;
	float3 rgbB1 = t.Sample(s, uv + direction * (3.0 / 3.0 - 0.5)).rgb;
	float3 rgbB = rgbA * 0.5 + 0.25 * (rgbB0 + rgbB1);

	float lumaB = dot(rgbB, luma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax)) 
	{
		return rgbA;
	}

	return rgbB;
}

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

Texture2D texture0 : register(t0);
SamplerState textureSampler : register(s0);

float4 PSEntry(PixelIn input) : SV_TARGET
{	
	const float2 TEXEL_SIZE = float2(1.0 / 1920.0, 1.0 / 1080.0);
	return float4(FXAA(texture0, textureSampler, TEXEL_SIZE, input.pixelUV), 1);
}