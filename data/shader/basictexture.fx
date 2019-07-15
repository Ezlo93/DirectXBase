#include "light.fx"

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

Texture2D diffuseMap;

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;

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
	float2 Tex     : TEXCOORD;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	//vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vout.Tex = vin.Tex;
	return vout;
}
 
float4 PS(VertexOut pin) : SV_Target
{
	float4 texColor = float4(1, 1, 1, 1);
        texColor = diffuseMap.Sample( samAnisotropic, pin.Tex );


    return texColor;
}

technique11 BasicTextureTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );
    }
}
