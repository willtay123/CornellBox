
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
	float3 PosWorld	: POSITION1;
	float3 ViewDir	: POSITION2;
};

TextureCube txBoxTexture : register(t0);
SamplerState txBoxSampler : register(s0);

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 colour = float4(txBoxTexture.Sample(txBoxSampler, input.ViewDir).xyz, 1.0f);
	return colour;
}

