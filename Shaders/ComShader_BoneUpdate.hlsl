#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 불변 데이터
struct IMMU_ELEMENT
{
    int                 iParentIndex;
    float3              Padding0;
    
    row_major float4x4  matPreTransform;
};

// 가변 데이터
struct MU_ELEMENT
{
    uint                 iMyIdx;
    float3              Padding0;
};

struct MU_SRT
{
    float3 vScale;
    float  Padding0;
    
    float4 vQuat;
    
    float3 vTranslation;
    float  Padding1;
};

// out put
struct BONE_OUTPUT
{
    row_major float4x4 matCombinedTransform;
};


cbuffer BoneGroupCB
{
    uint g_iGroupBoneCount;
};

StructuredBuffer<IMMU_ELEMENT>  IMMU_BONEDATA;
StructuredBuffer<MU_ELEMENT>    MU_INDEXES;
StructuredBuffer<MU_SRT>        MU_SRTS;

RWStructuredBuffer<BONE_OUTPUT> BONECOMBINED_TRANSFORMS;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 알맞는 인덱스 들고옴
    uint iGroupIdx = id.x; // group 내에 idx
    uint iBoneIdx = MU_INDEXES[iGroupIdx].iMyIdx;
    
    if (iGroupIdx >= g_iGroupBoneCount)
        return;

    int iParentIdx = IMMU_BONEDATA[iBoneIdx].iParentIndex;
    
    // 완성된 srt로 나의 local matrix 생성
    float4x4 matLocal = mul(mul(CreateScale(MU_SRTS[iBoneIdx].vScale), CreateRotaion_FromQuat(MU_SRTS[iBoneIdx].vQuat)),
                                CreateTranslation(MU_SRTS[iBoneIdx].vTranslation));

    // parent transform 구해옴
    float4x4 matParent =
        (iParentIdx < 0) ? IMMU_BONEDATA[iBoneIdx].matPreTransform : BONECOMBINED_TRANSFORMS[iParentIdx].matCombinedTransform;

    BONECOMBINED_TRANSFORMS[iBoneIdx].matCombinedTransform = mul(matLocal, matParent);
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