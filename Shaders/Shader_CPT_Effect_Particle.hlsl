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
    float4 vRight;
    float4 vUp;
    float4 vLook;
    float4 vTranslation;
    
    float2 vLifeTime;
    
    float vSpeed;
    float vPadding;
    row_major float4x4 vOriginMatrix;
};

// 가변 데이터
struct MU_ELEMENT
{
    float fTimeDelta;
    float fTotalTime;
    float fDuration;
    float fStartDelay;
    
    uint iMoveState;
    int bIsLoop;
    uint iTimeFlag;
    float fPadding4;
    
    float3 vFinalGravity; // 계산된 최종 중력 벡터 (방향 * 세기)
    float fExternalStrength; // 외부 중력장 강도
    
    float3 vPivot;
    float fPadding1;
    float3 vLook;
    float fPadding2;
    
    float fStartSpeed;
    float fSpiralRadius;
    float fSpiralSpeed;
    float fPadding3;
};

struct CurveKey
{
    float fTimeKey; // 0.0 ~ 1.0
    float fValue; // 해당 시간의 값
};

cbuffer MU_ParticleUpdate
{
    MU_ELEMENT g_InputB;
};

cbuffer CurveInfo
{
    uint g_iGravityKeyCount;
    float3 g_vPadding;
};

StructuredBuffer<CurveKey> g_GravityCurve;
StructuredBuffer<IMMU_ELEMENT> IMMU_EFFECT_PARTICLE;
RWStructuredBuffer<VTXPARTICLE> INSTANCE_OUTPUT;
StructuredBuffer<VTXPARTICLE> INSTANCE_RESULT_SRV;

