

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
	float4 Colour	: COLOR;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	return input.Colour;
}

