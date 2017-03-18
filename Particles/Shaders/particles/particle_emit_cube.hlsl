#include "particle_noise.h"
#include "particle_common.h"
#include "particle_emitter_common.h"

cbuffer CubeParticleEmitterData : register(b3) 
{
  	float4x4 cubeRotationMatrix;
	float3 cubeSideLengths;
};

[numthreads(1024, 1, 1)]
void CSEntry(uint3 id : SV_DispatchThreadID) 
{
  	float elapsedTime = time.y;
	float spawnTime = min(peSpawnTime, frac(elapsedTime / peSpawnTime));

	// NOTE: If particles are available (dead) they are consumed from the dead list
	if (id.x < pDeadCount && id.x < peMaxEmission && spawnTime < 0.1) 
	{ 
		uint particleIndex = particleDeadList.Consume();
    	float3 randomHash = (1 + id.x) * float3(elapsedTime % 1024, elapsedTime / 1024, elapsedTime);
    	float3 positionOffset = float3(SimplexNoise(randomHash.xyz), SimplexNoise(randomHash.yxz), SimplexNoise(randomHash.xzy)) * cubeSideLengths;
		
		// NOTE: Initialize the values or reset them for and individual particle
    	particleBuffer[particleIndex].pPosition = mul(cubeRotationMatrix, float4(pePosition + positionOffset, 1)); 
		particleBuffer[particleIndex].pAcceleration = peAcceleration;
		particleBuffer[particleIndex].pVelocity = peVelocity;
    	particleBuffer[particleIndex].pLifetime = peLifetime;
    	particleBuffer[particleIndex].pAge = peLifetime;
		particleBuffer[particleIndex].pTextureIndex = peTextureIndex;
		particleBuffer[particleIndex].pGemitterIndex = peGemitterIndex;
		particleBuffer[particleIndex].pSleeping = 0;
		particleBuffer[particleIndex].pCollisions = 0;
    	particleBuffer[particleIndex].pCulling = 0;
    	particleBuffer[particleIndex].pPainting = 0;
	}
}
