#include "particle_noise.h"
#include "particle_common.h"
#include "particle_emitter_common.h"

cbuffer ConeBuffer : register(b3) 
{
  float4x4 coneRotationMatrix;
  float coneAngle;
  float coneHeight;
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

        // NOTE: Get random angle within cone, create rotation matrices (not uniform)
        float phiAngle = coneAngle * FracRandom(randomHash.xy);
        float thetaAngle = TWO_PI * SimplexNoise(randomHash.xyz);
        float cosPhiAngle = cos(phiAngle);
        float sinPhiAngle = sin(phiAngle);
        float cosThetaAngle = cos(thetaAngle);
        float sinThetaAngle = sin(thetaAngle);

        float4x4 xAxisRotation = 
        {
          float4(1, 0, 0, 0),
          float4(0, cosPhiAngle, sinPhiAngle, 0),
          float4(0, -sinPhiAngle, cosPhiAngle, 0),
          float4(0, 0, 0, 1)
        };

        float4x4 yAxisRotation = 
        {
          float4(cosThetaAngle, 0, -sinThetaAngle, 0),
          float4(0, 1, 0, 0),
          float4(sinThetaAngle, 0, cosThetaAngle, 0),
          float4(0, 0, 0, 1)
        };

        // NOTE: Rotate the cone direction with the rotation matrices to get a new direction within the cone
        float4 direction = float4(0, 1, 0, 0);
        direction = mul(xAxisRotation, direction);
        direction = mul(yAxisRotation, direction);

        // NOTE: The length of this new cone direction vector
        float height = coneHeight * FracRandom(randomHash.xz);

        // NOTE: Initialize the values or reset them for and individual particle
        particleBuffer[particleIndex].pPosition = pePosition + mul(coneRotationMatrix, float4(height * direction.xyz, 1));
       	particleBuffer[particleIndex].pVelocity = peVelocity + mul(coneRotationMatrix, peSpeed * direction).xyz;
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
