

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
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 Pos		: SV_POSITION;
	float3 Norm		: NORMAL;
	float3 PosWorld	: POSITION1;
	float3 ViewDir	: POSITION2;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.PosWorld = input.Pos.xyz;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Norm = input.Normal;
	output.ViewDir = input.Pos.xyz;

	return output;
}
