struct VertexIn
{
	float3 vertexPosition : POSITION;
};

struct PixelIn
{
	float4 pixelPosition : SV_POSITION;
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

cbuffer MatrixData : register(b1)
{
	float4x4 worldMatrix;
}

PixelIn VSEntry(VertexIn input)
{
	PixelIn output;
	output.pixelPosition = mul(viewProjectionMatrix, mul(worldMatrix, float4(input.vertexPosition, 1)));
	return output;
}

float4 PSEntry(PixelIn input) : SV_TARGET
{
	return float4(0,1,0,1);
}
