#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

struct MU_INDEX
{
    uint    iAffectBoneNums;  // 이 메쉬에 영향을 주는 본 개수
    uint    iTotalBoneNums; // 원본 모델의 전체 본 개수 : output 버퍼의 크기 통일 하기 위함
    float2  Padding0;
};

struct OFFSET_AFFECTIDX
{
    uint                iAffectBoneIndex;
    float3              Padding0;
    
    row_major float4x4  matOffsetTransform;
};

cbuffer MU_MESHBONENUMS
{
    MU_INDEX g_CB;
};

StructuredBuffer<OFFSET_AFFECTIDX>  IMMU_OFFSETMAT;
StructuredBuffer<BONE_MAT>          MU_COMBINEMAT;

RWStructuredBuffer<BONE_MAT> BONEFNIMAL_TRANSFORMS;
StructuredBuffer<BONE_MAT> BONEFNIMAL_TRANSFORMS_SRV;

// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 알맞는 인덱스 들고옴
    uint index = id.x;
    
    bool isAffectEnd = false;
    if (index >= g_CB.iAffectBoneNums) // 만약 affect bone 개수가 넘어갔는데
    {
        if(index >= g_CB.iTotalBoneNums) // 전체 bone 개수도 넘었다면 : return
             return;
        
        else                            // 아직 전체 bone 안쪽이라면 : 그냥 컴바인 행렬로... 일단 넘기자
            isAffectEnd = true;
    }
    
    float4x4 matIdentity = float4x4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
    );
    
    matrix OffsetMat = isAffectEnd ? matIdentity : IMMU_OFFSETMAT[index].matOffsetTransform;
    matrix AffectMat = MU_COMBINEMAT[IMMU_OFFSETMAT[index].iAffectBoneIndex].matBoneTransform;

    BONEFNIMAL_TRANSFORMS[index].matBoneTransform = mul(OffsetMat, AffectMat);
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