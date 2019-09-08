#include "LightHelper.fx"

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
    float4x4 gShadowTransform;
	Material gMaterial;
	float4 gStaticColor;
}; 

Texture2D gDiffuseMap;
Texture2D gShadowMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;

	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
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
	float2 Tex     : TEXCOORD0;
        float4 ShadowPosH : TEXCOORD1;
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

	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);	

	return vout;
}
 
float4 PS(VertexOut pin, uniform bool gUseTexture, uniform bool gUseLighting, uniform bool gUseStaticColor) : SV_Target
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
		texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex ); // * gMultiTex.Sample(samAnisotropic, pin.Tex);
		clip(texColor.a - 0.1f); //alpha clipping
   }

	if(!gUseLighting){
		texColor.a = gMaterial.Diffuse.a;
		return texColor;
	}

	if(gUseStaticColor){
		texColor = gStaticColor;
	}

	// Lighting.
	//

	float4 litColor = texColor;

	// Start with a sum of zero. 
	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//shadow
	float3 shadow = float3(1.0f, 1.0f, 1.0f);
	shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pin.ShadowPosH);



	// Sum the light contribution from each light source.  

		float4 A, D, S;
		ComputeDirectionalLight(gMaterial, gDirLights, pin.NormalW, toEye, 
			A, D, S);

		ambient += A;
		diffuse += shadow[0]*D;
		spec    += shadow[0]*S;

	// Modulate with late add.
	litColor = texColor*(ambient + diffuse) + spec;

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;

}

technique11 BasicTextureTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true,  true, false) ) );
    }
}

technique11 BasicNoTextureTech
{
	pass P0 
	{
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false, true, false) ) );	
	}
}

technique11 BasicTextureNoLighting
{
	pass P0 
	{
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(true, false,false) ) );	
	}
}

technique11 BasicStaticColor
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(false,  true, true) ) );
    }
}