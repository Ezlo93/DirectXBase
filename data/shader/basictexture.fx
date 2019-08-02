#include "light.fx"

cbuffer cbPerFrame{
	DirectionalLight gDirLights;
	float3 gEyePosW;
}

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

Texture2D diffuseMap;
Texture2D gNormalMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float2 Tex : TEXCOORD;
	float3 NormalL     : NORMAL;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex     : TEXCOORD;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	
	return vout;
}
 
float4 PS(VertexOut pin, uniform bool gUseTexture, uniform bool gNormalMapping, uniform bool gUseLighting) : SV_Target
{

	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye); 

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);

  // Sample texture.
   if(gUseTexture){
		texColor = diffuseMap.Sample( samAnisotropic, pin.Tex ); // * gMultiTex.Sample(samAnisotropic, pin.Tex);
		clip(texColor.a - 0.1f); //alpha clipping
   }

	if(!gUseLighting){
		return texColor;
	}

	//normal mapping
	
	float3 bumpedNormalW = pin.NormalW;

	if(gNormalMapping){
		float3 normalMapSample = gNormalMap.Sample(samLinear, pin.Tex).rgb;
		bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);
	}

	// Lighting.
	//

	float4 litColor = texColor;

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Sum the light contribution from each light source.  

		float4 A, D, S;
		ComputeDirectionalLight(gMaterial, gDirLights, bumpedNormalW, toEye, 
			A, D, S);

		ambient += A;
		diffuse += D;
		spec    += S;

	// Modulate with late add.
	litColor = texColor*(ambient + diffuse) + spec;
	
	//litColor = diffuseMap.Sample(samAnisotropic, pin.Tex);

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;

/*
	float4 texColor = float4(1, 1, 1, 1);
        texColor = diffuseMap.Sample( samAnisotropic, pin.Tex );


    return texColor;
//return gMaterial.Ambient;
*/
}

technique11 BasicTextureTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false, true) ) );
    }
}

technique11 BasicTextureNormalMapTech
{
	pass P0 
	{
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, true, true) ) );	
	}
}

technique11 BasicNoTextureTech
{
	pass P0 
	{
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false, false, true) ) );	
	}
}

technique11 BasicTextureNoLighting
{
	pass P0 
	{
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false, false) ) );	
	}
}