

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
	float4 Colour	: COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.Pos = mul(input.Pos, World);
	float3 posWorld = output.Pos.xyz;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	float4 materialAmb = float4(0.3, 0.3, 0.3, 0.1);
	float4 materialDiff = float4(1.0, 1.0, 1.0, 1.0);
	float4 lightCol = normalize(LightCol);
	float3 normal = normalize(input.Normal);

	float3 lightDir = normalize(LightPos.xyz - posWorld);
	float diff = max(0.0, dot(lightDir, normal));

	float4 colour = (materialAmb + diff * materialDiff) * lightCol;
	output.Colour = colour;

	return output;
}
