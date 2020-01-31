

cbuffer ConstantBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 EyePos;
	float4 LightPos;
	float4 LightCol;
}

struct VS_INPUT
{
	float3 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float3 Normal	: NORMAL;
	float4 PosWorld	: POSITION;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float4 posTemp = float4(input.Pos, 1);
	output.Pos = mul(posTemp, World);
	output.PosWorld = output.Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	
	float4 tempNormal = float4(input.Normal, 0);
	tempNormal = mul(tempNormal, World);
	output.Normal = normalize(tempNormal);
	
	return output;
}
