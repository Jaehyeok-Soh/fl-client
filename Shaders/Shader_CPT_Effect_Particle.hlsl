#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"
#include "Struct_Defines.hlsl"

// 불변 데이터
#define NONE 0
#define DROP 1
#define RISE 2
#define SPREAD 3 
#define STARIGHT 4
#define SPIRAL 5
#define DNA 6
#define GATHER 7    // 가운데로 모이기 (Spread 반대)
#define FOUNTAIN 8  // 분수 (튀어올랐다 낙하)

// 시간 데이터
#define PLAY 0
#define PAUSE 1
#define RESET 2
#define STOP 3

struct IMMU_ELEMENT
{
    float4             vRight;
    float4             vUp;
    float4             vLook;
    float4             vTranslation;
    
    float2             vLifeTime;
    
    float              vSpeed;
    float              vPadding;
    row_major float4x4 vOriginMatrix;
};

// 가변 데이터
struct MU_ELEMENT
{
    float               fTimeDelta;
    float               fTotalTime;
    float               fDuration;
    float               fStartDelay;
    
    uint                iMoveState;
    int                 bIsLoop;
    uint                iTimeFlag;
    float               fPadding4;
    
    float3              vFinalGravity; // 계산된 최종 중력 벡터 (방향 * 세기)
    float               fExternalStrength; // 외부 중력장 강도
    
    float3              vPivot;
    float               fPadding1;
    float3              vLook;
    float               fPadding2;
    
    float               fStartSpeed;
    float               fSpiralRadius;
    float               fSpiralSpeed;
    float               fPadding3;
};


cbuffer MU_ParticleUpdate
{
    MU_ELEMENT g_InputB;
};


StructuredBuffer<IMMU_ELEMENT> IMMU_EFFECT_PARTICLE;
RWStructuredBuffer<VTXPARTICLE> INSTANCE_OUTPUT;
StructuredBuffer<VTXPARTICLE> INSTANCE_RESULT_SRV;

// Read Write가 둘다 된다고 해서 RWStructuredBuffer

// Warp/Wavefront는 32명씩 묶여서 연산을 한다.
[numthreads(32, 1, 1)]
void CS_Main(int3 dtid : SV_DispatchThreadID)
{
    IMMU_ELEMENT input = IMMU_EFFECT_PARTICLE[dtid.x];
    VTXPARTICLE currentData = INSTANCE_OUTPUT[dtid.x];

    // 리셋/중단 강제 명령 처리
    if (g_InputB.iTimeFlag == RESET || g_InputB.iTimeFlag == STOP)
    {
        currentData.matTransform = input.vOriginMatrix;
        currentData.vLifeTime.x = 0.f;
        currentData.vLifeTime.y = input.vLifeTime.y;
        INSTANCE_OUTPUT[dtid.x] = currentData;
        return;
    }

    // 수명 업데이트 (누적)
    currentData.vLifeTime.x += g_InputB.fTimeDelta;
    
    // currentData.vLifeTime.y는 툴에서 준 개별 파티클의 최대 수명입니다.
    if (g_InputB.bIsLoop && currentData.vLifeTime.x >= currentData.vLifeTime.y)
    {
        currentData.vLifeTime.x = 0.0f;
        currentData.matTransform = input.vOriginMatrix;
        
        INSTANCE_OUTPUT[dtid.x] = currentData;
        return;
    }

    // 이동 로직 (수명이 유효할 때만 수행)
    if (g_InputB.iMoveState == DROP)
        currentData.matTransform._42 -= input.vSpeed * g_InputB.fTimeDelta;
    else if (g_InputB.iMoveState == RISE)
        currentData.matTransform._42 += input.vSpeed * g_InputB.fTimeDelta;
    
    if (g_InputB.iMoveState == GATHER)
    {
        // Spread와 반대로 Pivot에서 입자 위치를 빼는 게 아니라, 입자 위치에서 Pivot을 향하게 함
        float3 vDir = normalize(g_InputB.vPivot - input.vTranslation.xyz);
        currentData.matTransform._41_42_43 += vDir * g_InputB.fTimeDelta * g_InputB.fStartSpeed;
    }

    // FOUNTAIN: 분수처럼 솟구쳤다가 중력에 의해 낙하 (포물선)
    else if (g_InputB.iMoveState == FOUNTAIN)
    {
       // [수정] vFinalGravity의 y값(혹은 길이)을 사용하여 수직 속도 계산
    // 만약 vFinalGravity가 (0, -9.8, 0)이라면 절대값이나 .y를 사용해야 합니다.
        float fGravityStrength = length(g_InputB.vFinalGravity);
    
        float fVerticalVelocity = g_InputB.fStartSpeed - (fGravityStrength * currentData.vLifeTime.x);
    
        currentData.matTransform._42 += fVerticalVelocity * g_InputB.fTimeDelta;
    
    // 옆으로 퍼지는 효과
        float3 vSideDir = normalize(input.vTranslation.xyz - g_InputB.vPivot);
        currentData.matTransform._41_43 += vSideDir.xz * g_InputB.fTimeDelta * (g_InputB.fStartSpeed * 0.3f);
    }
    
    else if (g_InputB.iMoveState == SPREAD)
    {
        float3 vDir = normalize(input.vTranslation.xyz - g_InputB.vPivot);
        currentData.matTransform._41_42_43 += vDir * g_InputB.fTimeDelta * g_InputB.fStartSpeed;
    }

    // 최종 결과 저장
    INSTANCE_OUTPUT[dtid.x] = currentData;
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