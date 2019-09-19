cbuffer cbPerFrame{
    float bValue;
}

Texture2D gInput;
RWTexture2D<float4> gOutput;

#define N 256

[numthreads(N,1,1)]
void FadeToBlackCS(int3 groupThreadID : SV_GroupThreadID,
                 int3 dispatchThreadID : SV_DispatchThreadID)
{

    float4 fadeColor = float4(bValue,bValue,bValue,1.0f);
    gOutput[dispatchThreadID.xy] = gInput[dispatchThreadID.xy] * fadeColor;

}

technique11 FadeToBlack{
    pass P0{
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, FadeToBlackCS()));
    }
}