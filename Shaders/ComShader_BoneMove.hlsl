#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 가변 데이터
struct MU_ELEMENT
{
    int                     iMovingIdx;
    uint                    iBoneNums;
    float                   fRatio;
    float                   Padding0;
    
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
        float3 vPreScale,       vMoveScale,         vFinalScale;
        float4 vPreQuat,        vMoveQuat,          vFinalQuat;
        float3 vPreTranslation, vMoveTranslation,   vFinalTranslation;
        
        vPreScale           = FINAL_SRT[iIdx].vScale;
        vPreQuat            = FINAL_SRT[iIdx].vQuat;
        vPreTranslation     = FINAL_SRT[iIdx].vTranslation;
        
        
        float4x4 matPreTransform = mul(mul(CreateScale(vPreScale), CreateRotaion_FromQuat(vPreQuat)),
                                CreateTranslation(vPreTranslation));
        
        float4x4 matMoveingTranform = mul(g_MuElements.matOffset, matPreTransform);
        
        DecomposeMatrix(matMoveingTranform, vMoveScale, vMoveQuat, vMoveTranslation);
        
        vFinalScale = lerp(vPreScale, vMoveScale, g_MuElements.fRatio);
        vFinalQuat = Slerp(vPreQuat, vMoveQuat, g_MuElements.fRatio);
        vFinalTranslation = lerp(vPreTranslation, vMoveTranslation, g_MuElements.fRatio);
        
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