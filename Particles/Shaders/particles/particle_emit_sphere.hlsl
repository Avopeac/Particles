#include "particle_noise.h"
#include "particle_common.h"
#include "particle_emitter_common.h"

cbuffer SphereParticleEmitterData : register(b3) 
{
	float sphereRadius;
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
    	float3 randomHash = (1.0 + id.x) * float3(elapsedTime % 1024, elapsedTime / 1024, elapsedTime);
    	float3 unitSpherePosition = float3(SimplexNoise(randomHash.xyz), SimplexNoise(randomHash.xzy), SimplexNoise(randomHash.yzx));

    	// NOTE: Get radius for either shell or volume emission
    	float radius = (peVolume == 0.0) ? min(sphereRadius * (0.5 + 0.5 * unitSpherePosition.x), sphereRadius) : sphereRadius;
		float3 spherePosition = radius * normalize(unitSpherePosition);
		
		// NOTE: Initialize the values or reset them for and individual particle
		particleBuffer[particleIndex].pPosition = spherePosition + pePosition; 
		particleBuffer[particleIndex].pVelocity = peVelocity + peSpeed * unitSpherePosition;
		particleBuffer[particleIndex].pAcceleration = peAcceleration;
		particleBuffer[particleIndex].pLifetime = peLifetime;
    	particleBuffer[particleIndex].pAge = peLifetime;
		particleBuffer[particleIndex].pTextureIndex = peTextureIndex;
    	particleBuffer[particleIndex].pGemitterIndex = peGemitterIndex;
		particleBuffer[particleIndex].pCollisions = 0;
		particleBuffer[particleIndex].pSleeping = 0;
    	particleBuffer[particleIndex].pCulling = 0;
    	particleBuffer[particleIndex].pPainting = 0;
	}
}
