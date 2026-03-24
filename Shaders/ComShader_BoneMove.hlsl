#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 가변 데이터
struct MU_ELEMENT
{
    int                    iMovingIdx;
    uint                    iBoneNums;
    float2                  Padding0;
    
    row_major float4x4      matOffset;
};

struct SRT
{
    float3 vScale;
    float  Padding0;
    
    float4 vQuat;
    
    float3 vTranslation;
    float  Padding1;
};

cbuffer MU_BONENUMS
{
    MU_ELEMENT g_MuElements;
};

StructuredBuffer<SRT>           MU_PRESRTS;

RWStructuredBuffer<SRT>         BONECOMBINED_TRANSFORMS;
StructuredBuffer<SRT>           BONECOMBINED_TRANSFORMS_SRV;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    uint iIdx = id.x;
    
    // 인덱스 방어
    if (iIdx >= g_MuElements.iBoneNums)
        return;
    
    // moving할 뼈라면
    if (g_MuElements.iMovingIdx == iIdx)
    {
        float4x4 matPreTransform = mul(mul(CreateScale(MU_PRESRTS[iIdx].vScale), CreateRotaion_FromQuat(MU_PRESRTS[iIdx].vQuat)),
                                CreateTranslation(MU_PRESRTS[iIdx].vTranslation));
        
        float4x4 matMoveingTranform = mul(matPreTransform, g_MuElements.matOffset);
        
        float3 vFinalScale, vFinalTranslation;
        float4 vFinalQuat;
        DecomposeMatrix(matMoveingTranform, vFinalScale, vFinalQuat, vFinalTranslation);

        BONECOMBINED_TRANSFORMS[iIdx].Padding0      = 0.f;
        BONECOMBINED_TRANSFORMS[iIdx].Padding1      = 0.f;
        BONECOMBINED_TRANSFORMS[iIdx].vScale        = vFinalScale;
        BONECOMBINED_TRANSFORMS[iIdx].vQuat         = vFinalQuat;
        BONECOMBINED_TRANSFORMS[iIdx].vTranslation  = vFinalTranslation;
    }
    
    // 아니라면 바로 넣기
    else
    {
        BONECOMBINED_TRANSFORMS[iIdx].Padding0      = 0.f;
        BONECOMBINED_TRANSFORMS[iIdx].Padding1      = 0.f;
        BONECOMBINED_TRANSFORMS[iIdx].vScale        = MU_PRESRTS[iIdx].vScale;
        BONECOMBINED_TRANSFORMS[iIdx].vQuat         = MU_PRESRTS[iIdx].vQuat;
        BONECOMBINED_TRANSFORMS[iIdx].vTranslation  = MU_PRESRTS[iIdx].vTranslation;
    }

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