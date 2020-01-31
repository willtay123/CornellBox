

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
	float2 Tex		: TEXCOORD0;
	float3 Norm		: TEXCOORD1;
	float3 PosWorld	: TEXCOORD2;
};

Texture2D txColor : register(t0);
SamplerState txSampler : register(s0);

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 lightCol = normalize(LightCol);

	float4 textureColour = txColor.Sample(txSampler, input.Tex);

	return textureColour * lightCol;
}

