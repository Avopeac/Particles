#include "particle_common.h"

cbuffer CameraBuffer : register(b0) 
{
	float3 camera;
	float4x4 normalMatrix;
  	float4x4 viewMatrix;
  	float4x4 projectionMatrix;
  	float4x4 viewProjectionMatrix;
  	float4x4 previousViewProjectionMatrix;
};

cbuffer EmitterBuffers : register(b1) 
{
	GlobalParticleEmitterData emitters[8];
};

cbuffer TextureData : register(b2) 
{
	float4 renderTargetSize;
};

struct PixelIn 
{
	float4 pixelPosition : SV_POSITION;
	float2 pixelUV : TEXCOORD0;
	float4 pixelColor : TEXCOORD1;
	float pixelPaint : TEXCOORD2;
	uint pixelTextureIndex : TEXCOORD3;
	nointerpolation float pixelViewDepth : TEXCOORD4;
	float3 pixelPaintColor : TEXCOORD5;
};

struct PixelOut 
{
	float4 targetScreenSpace : SV_TARGET0;
	float4 targetColor : SV_TARGET1;
};

// NOTE: Vertex shader SRVs
StructuredBuffer<Particle> particleBuffer : register(t0); 
StructuredBuffer<uint> particleRenderList : register(t1);

// NOTE: Pixel shader SRVs
Texture2DArray particleTextures : register(t0); 
Texture2D sceneDepthTexture : register(t1);
SamplerState textureSampler : register(s0);

PixelIn VSEntry(uint id : SV_VertexID) 
{
	// NOTE: Every quad has 6 vertices
	uint index = id / 6;
	uint vertex = id % 6;
	Particle particle = particleBuffer[particleRenderList[index]];
	PixelIn input = (PixelIn)0;

	// NOTE: Return right away if particle is behind object, set in the update shader
	if (particle.pCulling == 1) 
	{
		return input; 
	}

	// NOTE: Get values over time
	float age = particle.pAge;
	float lifetime = particle.pLifetime;
	float interpolate = max(0.0, 1.0 - age / lifetime);

	GlobalParticleEmitterData currentEmitter = emitters[particle.pGemitterIndex];

	float2 size = SplineInterpolate(currentEmitter.pgeSizePoint0,
		currentEmitter.pgeSizePoint1,
		currentEmitter.pgeSizePoint2,
		currentEmitter.pgeSizePoint3, 
		interpolate);

	float4 color = SplineInterpolate(currentEmitter.pgeColorPoint0,
		currentEmitter.pgeColorPoint1, 
		currentEmitter.pgeColorPoint2, 
		currentEmitter.pgeColorPoint3, 
		interpolate);

	float2 rotation = SplineInterpolate(currentEmitter.pgeRotationPoints.x, 
		currentEmitter.pgeRotationPoints.y, 
		currentEmitter.pgeRotationPoints.z,
		currentEmitter.pgeRotationPoints.w,
		interpolate);

	float sinA = sin(rotation.x);
	float cosA = cos(rotation.x);
	const float2x2 ROTATION_MAT = 
	{
		cosA, -sinA,
		sinA, cosA
	};

	// NOTE: Build a orthonormal base for the billboard
	float4 worldPosition = float4(particle.pPosition.xyz, 1);
	float3 look = normalize(camera - worldPosition.xyz);
	float3 normal = normalize(viewMatrix[1].xyz);
	float3 tangent = normalize(cross(normal, look));
	float3 bitangent = normalize(cross(look, tangent));

	float3 stretch = 0;
	if (currentEmitter.pgeStretchingEnabled)
	{
		stretch = mul(viewMatrix, float4(particle.pVelocity, 0)).xyz;
		stretch = currentEmitter.pgeStretchFactor * abs(stretch);
	}
	
	const float4 OFFSETS[6] =
	{
		float4(-1 - stretch.x, -1 - stretch.y, -1, 1), // NOTE: Bottom left, texture coordinates are upside down
		float4(1 + stretch.x, -1 - stretch.y, 1, 1), // NOTE: Bottom right, texture coordinates are upside down
		float4(-1 - stretch.x, 1 + stretch.y, -1, -1), // NOTE: Top left, texture coordinates are upside down
		float4(-1 - stretch.x, 1 + stretch.y, -1, -1), // NOTE: Top left, texture coordinates are upside down
		float4(1 + stretch.x, -1 - stretch.y, 1, 1), // NOTE: Bottom right, texture coordinates are upside down
		float4(1 + stretch.x, 1 + stretch.y, 1, -1) // NOTE: Top right, texture coordinates are upside down
	};

    worldPosition.xyz += tangent * size.x * OFFSETS[vertex].x + bitangent * size.y * OFFSETS[vertex].y;
    worldPosition = mul(viewMatrix, worldPosition);
	input.pixelUV = 0.5 * mul(ROTATION_MAT, OFFSETS[vertex].zw) + 0.5;
	// NOTE: Texture index of the texture array supplied by the particle system
	input.pixelTextureIndex = particle.pTextureIndex;
	// NOTE: Convert to clip space
	input.pixelPaint = particle.pPainting;
	input.pixelPosition = mul(projectionMatrix, worldPosition);
	input.pixelViewDepth = worldPosition.z;
	input.pixelColor = color;
	input.pixelPaintColor = currentEmitter.pgePaintColor.rgb;

	return input;
}

PixelOut PSEntry(PixelIn input) : SV_TARGET
{
	float3 arrayLookup = float3(input.pixelUV, input.pixelTextureIndex);
	float4 color = particleTextures.Sample(textureSampler, arrayLookup);
	color *= input.pixelColor;

	PixelOut o;
	o.targetColor = color;
	o.targetScreenSpace = float4(0, 0, 0, 0);

	if (input.pixelPaint > 0.5) 
	{
		// NOTE: Scale viewport coordinates to UV coordinates
		//float2 screenSpacePosition = input.pixelPosition.xy * renderTargetSize.zw;
		//o.targetScreenSpace = float4(input.pixelPaintColor, input.pixelViewDepth);
		o.targetScreenSpace = float4(input.pixelPaintColor, input.pixelViewDepth);
	}

	return o;
}
