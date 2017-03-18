#include "noise.h"

float4 GetTexel(Texture2D t, SamplerState s, float2 uv, float2 size)
{
	float2 p = uv * size;
	float2 i = floor(p);
	float2 f = p - i;
	f = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);
	p = i + f;
	p = p / size;

	float2 isize = 1.0 / size;
	float4 color = t.Sample(s, p);
	color += t.Sample(s, p + float2(1, 1) * isize);
	color += t.Sample(s, p + float2(-1, 1) * isize);
	color += t.Sample(s, p + float2(1, -1) * isize);
	color += t.Sample(s, p + float2(-1, -1) * isize);
	return color * 0.2; 
}

float4 BicubicFilter(Texture2D t, SamplerState s, float2 uv, float2 size, float2 invSize)
{
	float2 texelPosition = uv * size;
	float2 texelCenter = floor(texelPosition - 0.5) + 0.5;
	
	float2 offset = texelPosition - texelCenter;
	float2 offset2 = offset * offset;
	float2 offset3 = offset * offset2;

	float2 invOffset = 1 - offset;
	float2 invOffset2 = invOffset * invOffset;
	float2 invOffset3 = invOffset2 * invOffset;

	float4 w0w1 = 0.166667 * float4(invOffset3, 4 + 3 * offset3 - 6 * offset2);
	float4 w2w3 = 0.166667 * float4(4 + 3 * invOffset3 - 6 * invOffset2, offset3);
	float4 s0s1 = float4(w0w1.xy + w0w1.zw, w2w3.xy + w2w3.zw);
	float4 t0t1 = invSize.xyxy * float4(texelCenter - 1 + (w0w1.zw / (w0w1.xy + w0w1.zw)), texelCenter + 1 + (w2w3.zw / (w2w3.xy + w2w3.zw)));

	return t.Sample(s, float2(t0t1.x, t0t1.y)) * s0s1.x * s0s1.y +
		t.Sample(s, float2(t0t1.z, t0t1.y)) * s0s1.z * s0s1.y +
		t.Sample(s, float2(t0t1.x, t0t1.w)) * s0s1.x * s0s1.w +
		t.Sample(s, float2(t0t1.z, t0t1.w)) * s0s1.z * s0s1.w;
}

float Overlay(float v0, float v1)
{
	return v0 < 0.5 ? 2.0 * v0 * v1 : 1.0 - 2.0 * (1.0 - v0) * (1.0 - v1);	
}

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
	float3 pixelNormal : TEXCOORD0;
	float3 pixelTangent : TEXCOORD1;
	float3 pixelBitangent : TEXCOORD2;
	float2 pixelUV : TEXCOORD3;
  	float3 pixelColor : COLOR;
};

struct PixelOut
{
	float3 targetDiffuse : SV_Target0;
	float4 targetViewNormalSpecular : SV_Target1;
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

cbuffer MeshData : register(b1)
{
	float4x4 meshWorldTransform;
	float4 uvNormalizeTransform;
	float4 paintTextureSize;
	float4 materialTextures;
	float3 materialDiffuse;
	float materialExponent;
};

PixelIn VSEntry(VertexIn input)
{
	PixelIn output;
	output.pixelPosition = mul(viewProjectionMatrix, mul(meshWorldTransform, float4(input.vertexPosition, 1)));
	output.pixelNormal = input.vertexNormal;
	output.pixelTangent = input.vertexTangent;
	output.pixelBitangent = input.vertexBitangent;
	output.pixelColor = input.vertexColor;
	output.pixelUV = input.vertexUV;
	return output;
}

// NOTE: Mesh texture inputs
Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D maskTexture : register(t3);
Texture2D paintTexture : register(t4);
Texture2D noiseTexture : register(t5);
SamplerState textureSampler : register(s0);

PixelOut PSEntry(PixelIn input) : SV_TARGET
{
	const float2 UV = float2(input.pixelUV.x, 1.0 - input.pixelUV.y);
	const float NORMAL_STRENGTH = 0.1;
	const float3x3 TBN_MATRIX = 
	{
		normalize(input.pixelTangent),
		normalize(input.pixelBitangent),
		normalize(input.pixelNormal)
	};

	if (materialTextures.x && !maskTexture.Sample(textureSampler, UV).r) 
	{
		discard; 
	}
	
	float2 uv = (uvNormalizeTransform.zw + input.pixelUV) / uvNormalizeTransform.xy;
	uv.y = 1.0 - uv.y;

	float3 surfaceColor = diffuseTexture.Sample(textureSampler, UV).rgb * input.pixelColor.xyz; 
	float3 surfaceNormal = normalize(mul(TBN_MATRIX, 2.0 * normalTexture.Sample(textureSampler, UV).xyz - 1.0)); 
	float surfaceSpecularity = 90.0 * specularTexture.Sample(textureSampler, UV).r + materialExponent; 
	surfaceNormal = normalize(input.pixelNormal - NORMAL_STRENGTH * surfaceNormal);
	surfaceNormal = normalize(mul((float3x3)viewMatrix, surfaceNormal));

	// NOTE: Smear the paint color, the conditional is a large optimization
	if (materialTextures.y) 
	{
		float noiseColor = noiseTexture.Sample(textureSampler, 5.0 * uv).r;
		noiseColor += noiseTexture.Sample(textureSampler, 25.0 * uv).r;
		float4 paintColor = BicubicFilter(paintTexture, textureSampler, uv, paintTextureSize.xy, paintTextureSize.zw);
		paintColor.a = Overlay(paintColor.a, noiseColor);
		surfaceColor = surfaceColor * (1.0 - paintColor.a) + paintColor.rgb * paintColor.a;
	}

	// NOTE: Fill G-buffer targets
	PixelOut targetOutput;
  	targetOutput.targetDiffuse = surfaceColor;
	targetOutput.targetViewNormalSpecular = float4(surfaceNormal, surfaceSpecularity / 255.0);
	return targetOutput;
}
