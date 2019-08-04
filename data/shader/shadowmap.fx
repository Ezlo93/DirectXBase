
cbuffer cbPerFrame
{
	float3 gEyePosW;
	float gHeightScale;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gViewProj;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
 
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL     : POSITION;
	float2 Tex      : TEXCOORD;
	float3 NormalL  : NORMAL;
	float3 TangentW : TANGENT;
	
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex  : TEXCOORD;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.Tex  = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

void PS(VertexOut pin)
{
	float4 diffuse = gDiffuseMap.Sample(samLinear, pin.Tex);

	// clip transparent
	clip(diffuse.a - 0.15f);
}

RasterizerState Depth
{

	DepthBias = 100000;
        DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
};

technique11 ShadowMapTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS() ) );

	SetRasterizerState(Depth);
    }
}