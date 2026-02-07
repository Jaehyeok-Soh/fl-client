#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"

// 각 채널에서 transform을 만든 다음
// 그 두개의 transform을 선형 보간 한다
// 뼈 기준으로 들어옴

// 가변 데이터 : gpu
struct MU_SRT
{
    float3  vScale;
    float   Padding0;
    
    float4  vQuat;
    
    float3  vTranslation;
    float   Padding1;
};

// 가변 데이터 : cpu
struct MU_ELEMENT_ONCE  
{
    int     iRootMotionBoneIndex; // root motion일 경우 tralation을 0으로 만들기 위함
    float   fRatio;
    float2  Padding0;
};

// out put
struct BLENDANIM_OUTPUT
{
    float3  vScale;
    float   Padding0;
    
    float4  vQuat;
    
    float3  vTranslation;
    float   Padding1;
};

cbuffer MU_RATIO
{
    MU_ELEMENT_ONCE g_InputMU;
};
StructuredBuffer<MU_SRT> MU_PRETRANSFORMS;
StructuredBuffer<MU_SRT> MU_CURTRANSFORMS;

RWStructuredBuffer<BLENDANIM_OUTPUT> UPDATE_DATA; // bone 인덱스랑 1 : 1 매칭 -> bone update때 문제 없도록 하기 위함

// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    uint iBoneIdx = id.x;
    
    float3 vScale, vTranslation;
    float4 vQuat;
    
    // SRT 보간
    vScale = lerp(MU_PRETRANSFORMS[iBoneIdx].vScale, MU_CURTRANSFORMS[iBoneIdx].vScale, g_InputMU.fRatio);
    vQuat = normalize(lerp(MU_PRETRANSFORMS[iBoneIdx].vQuat, MU_CURTRANSFORMS[iBoneIdx].vQuat, g_InputMU.fRatio));
    
    if (g_InputMU.iRootMotionBoneIndex == iBoneIdx)
        vTranslation = float3(0.f, 0.f, 0.f);
    else
        vTranslation = lerp(MU_PRETRANSFORMS[iBoneIdx].vTranslation, MU_CURTRANSFORMS[iBoneIdx].vTranslation, g_InputMU.fRatio);
    
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