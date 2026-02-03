#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"
#include "Struct_Defines.hlsl"

// 불변 데이터
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
    float               fStartSpeed;
    float               fEndSpeed;
    uint                MoveFlag;
    int                 IsLoop;
    
    float3              vPivot;
    float3              vLook;
    float              vPadding1;
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
// 불변 데이터(초기 위치, 속도 등) 가져오기
    IMMU_ELEMENT input = IMMU_EFFECT_PARTICLE[dtid.x];
    
//  가변 데이터(직전 프레임까지의 결과) 가져오기
    VTXPARTICLE currentData = INSTANCE_OUTPUT[dtid.x];

//  만약 수명이 0이거나 방금 리셋되었다면, 초기 Matrix를 사용한다.
//  이렇게 하면 CPU가 gOutput을 초기화해서 던져줄 필요가 없음!
    if (currentData.vLifeTime.x <= 0.0f)
    {
        currentData.matTransform = input.vOriginMatrix;
        currentData.vLifeTime = input.vLifeTime; // x: 0, y: Max
    }

//  누적 연산 (델타 타임 적용)
    currentData.vLifeTime.x += g_InputB.fTimeDelta;

//  상태별 이동 로직 (DROP 예시)
    if (g_InputB.MoveFlag == 3)
    {
    // 이전 위치(currentData)에서 속도(input)만큼 더 이동시킨다.
        currentData.matTransform._42 -= input.vSpeed * g_InputB.fTimeDelta * g_InputB.fStartSpeed;
    }

//  루프(리셋) 처리
    if (g_InputB.IsLoop && currentData.vLifeTime.x >= currentData.vLifeTime.y)
    {
        currentData.vLifeTime.x = 0.0f; // 다음 프레임에 상단 if문에 걸려 초기화됨
    }

//  최종 결과 저장
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