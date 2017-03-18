#define NEAR_PLANE 0.1
#define FAR_PLANE 500.0

struct Particle 
{
  float3 pPosition;
  float3 pVelocity;
  float3 pAcceleration;
  float pLifetime;
  float pAge;
  float pTextureIndex;
  float pCollisions;
  float pCulling;
  float pPainting;
  float pSleeping;
  float pGemitterIndex;
};

struct GlobalParticleEmitterData 
{
	float4 pgeColorPoint0;
	float4 pgeColorPoint1;
	float4 pgeColorPoint2;
	float4 pgeColorPoint3;
	float2 pgeSizePoint0;
	float2 pgeSizePoint1;
	float2 pgeSizePoint2;
	float2 pgeSizePoint3;
	float4 pgeRotationPoints;
	float pgeCollisionsEnabled;
	float pgeCollisionDampening;
	float pgeRestitutionFactor;
	float pgePaintingEnabled;
	float4 pgePaintColor;
	float pgeSleepingEnabled;
	float pgeSleepingCollisionFrames;
	float pgeStretchingEnabled;
	float pgeStretchFactor;
};

float4 SplineInterpolate(float4 p0, float4 p1, float4 p2, float4 p3, float t) 
{
	float i0 = 1.0 - t;
	float i1 = i0 * i0;
	float t2 = t * t;
	return p0 * (i1 * i0) + p1 * (3.0 * i1 * t) + p2 * (3.0 * i0 * t2) + p3 * (t2 * t);
}

float3 SplineInterpolate(float3 p0, float3 p1, float3 p2, float3 p3, float t) 
{
	float i0 = 1.0 - t;
	float i1 = i0 * i0;
	float t2 = t * t;
	return p0 * (i1 * i0) + p1 * (3.0 * i1 * t) + p2 * (3.0 * i0 * t2) + p3 * (t2 * t);
}

float2 SplineInterpolate(float2 p0, float2 p1, float2 p2, float2 p3, float t) 
{
	float i0 = 1.0 - t;
	float i1 = i0 * i0;
	float t2 = t * t;
	return p0 * (i1 * i0) + p1 * (3.0 * i1 * t) + p2 * (3.0 * i0 * t2) + p3 * (t2 * t);	
}

float SplineInterpolate(float p0, float p1, float p2, float p3, float t)
{
	float i0 = 1.0 - t;
	float i1 = i0 * i0;
	float t2 = t * t;
	return p0 * (i1 * i0) + p1 * (3.0 * i1 * t) + p2 * (3.0 * i0 * t2) + p3 * (t2 * t);		
}

// NOTE: Taken from http://www.humus.name/temp/Linearize%20depth.txt (2017-01-02)
float EyeDepth(float depth)
{
	return (NEAR_PLANE * FAR_PLANE) / (FAR_PLANE - depth * (FAR_PLANE - NEAR_PLANE));
}

float NearToFarLinearDepth(float depth)
{
	return depth / (FAR_PLANE - depth * (FAR_PLANE - NEAR_PLANE));
}

float EyeToFarLinearDepth(float depth)
{
	return (2.0 * NEAR_PLANE) / (FAR_PLANE + NEAR_PLANE - depth * (FAR_PLANE - NEAR_PLANE));
}

float WeightFunctionA(float linearDepth) 
{
	return 100.0 / (pow(linearDepth / 5.0, 2.0) + pow(linearDepth / 200.0, 6.0));
}

float WeightFunctionB(float linearDepth)
{
	return 100.0 / (pow(linearDepth / 10.0, 3.0) + pow(linearDepth / 200.0, 6.0));
}

float WeightFunctionC(float linearDepth)
{
	return 100.0 / pow(linearDepth / 200.0, 6.0);
}