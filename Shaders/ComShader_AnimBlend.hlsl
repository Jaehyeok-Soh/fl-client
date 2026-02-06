#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 각 채널에서 transform을 만든 다음
// 그 두개의 transform을 선형 보간 한다
// 뼈 기준으로 들어옴

// 불변 데이터
struct IMMU_ELEMENT
{
    int     iRootMotionBoneIndex; // root motion일 경우 tralation을 0으로 만들기 위함
    
    float3  vPadding3;
};

// 가변 데이터
struct MU_ELEMENT
{
    float3  vPreScale;
    float   Padding1;
    
    float4  vPreQuat;
    
    float3  vPreTranslation;
    float   Padding2;
    
    float3  vCurScale;
    float   Padding3;
    
    float4  vCurQuat;
    
    float3  vCurTranslation;
    float   Padding4;
};

struct MU_ELEMENT_ONCE
{
    float   fRatio;
    float3  Padding1;
};

// out put
struct BLENDANIM_OUTPUT
{
    float3  vScale;
    float   Padding1;
    
    float4  vQuat;
    
    float3  vTranslation;
    float   Padding2;
};

cbuffer IMMU_ROOTMOTION
{
    IMMU_ELEMENT g_InputIMMU;
};

cbuffer MU_RATIO
{
    MU_ELEMENT_ONCE g_InputMU;
};
StructuredBuffer<MU_ELEMENT>    MU_TRANSFORMS;

RWStructuredBuffer<BLENDANIM_OUTPUT> UPDATE_DATA; // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함

// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    uint iBoneIdx = id.x;
    
    float3 vScale, vTranslation;
    float4 vQuat;
    
    // SRT 보간
    vScale = lerp(MU_TRANSFORMS[iBoneIdx].vPreScale, MU_TRANSFORMS[iBoneIdx].vCurScale, g_InputMU.fRatio);
    vQuat = normalize(lerp(MU_TRANSFORMS[iBoneIdx].vPreQuat, MU_TRANSFORMS[iBoneIdx].vCurQuat, g_InputMU.fRatio));
    
    if (g_InputIMMU.iRootMotionBoneIndex == iBoneIdx)
        vTranslation = float3(0.f, 0.f, 0.f);
    else
        vTranslation = lerp(MU_TRANSFORMS[iBoneIdx].vPreTranslation, MU_TRANSFORMS[iBoneIdx].vCurTranslation, g_InputMU.fRatio);
    
    // 값 저장
    UPDATE_DATA[iBoneIdx].vScale        = vScale;
    UPDATE_DATA[iBoneIdx].vQuat         = vQuat;
    UPDATE_DATA[iBoneIdx].vTranslation  = vTranslation;
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