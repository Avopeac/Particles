#define MAX_DIRECTIONAL_LIGHTS 32

float3 ComputeViewPositionFromDepth(float z, float2 uv, float4x4 invProjMat)
{
	float2 screenPos = 2.0 * uv - 1.0;
	screenPos.y = 1.0 - screenPos.y;
	float4 viewPosition = mul(invProjMat, float4(screenPos, z, 1));
	viewPosition.xyz /= viewPosition.w;
	return viewPosition.xyz;
}

struct DirectionalLight
{
	float3 direction;
	float3 intensity;
};

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
}

cbuffer DirectionalLightsBuffer : register(b1)
{
	DirectionalLight lights[MAX_DIRECTIONAL_LIGHTS];
	float4 lightCount;
}

PixelIn VSEntry(VertexIn input)
{
	PixelIn output;
	output.pixelPosition = input.vertexPosition;
	output.pixelUV = input.vertexUV;
	return output;
}

Texture2D<float3> diffuseTexture : register(t0);
Texture2D<float4> normalTexture : register(t1);
Texture2D<float> depthTexture : register(t3);
SamplerState textureSampler : register(s0);

float4 PSEntry(PixelIn input) : SV_TARGET
{
	float z = depthTexture.Sample(textureSampler, input.pixelUV);
	float3 viewPosition = ComputeViewPositionFromDepth(z, input.pixelUV, inverseProjectionMatrix);
	float3 diffuseColor = diffuseTexture.Sample(textureSampler, input.pixelUV);
	float4 viewNormalSpecular = normalTexture.Sample(textureSampler, input.pixelUV);

	float3 lighting = float3(0,0,0);
	float3 ambient = float3(0.16, 0.2, 0.24);
	float3 cameraViewDirection = normalize(-viewPosition);
	for (float j = 0.0; j < lightCount.x; ++j)
	{
		float3 lightViewDir = normalize(mul(viewMatrix, normalize(lights[j].direction)));
		float nDotL = max(0.0, dot(viewNormalSpecular.xyz, lightViewDir));
		if (nDotL > 0.0)
		{
			lighting += diffuseColor * lights[j].intensity * nDotL;
			float3 halfDirection = normalize(lightViewDir + cameraViewDirection);
			float nDotH = max(dot(viewNormalSpecular.xyz, halfDirection), 0.0);
			lighting += diffuseColor * lights[j].intensity * pow(nDotH, 255.0 * viewNormalSpecular.w);
		}	
	}
	
	const float ambientFactor = 0.4;
	float4 finalColor = float4(ambientFactor * ambient + lighting, 1);
	return finalColor;
}
