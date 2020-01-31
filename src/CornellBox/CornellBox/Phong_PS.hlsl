

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
	float3 Normal	: NORMAL;
	float4 PosWorld	: POSITION;
};

float4 main(VS_OUTPUT input) : SV_Target
{
	float4 ambient = float4(0.1f, 0.1f, 0.1f, 0.1f);
	float4 diffuse = float4(0.1f, 0.1f, 0.1f, 0.1f);
	float spec = 0.5f;
	
	// Get light direction for this pixel
	float3 lightDir = -normalize(LightPos - input.PosWorld);

	float diffuseLighting = saturate(dot(input.Normal, -lightDir));

	// Introduce fall-off of light intensity
	diffuseLighting *= ((length(lightDir) * length(lightDir)) / dot(LightPos - input.PosWorld, LightPos - input.PosWorld));

	// Using Blinn half angle modification for performance over correctness
	float3 h = normalize(normalize(EyePos - input.PosWorld) - lightDir);
	float specLighting = pow(saturate(dot(h, input.Normal)), 2.0f);

	float4 colour = saturate(ambient + (diffuse * diffuseLighting * 0.6f) + (specLighting * 0.5f));
	colour = colour * normalize(LightCol);
	return colour;
}

