#include "particle_noise.h"
#include "particle_common.h"

cbuffer TimeBuffer : register(b0) 
{
	float4 timings;
};

cbuffer CameraBuffer : register(b1) 
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

cbuffer EmitterBuffers : register(b2)
{
	GlobalParticleEmitterData emitters[8];
};

cbuffer CounterBuffer : register(b3) 
{
	uint pDeadCount;
	uint pMaxCount;
	uint pRenderCount;
};

AppendStructuredBuffer<uint> particleDeadList : register(u0);
RWStructuredBuffer<Particle> particleBuffer : register(u1);
RWStructuredBuffer<uint> particleRenderList : register(u2);
RWBuffer<uint> particleDrawArgsBuffer : register(u3);

Texture2D depthTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState textureSampler: register(s0);

[numthreads(128, 1, 1)]
void CSEntry(uint3 id : SV_DispatchThreadID) 
{

	// NOTE: Reset draw indirect arguments buffer
	if (id.x == 0) 
	{
		particleDrawArgsBuffer[0] = 0;
		particleDrawArgsBuffer[1] = 1;
		particleDrawArgsBuffer[2] = 0;
		particleDrawArgsBuffer[3] = 0;
		particleDrawArgsBuffer[4] = 0;
	}

	AllMemoryBarrierWithGroupSync();

	Particle particle = particleBuffer[id.x];
	particle.pPainting = 0.0;
	if (particle.pAge > 0.0) 
	{
		// NOTE: Get particle depth and screen position
		float4 pViewPosition = mul(viewMatrix, float4(particle.pPosition, 1)); 
		float pViewDepth = pViewPosition.z;
		float4 pProjectedPosition = mul(projectionMatrix, pViewPosition);
		float2 pNormalizedPosition = pProjectedPosition.xy / pProjectedPosition.w;

		GlobalParticleEmitterData emitter = emitters[particle.pGemitterIndex];
		// NOTE: If particle is within screen space, check for collisions
		if (pNormalizedPosition.x > -1 && pNormalizedPosition.y > -1 && pNormalizedPosition.x < 1 && pNormalizedPosition.y < 1) 
		{ 
			// NOTE: Scale and bias NDC space position so it can be used for texture lookup
			pNormalizedPosition = 0.5 + pNormalizedPosition * 0.5; 
			pNormalizedPosition.y = 1.0 - pNormalizedPosition.y;

			// NOTE: Need a thickness value since we only have surface information, how thick is the surface?
			float sceneViewDepth = EyeDepth(depthTexture.SampleLevel(textureSampler, pNormalizedPosition, 0).r);
			float sceneThickness = 0.05; 
			if ((sceneViewDepth < pViewDepth) && (pViewDepth < sceneViewDepth + sceneThickness) && emitter.pgeCollisionsEnabled > 0) 
			{
				// NOTE: Handle particle collisions
				float3 sceneNormal = normalTexture.SampleLevel(textureSampler, pNormalizedPosition, 0).xyz; 
				float3 pVelocity = particle.pVelocity;
				float3 pVelocityNormal = dot(pVelocity, sceneNormal) * sceneNormal;
				float3 pVelocityTangent = pVelocity - pVelocityNormal;

				particle.pVelocity = emitter.pgeCollisionDampening * (pVelocityTangent - emitter.pgeRestitutionFactor * pVelocityNormal);
				particle.pPainting = emitter.pgePaintingEnabled;

				particle.pCollisions.x++;
				if (emitter.pgeSleepingEnabled && particle.pCollisions > emitter.pgeSleepingCollisionFrames)
				{
					particle.pSleeping = 1.0;
				}
			}

			// NOTE: Should be culled in the rendering step due to being behind and object
			particle.pCulling = (pViewDepth > (sceneViewDepth + sceneThickness * 1.1)) ? 1.0 : 0.0; 
		}

		// NOTE: Propagate the particle through space
		if (!particle.pSleeping)
		{
			particle.pVelocity += particle.pAcceleration * timings.x; 
			particle.pPosition += 0.5 * particle.pVelocity * timings.x;
		}
		
		particle.pAge -= timings.x;
		particleBuffer[id.x] = particle;

		// NOTE: Choose where to put the particle, is it dead or should it be sorted for rendering?
		if (particle.pAge <= 0.0) 
		{ 
			particleBuffer[id.x].pAge = -1;
			particleDeadList.Append(id.x);
		} 
		else 
		{
			particleRenderList[particleRenderList.IncrementCounter()] = id.x;
			InterlockedAdd(particleDrawArgsBuffer[0], 6);
		}
	}
}
