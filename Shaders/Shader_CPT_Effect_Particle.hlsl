#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"

struct Data
{
    float3 v1;
    float vPadding;
    float2 v2;
};

StructuredBuffer<Data> gInputA;
StructuredBuffer<Data> gInputB;
RWStructuredBuffer<Data> gOutput;

// Read Write가 둘다 된다고 해서 RWStructuredBuffer

[numthreads(31, 1, 1)]
void CS_Main(int3 dtid : SV_DispatchThreadID)
{
    gOutput[dtid.x].v1 = gInputA[dtid.x].v1 + gInputB[dtid.x].v1;
    gOutput[dtid.y].v1 = gInputA[dtid.y].v2 + gInputB[dtid.y].v2;
}

technique11 T0
{
    pass Particle_Default
    {
        SetVertexShader(NULL);
        GeometryShader = NULL;
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(ps_5_0, PS_DEFAULT()));
    }
}