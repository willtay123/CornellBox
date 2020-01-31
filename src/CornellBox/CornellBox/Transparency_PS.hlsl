

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 EyePos;
	float4 LightPos;
	float4 LightCol;
}

struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float3 Norm		: NORMAL;
	float3 PosWorld	: POSITION;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 objectColour = float4(0.8f, 0.9f, 0.9f, 1.0f);
	objectColour *= normalize(LightCol);

	return float4(0.5 * objectColour.xyz, 0.6f);
}