// 중력을 전체적으로 관리하니까 난리가 나서 개별적으로 뺀다.
float SampleCurve(StructuredBuffer<CurveKey> curve, uint keyCount, float fRatio)
{
    // 키가 없으면 기본값 1.0 반환
    if (keyCount == 0)
        return 1.0f;
    
    // 키가 하나면 그 값 반환
    if (keyCount == 1)
        return curve[0].fValue;
    
    // 시간이 0보다 작거나 1보다 크면 처음/끝값 반환
    if (fRatio <= curve[0].fTimeKey)
        return curve[0].fValue;
    
    if (fRatio >= curve[keyCount - 1].fTimeKey)
        return curve[keyCount - 1].fValue;

    // 현재 fRatio가 위치한 인덱스 찾기 (선형 탐색)
    for (uint i = 0; i < keyCount - 1; ++i)
    {
        if (fRatio >= curve[i].fTimeKey && fRatio <= curve[i + 1].fTimeKey)
        {
            // 두 키 사이의 비율 계산 (t)
            float t = (fRatio - curve[i].fTimeKey) / (curve[i + 1].fTimeKey - curve[i].fTimeKey);
            // 선형 보간
            return lerp(curve[i].fValue, curve[i + 1].fValue, t);
        }
    }
    return curve[keyCount - 1].fValue;
}

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
        currentData.vLifeTime.x = input.vLifeTime.x;
        currentData.vLifeTime.y = input.vLifeTime.y;
        INSTANCE_OUTPUT[dtid.x] = currentData;
        return;
    }
  
    // 수명 업데이트 (누적)
    currentData.vLifeTime.x += g_InputB.fTimeDelta;
    
    // 개별 파티클의 시간 비율 (0.0 ~ 1.0) 
    float fRatio = saturate(currentData.vLifeTime.x / currentData.vLifeTime.y);
    
    if (currentData.vLifeTime.x < 0.0f)
    {
        currentData.matTransform._41_42_43 = float3(9999.f, 9999.f, 9999.f);
        INSTANCE_OUTPUT[dtid.x] = currentData;
        return;
    }
    
    // currentData.vLifeTime.y는 툴에서 준 개별 파티클의 최대 수명
    if (g_InputB.bIsLoop && currentData.vLifeTime.x >= currentData.vLifeTime.y)
    {
        currentData.vLifeTime.x = 0.0f; // 리셋 후에는 대기 없이 0부터 시작
        currentData.matTransform = input.vOriginMatrix;
        INSTANCE_OUTPUT[dtid.x] = currentData;
        return;
    }
    
    float3 vVelocity = float3(0.f, 0.f, 0.f);

    // 중력 커브 샘플링
    float fGravityCurveValue = SampleCurve(g_GravityCurve, g_iGravityKeyCount, fRatio);
    float3 vAppliedGravity = g_InputB.vFinalGravity * fGravityCurveValue;

    // --- 이동 로직 시작 ---
    if (g_InputB.iMoveState == DROP)
    {
        vVelocity = float3(0.f, -input.vSpeed, 0.f);
        currentData.matTransform._42 += vVelocity.y * g_InputB.fTimeDelta;
    }
    else if (g_InputB.iMoveState == RISE)
    {
        vVelocity = float3(0.f, input.vSpeed, 0.f);
        currentData.matTransform._42 += vVelocity.y * g_InputB.fTimeDelta;
    }
    else if (g_InputB.iMoveState == GATHER)
    {
        float3 vTargetPos = g_InputB.vPivot;
        float3 vCurPos = currentData.matTransform._41_42_43;
        float fRemainingTime = max(currentData.vLifeTime.y - currentData.vLifeTime.x, 0.01f);
        float3 vToTarget = vTargetPos - vCurPos;
        
        // [수정] 지역 변수가 아닌 공용 vVelocity에 저장
        vVelocity = vToTarget / fRemainingTime;
        currentData.matTransform._41_42_43 += vVelocity * g_InputB.fTimeDelta;
    }
    else if (g_InputB.iMoveState == FOUNTAIN)
    {
        float3 vStartVelocity = float3(0.f, g_InputB.fStartSpeed, 0.f);
        float3 vSideDir = normalize(input.vTranslation.xyz - g_InputB.vPivot);
        vSideDir.y = 0.f;
        
        // [수정] 수직 속도와 수평 속도를 합산하여 vVelocity 계산
        vVelocity = (vStartVelocity + (vAppliedGravity * currentData.vLifeTime.x)) * input.vSpeed;
        vVelocity += vSideDir * (g_InputB.fStartSpeed * 0.3f);
        
        currentData.matTransform._41_42_43 += vVelocity * g_InputB.fTimeDelta;
    }
    else if (g_InputB.iMoveState == SPREAD)
    {
        float3 vDir = normalize(input.vTranslation.xyz - g_InputB.vPivot);
        float3 vStartVelocity = vDir * g_InputB.fStartSpeed * input.vSpeed;
        
        // [수정] 최종 이동 속도를 vVelocity에 저장
        vVelocity = vStartVelocity + (vAppliedGravity * currentData.vLifeTime.x);
        currentData.matTransform._41_42_43 += vVelocity * g_InputB.fTimeDelta;
    }

    if (g_InputB.iMoveState == SPREAD || g_InputB.iMoveState == FOUNTAIN || g_InputB.iMoveState == GATHER)
    {
        if (length(vVelocity) > 0.001f)
        {
            float3 vLook = normalize(vVelocity);
            
            float3 vWorldUp = abs(vLook.y) > 0.99f ? float3(0, 0, 1) : float3(0, 1, 0);
            float3 vRight = normalize(cross(vWorldUp, vLook));
            float3 vUp = cross(vLook, vRight);

            float3 vScale = float3(
                length(currentData.matTransform[0].xyz),
                length(currentData.matTransform[1].xyz),
                length(currentData.matTransform[2].xyz)
            );

            currentData.matTransform[0].xyz = vRight * vScale.x;
            currentData.matTransform[1].xyz = vUp * vScale.y;
            currentData.matTransform[2].xyz = vLook * vScale.z;
        }
    }
    
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