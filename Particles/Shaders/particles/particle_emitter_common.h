#define TWO_PI 6.28318530718

cbuffer TimeBuffer : register(b0) 
{
  float4 time;
};

cbuffer CounterBuffer : register(b1) 
{
  uint pDeadCount;
  uint pMaxCount;
  uint pRenderCount;
  uint pCollisionCount;
};

cbuffer EmitterBuffer0 : register(b2) 
{
    float3 pePosition;
    float peLifetime;
    float3 peVelocity;
    float peSpawnTime;
    float3 peAcceleration;
    float peMaxEmission;
    float peTextureIndex;
    float peGemitterIndex;
    float peSpeed;
    float peVolume;
}

ConsumeStructuredBuffer<uint> particleDeadList : register(u0);
RWStructuredBuffer<Particle> particleBuffer : register(u1);