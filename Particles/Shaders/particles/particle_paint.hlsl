#include "../particles/particle_common.h"

struct VertexIn
{
	float3 vertexPosition : POSITION;
	float3 vertexNormal : TEXCOORD0;
	float3 vertexTangent : TEXCOORD1;
	float3 vertexBitangent : TEXCOORD2;
	float3 vertexColor : COLOR;
	float2 vertexUV : TEXCOORD3;
};

struct PixelIn
{
	float4 pixelPosition : SV_POSITION;
	float4 pixelUV : TEXCOORD0;
	float pixelViewDepth : TEXCOORD1;
};

cbuffer CameraData : register(b0)
{
	float3 camera;
	float4x4 normalMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 viewProjectionMatrix;
	float4x4 previousViewProjectionMatrix;
	float4x4 inverseProjectionMatrix;
	float4x4 inverseViewMatrix;
	float4x4 inverseViewProjectionMatrix;
};

cbuffer TransformData : register(b1)
{
	float4x4 worldTransformMatrix;
};

cbuffer UVTransformData : register(b2)
{
	float4 uvTransform;
};

Texture2D particleTexture : register(t0);
Texture2D paintTexture : register(t1);
SamplerState textureSampler : register(s0);
SamplerState texturePointSampler : register(s1);

PixelIn VSEntry(VertexIn input)
{
	float2 uv = (uvTransform.zw + input.vertexUV) / uvTransform.xy;
	uv = 2.0 * uv - 1.0;

	PixelIn output;
	output.pixelPosition = float4(uv, 0, 1);
	output.pixelUV = mul(viewProjectionMatrix, mul(worldTransformMatrix, float4(input.vertexPosition, 1)));
	output.pixelViewDepth = mul(viewMatrix, mul(worldTransformMatrix, float4(input.vertexPosition, 1))).z;
	return output;
}

float4 PSEntry(PixelIn input) : SV_TARGET
{
	const float invSize = 1.0 / 512.0;
	float2 actualUV = input.pixelPosition.xy * invSize;
	float4 finalColor = paintTexture.Sample(textureSampler, actualUV);
	
	float4 position = input.pixelUV;
	position.xyz /= position.w;
	if (position.x < -1 || position.x > 1 || position.y < -1 || position.y > 1 || position.z < 0)
	{
		return finalColor;
	}

	// TODO: Do viewport transform instead
	position.xy = 0.5 + position.xy * 0.5;
	position.y = 1.0 - position.y;
	position.z = input.pixelViewDepth;

	float4 particleColor = particleTexture.Sample(texturePointSampler, position.xy);
	float dist = abs(input.pixelViewDepth  - particleColor.w);
	if (particleColor.w > 0 && dist < 0.1)
	{
		finalColor.rgb = particleColor.rgb;
		finalColor.a = saturate(finalColor.a + 0.1);
	}

	return finalColor;
}
