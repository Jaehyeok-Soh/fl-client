#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

struct IMMU_BONEDATA
{
    uint    iBoneIndex;         // 몇번째 뼈인지
    
    uint    iTotalBoneNums;     // 총 뼈 개수
    uint    iRagDollBoneNums;   // 래그돌 할 개수
    
    uint    Padding0;
};

// out put
struct SRT
{
    float3  vScale;
    float   Padding0;
    
    float4  vQuat;
    
    float3  vTranslation;
    float   Padding1;
};


StructuredBuffer<IMMU_BONEDATA>     BONEINDEXES_DATA;

StructuredBuffer<BONE_MAT>          RAGDOLL_LOCAL_TRANSFORMS;   // rag doll system에서 넘겨준 transform
//StructuredBuffer<SRT>               MU_SRTS;                    // channel을 통해 combine 전까지 업데이트된 srt

RWStructuredBuffer<SRT>             RAGDOLL_FINALSRT;
StructuredBuffer<SRT>               RAGDOLL_FINALSRT_SRV;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    // 변수 설정
    uint index = id.x;
    
    uint iBoneIdx = BONEINDEXES_DATA[index].iBoneIndex;
    
    // over된 값 막기
    if (index >= BONEINDEXES_DATA[0].iRagDollBoneNums)
        return;
    
    // ragdoll이 만든 matrix 분해
    float3 vFinalScale, vFinalTranslation;
    float4 vFinalQuat;
    
    DecomposeMatrix(RAGDOLL_LOCAL_TRANSFORMS[index].matBoneTransform, vFinalScale, vFinalQuat, vFinalTranslation);
   
    // 결과 바인딩
    RAGDOLL_FINALSRT[iBoneIdx].vScale           = vFinalScale;
    RAGDOLL_FINALSRT[iBoneIdx].vQuat            = vFinalQuat;
    RAGDOLL_FINALSRT[iBoneIdx].vTranslation     = vFinalTranslation;
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