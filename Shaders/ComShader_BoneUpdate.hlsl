#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 불변 데이터
struct IMMU_ELEMENT
{
    int                 iParentIndex;
    row_major float4x4  matPreTransform;
    
    float3              vPadding3;
};

// 가변 데이터
struct MU_ELEMENT
{
    int                iMyIdx;
    row_major float4x4 matLocalTransform;
    
    float3              vPadding3;
};

// out put
struct BONE_OUTPUT
{
    row_major float4x4 matCombinedTransform;
};


StructuredBuffer<IMMU_ELEMENT>  IMMU_BONEDATA;
StructuredBuffer<MU_ELEMENT>    MU_DATA;

RWStructuredBuffer<BONE_OUTPUT> BONECOMBINED_TRANSFORMS;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 알맞는 인덱스 들고옴
    uint iGroupIdx = id.x; // group 내에 idx
    uint iBoneIdx = MU_DATA[iGroupIdx].iMyIdx;

    int iParentIdx = IMMU_BONEDATA[iBoneIdx].iParentIndex;

    float4x4 matParent =
        (iParentIdx < 0) ? IMMU_BONEDATA[iBoneIdx].matPreTransform : BONECOMBINED_TRANSFORMS[iParentIdx].matCombinedTransform;

    BONECOMBINED_TRANSFORMS[iBoneIdx] = mul(MU_DATA[iGroupIdx].matLocalTransform, matParent);
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