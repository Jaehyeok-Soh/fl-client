#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 불변 데이터
struct IMMU_ELEMENT
{
    int                 iParentIndex;
    row_major float4x4  matPreTransform;
    
    float3 fPadding;
};

// 가변 데이터
struct MU_ELEMENT
{
    row_major float4x4 matLocalTransform;
};

// out put
struct BONE_OUTPUT
{
    row_major float4x4 matCombinedTransform;
};


StructuredBuffer<IMMU_ELEMENT>  IMMU_BONEDATA;
StructuredBuffer<MU_ELEMENT>    LOCAL_TRANSFORMS;

RWStructuredBuffer<BONE_OUTPUT> BONECOMBINED_TRANSFORMS;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    uint i = id.x;

    int iParent = IMMU_BONEDATA[i].iParentIndex;

    float4x4 matParent =
        (iParent < 0) ? IMMU_BONEDATA[i].matPreTransform : BONECOMBINED_TRANSFORMS[iParent].matCombinedTransform;

    BONECOMBINED_TRANSFORMS[i] = mul(LOCAL_TRANSFORMS[i].matLocalTransform, matParent);
}

technique11 T0
{
    pass Particle_Default
    {
        SetVertexShader(NULL);
        GeometryShader = NULL;
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, CS_Main()));
    }
}