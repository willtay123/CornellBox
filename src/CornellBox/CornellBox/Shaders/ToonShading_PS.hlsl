

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
	float3 Norm		: TEXCOORD0;
	float3 PosWorld	: TEXCOORD1;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float3 normal = normalize(input.Norm + 1);
	float4 lightCol = normalize(LightCol);

	float4 materialAmb = float4(0.2, 0.1, 0.3, 0.1);
	float4 materialDiff = float4(1.0, 1.0, 1.0, 1.0);
	float3 lightDir = normalize(LightPos.xyz - input.PosWorld);
	float diff = max(0.0, dot(lightDir, normal));

	//float4 colour = (materialAmb + diff * materialDiff) * lightCol;
	float4 colour = float4(1, 0, 1, 0);

	return colour;
}

