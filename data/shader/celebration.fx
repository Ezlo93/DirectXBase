//***********************************************
// GLOBALS                                      *
//***********************************************

cbuffer cbPerFrame
{
	float3 gEyePosW;
	
	// for when the emit position/direction is varying
	float3 gEmitPosW;
	float3 gEmitDirW;
	
	float gGameTime;
	float gTimeStep;
	float4x4 gViewProj; 
};

cbuffer cbPerObject
{
	float2 gSize = {.1f,.1f};
	float3 gAccelW = {0.0f, 9.8f, 0.0f};
}

cbuffer cbFixed
{
	// Net constant acceleration used to accerlate the particles.
	
	
	// Texture coordinates used to stretch texture over quad 
	// when we expand point particle into a quad.
	float2 gQuadTexC[4] = 
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};


// Array of textures for texturing the particles.
Texture2DArray gTexArray;

// Random texture used to generate random numbers in shaders.
Texture1D gRandomTex;
 
SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
    DepthEnable = FALSE;
    DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
    DepthEnable = TRUE;
    DepthWriteMask = ZERO;
};


//***********************************************
// HELPER FUNCTIONS                             *
//***********************************************
float3 RandUnitVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	// project onto unit sphere
	return normalize(v);
}

float3 RandVec3(float offset)
{
	// Use game time plus offset to sample random texture.
	float u = (gGameTime + offset);
	
	// coordinates in [-1,1]
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	
	return v;
}
 
//***********************************************
// STREAM-OUT TECH                              *
//***********************************************

#define PT_SPAWN 0
#define PT_C1 1
#define PT_C2 2
#define PT_C3 3
#define PT_c4 4
 
struct Particle
{
	float3 InitialPosW : POSITION;
	float3 InitialVelW : VELOCITY;
	float2 SizeW       : SIZE;
	float Age          : AGE;
	uint Type          : TYPE;
};
  
Particle StreamOutVS(Particle vin)
{
	return vin;
}

// The stream-out GS is just responsible for emitting 
// new particles and destroying old particles.  The logic
// programed here will generally vary from particle system
// to particle system, as the destroy/spawn rules will be 
// different.
[maxvertexcount(6)]
void StreamOutGS(point Particle gin[1], 
                 inout PointStream<Particle> ptStream)
{	
	gin[0].Age += gTimeStep;

	if( gin[0].Type == PT_SPAWN )
	{	
		// time to emit a new particle?
		if( gin[0].Age > 0.002f )
		{
			for(int i = 0; i < 5; ++i)
			{
				// Spread rain drops out above the camera.
				float3 r = RandVec3((float)i/5.0f);
				float3 vRandom = 2.0f*r;
				vRandom.y = 0.0f;
			
				Particle p;
				p.InitialPosW = gEmitPosW.xyz + vRandom;
				p.InitialVelW = float3(0.f,0.f,0.f);//float3(r.x, 0.0f, r.z);
				p.SizeW       = float2(1.f,1.f);
				p.Age         = 0.0f;
				p.Type        = i%5+1;
			
				ptStream.Append(p);
			}
			
			// reset the time to emit
			gin[0].Age = 0.0f;
		}

		// always keep emitters
		ptStream.Append(gin[0]);
	}
	else
	{
		// Specify conditions to keep particle; this may vary from system to system.
		if( gin[0].Age <= 3.f )
			ptStream.Append(gin[0]);
	}		
}

GeometryShader gsStreamOut = ConstructGSWithSO( 
	CompileShader( gs_5_0, StreamOutGS() ), 
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x" );
	
technique11 StreamOutTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_5_0, StreamOutVS() ) );
        SetGeometryShader( gsStreamOut );
        
        // disable pixel shader for stream-out only
        SetPixelShader(NULL);
        
        // we must also disable the depth buffer for stream-out only
        SetDepthStencilState( DisableDepth, 0 );
    }
}

//***********************************************
// DRAW TECH                                    *
//***********************************************

struct VertexOut
{
	float3 PosW  : POSITION;
	float2 SizeW : SIZE;
	uint   Type  : TYPE;
};

VertexOut DrawVS(Particle vin)
{
	VertexOut vout;
	
	float t = vin.Age;
	
	// constant acceleration equation
	vout.PosW = 0.5f*t*t*gAccelW + t*vin.InitialVelW + vin.InitialPosW;
	vout.SizeW = gSize;
	vout.Type  = vin.Type;
	
	return vout;
}

struct GeoOut
{
	float4 PosH  : SV_Position;
	float2 Tex   : TEXCOORD;
	uint Type : TYPE;
};

// The draw GS just expands points into lines.
[maxvertexcount(4)]
void DrawGS(point VertexOut gin[1], 
            inout TriangleStream<GeoOut> triStream)
{	
	// do not draw emitter particles.
	if( gin[0].Type != PT_SPAWN )
	{
		float3 look  = normalize(gEyePosW.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0,1,0), look));
		float3 up    = cross(look, right);
		
		//
		// Compute triangle strip vertices (quad) in world space.
		//
		float halfWidth  = 0.5f*gin[0].SizeW.x;
		float halfHeight = 0.5f*gin[0].SizeW.y;
	
		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth*right - halfHeight*up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth*right + halfHeight*up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth*right - halfHeight*up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth*right + halfHeight*up, 1.0f);
		
		//
		// Transform quad vertices to world space and output 
		// them as a triangle strip.
		//
		GeoOut gout;
		[unroll]
		for(int i = 0; i < 4; ++i)
		{
			gout.PosH  = mul(v[i], gViewProj);
			gout.Tex   = gQuadTexC[i];
			gout.Type = gin[0].Type;
			triStream.Append(gout);
		}	

		// // Slant line in acceleration direction.
		// float3 p0 = gin[0].PosW;
		// float3 p1 = gin[0].PosW ;
		
		// GeoOut v0;
		// v0.PosH = mul(float4(p0, 1.0f), gViewProj);
		// v0.Tex = float2(0.0f, 0.0f);
		// v0.Type = gin[0].Type;
		// lineStream.Append(v0);
		
		// GeoOut v1;
		// v1.PosH = mul(float4(p1, 1.0f), gViewProj);
		// v1.Tex  = float2(1.0f, 1.0f);
		// v1.Type = gin[0].Type;
		// lineStream.Append(v1);
	}
}

float4 DrawPS(GeoOut pin) : SV_TARGET
{
	return gTexArray.Sample(samLinear, float3(pin.Tex,pin.Type-1));
}

technique11 DrawTech
{
    pass P0
    {
        SetVertexShader(   CompileShader( vs_5_0, DrawVS() ) );
        SetGeometryShader( CompileShader( gs_5_0, DrawGS() ) );
        SetPixelShader(    CompileShader( ps_5_0, DrawPS() ) );
        
        SetDepthStencilState( NoDepthWrites, 0 );
    }
}