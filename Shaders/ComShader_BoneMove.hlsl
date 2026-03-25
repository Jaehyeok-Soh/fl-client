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

cbuffer MU_BONEMOVE
{
    MU_ELEMENT g_MuElements;
};

RWStructuredBuffer<SRT>         FINAL_SRT;
StructuredBuffer<SRT>           FINAL_SRT_SRV;


// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    uint iIdx = id.x;
    
    // 인덱스 방어
    if (iIdx >= g_MuElements.iBoneNums)
        return;
    
    float4x4 matIdentity = float4x4(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
    );
    
    bool bIdentity = (g_MuElements.matOffset == matIdentity);
    
    // moving할 뼈라면
    if (g_MuElements.iMovingIdx == iIdx)
    {
        float4x4 matPreTransform = mul(mul(CreateScale(FINAL_SRT[iIdx].vScale), CreateRotaion_FromQuat(FINAL_SRT[iIdx].vQuat)),
                                CreateTranslation(FINAL_SRT[iIdx].vTranslation));
        
        float4x4 matMoveingTranform = mul(g_MuElements.matOffset, matPreTransform);
        
        float3 vFinalScale, vFinalTranslation;
        float4 vFinalQuat;
        DecomposeMatrix(matMoveingTranform, vFinalScale, vFinalQuat, vFinalTranslation);

        FINAL_SRT[iIdx].Padding0 = 0.f;
        FINAL_SRT[iIdx].Padding1 = 0.f;
        FINAL_SRT[iIdx].vScale = vFinalScale;
        FINAL_SRT[iIdx].vQuat = vFinalQuat;
        FINAL_SRT[iIdx].vTranslation = vFinalTranslation;
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