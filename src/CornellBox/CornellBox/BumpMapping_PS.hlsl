

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
	float4 Pos				: SV_POSITION;
	float2 TexCoord			: TEXCOORD0;
	float3 Normal			: NORMAL;
	float3 Tangent			: TANGENT;
	float3 Binormal			: BINORMAL;
	float3x3 TBN			: POSITION;
};

Texture2D txColour : register(t0);
Texture2D txBump : register(t1);
SamplerState txSampler : register(s0);


float4 main(VS_OUTPUT input) : SV_Target
{
	float4 textureColour = txColour.Sample(txSampler, input.TexCoord);
	float4 bumpMap = txBump.Sample(txSampler, input.TexCoord);

	// Expand the range of the normal value from (0, +1) to (-1, +1).
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
	float3 bumpNormal = mul(bumpMap, input.TBN);
	bumpNormal = normalize(bumpNormal);

	float3 lightDirection = -normalize(LightPos.xyz - input.Pos.xyz);

	// Calculate the amount of light on this pixel based on the bump map normal value.
	float lightIntensity = max(0.0f, dot(bumpNormal, lightDirection));

	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	float4 bumpColour = normalize(LightCol) * lightIntensity;
	// Combine the final bump light color with the texture color.
	float4 colour = bumpColour * textureColour;

	// DEBUG
	//colour = textureColour;
	//colour = bumpColour;
	//colour = bumpMap;
	//colour = float4(lightDirection, 1);
	//colour = float4(bumpNormal, 1);

	return colour;
}

