#include "Struct_Defines.hlsl"
#include "Animation_Defines.hlsl"
#include "Light_Defines.hlsl"
#include "Struct_Defines.hlsl"

// 불변 데이터
#define NONE 0
#define DROP 1
#define RISE 2
#define SPREAD 3 
#define STOP 4
#define SPIRAL 5
#define DNA 6
#define GATHER 7    // 가운데로 모이기 (Spread 반대)
#define FOUNTAIN 8  // 분수 (튀어올랐다 낙하)
#define LEAF 9

// 시간 데이터
#define PLAY 0
#define PAUSE 1
#define RESET 2
#define STOP 3

// ContinueFlag
#define CONTINUE_NONE 0
#define CONTINUE_PLAY 1
#define CONTINUE_DISTROY 2

// RandomSeed

 #define  RAND_POS  1 << 0
 #define  RAND_LIFE  1 << 1
 #define  RAND_SIZE  1 << 2
 #define  RAND_SPEED 1 << 3

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
    // 1 
    float fTimeDelta;
    float fTotalTime;
    float fDuration;
    float fStartDelay;
    
    // 2
    uint iMoveState;
    int bIsLoop;
    uint iTimeFlag;
    float fPadding4;
    
    // 3
    float3 vFinalGravity; // 계산된 최종 중력 벡터 (방향 * 세기)
    float fExternalStrength; // 외부 중력장 강도
    
    // 4
    float3 vPivot;
    float fPadding1;
    
    // 5
    float3 vLook;
    float fPadding2;
    
    // 6
    float fStartSpeed;
    float fSpiralRadius;
    float fSpiralSpeed;
    int  UseContinueFlag;
    
    // 7
    float3 vRange;
    uint   vRandomSeed;
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

// TODO : Initialize 때 Curve를 무조건 하나 생성하는 것 떄문에 터짐.

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

float GetRandom(float2 seed)
{
    return frac(sin(dot(seed, float2(12.9898f, 78.223f))) * 4378.5453);
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
  
    // 수명 업데이트
    currentData.vLifeTime.x += g_InputB.fTimeDelta;
    float fLife = currentData.vLifeTime.x;
    float fMaxLife = currentData.vLifeTime.y;

    // 그 이후에 fRatio 계산
    float fRatio = saturate(currentData.vLifeTime.x / currentData.vLifeTime.y);
    
    if (g_InputB.UseContinueFlag == CONTINUE_PLAY)
    {
        if (fRatio >= 0.5f)
        {
            currentData.vLifeTime.x = currentData.vLifeTime.y * 0.5f;
        }
    }
    
    if (currentData.vLifeTime.x < 0.0f)
    {
        currentData.matTransform._41_42_43 = float3(9999.f, 9999.f, 9999.f);
        INSTANCE_OUTPUT[dtid.x] = currentData;
        return;
    }
    
    // currentData.vLifeTime.y는 툴에서 준 개별 파티클의 최대 수명
    if ((g_InputB.UseContinueFlag != CONTINUE_DISTROY) && g_InputB.bIsLoop && currentData.vLifeTime.x >= currentData.vLifeTime.y)
    {
        //[기존 위치 공식]
        //currentData.vLifeTime.x = 0.0f; // 리셋 후에는 대기 없이 0부터 시작
        //currentData.matTransform = input.vOriginMatrix;
        //INSTANCE_OUTPUT[dtid.x] = currentData;
        //return;
        
        currentData.vLifeTime.x = 0.0f; // 시간 리셋
        
        float baseSeed = (float) dtid.x + (float) g_InputB.vRandomSeed + g_InputB.fTotalTime;
        
        float3 vOriginPos = input.vOriginMatrix._41_42_43;
        float3 vFinalPos = vOriginPos;
        if (g_InputB.vRandomSeed & RAND_POS)
        {
            float3 vRandOffset;
            vRandOffset.x = (GetRandom(float2(baseSeed, 1.1f)) - 0.5f) * g_InputB.vRange.x;
            vRandOffset.y = (GetRandom(float2(baseSeed, 2.2f)) - 0.5f) * g_InputB.vRange.y;
            vRandOffset.z = (GetRandom(float2(baseSeed, 3.3f)) - 0.5f) * g_InputB.vRange.z;
            
            vFinalPos = vOriginPos + vRandOffset;
        }
        
        float fNewMaxLife = input.vLifeTime.y;
        if (g_InputB.vRandomSeed & RAND_LIFE)
        {
            // input.vLifeTime.x와 y 사이의 값으로 보간
            float randRatio = GetRandom(float2(baseSeed, 4.4f));
            fNewMaxLife = lerp(input.vLifeTime.x, input.vLifeTime.y, randRatio);
            if (fNewMaxLife < 0.1f)
                fNewMaxLife = 0.1f;
        }
        currentData.vLifeTime.y = fNewMaxLife;

        float3 vScale = float3(length(input.vRight.xyz), length(input.vUp.xyz), length(input.vLook.xyz));
        if (g_InputB.vRandomSeed & RAND_SIZE)
        {
            // 기준 크기(vScale)의 0.5 ~ 1.5배 사이로 조절
            float sizeRatio = GetRandom(float2(baseSeed, 5.5f)) + 0.5f;
            vScale *= sizeRatio;
        }
        
        currentData.matTransform = input.vOriginMatrix;
        currentData.matTransform._41_42_43 = vFinalPos;
        
        currentData.matTransform[0].xyz = normalize(currentData.matTransform[0].xyz) * vScale.x;
        currentData.matTransform[1].xyz = normalize(currentData.matTransform[1].xyz) * vScale.y;
        currentData.matTransform[2].xyz = normalize(currentData.matTransform[2].xyz) * vScale.z;

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
        currentData.matTransform._42 += vVelocity.y * g_InputB.fTimeDelta * g_InputB.fStartSpeed;
    }
    else if (g_InputB.iMoveState == GATHER)
    {
        float3 vTargetPos = g_InputB.vPivot;
        float3 vCurPos = currentData.matTransform._41_42_43;
        float fRemainingTime = max(currentData.vLifeTime.y - currentData.vLifeTime.x, 0.01f);
        float3 vToTarget = vTargetPos - vCurPos;
        
        vVelocity = vToTarget / fRemainingTime;
        currentData.matTransform._41_42_43 += vVelocity * g_InputB.fTimeDelta;
    }
    else if (g_InputB.iMoveState == FOUNTAIN)
    {
        float3 vStartVelocity = float3(0.f, g_InputB.fStartSpeed, 0.f);
        float3 vSideDir = normalize(input.vTranslation.xyz - g_InputB.vPivot);
        vSideDir.y = 0.f;
          
        // 수직 속도와 수평 속도를 합산하여 vVelocity 계산
        vVelocity = (vStartVelocity + (vAppliedGravity * currentData.vLifeTime.x)) * input.vSpeed;
        vVelocity += vSideDir * (g_InputB.fStartSpeed * 0.3f);
        
        currentData.matTransform._41_42_43 += vVelocity * g_InputB.fTimeDelta;
    }
    else if (g_InputB.iMoveState == SPREAD)
    {
        float3 vDir = normalize(input.vTranslation.xyz - g_InputB.vPivot);
        float3 vStartVelocity = vDir * g_InputB.fStartSpeed * input.vSpeed;
        
        // 최종 이동 속도를 vVelocity에 저장
        vVelocity = vStartVelocity + (vAppliedGravity * currentData.vLifeTime.x);
        currentData.matTransform._41_42_43 += vVelocity * g_InputB.fTimeDelta;
    }
    
    if (g_InputB.iMoveState == LEAF)
    {
        float fTime = currentData.vLifeTime.x;
        
        // 1. 중력 및 커브 반영 하강 속도 계산
        // vAppliedGravity는 g_InputB.vFinalGravity * 커브값입니다.
        // 물리 공식: 위치 = 초기속도 * t + 0.5 * 가속도 * t^2
        // 하지만 여기선 매 프레임 위치를 더해주므로 '속도' 개념으로 접근합니다.
        
        float3 vGravityEffect = vAppliedGravity * fTime; // 시간에 따라 증가하는 중력 속도
        float fVerticalSpeed = (input.vSpeed + length(vGravityEffect)) * g_InputB.fTimeDelta;

        // 2. 좌우 흔들림 (Swaying) 강화
        // sin 곡선에 따라 좌우로 움직이되, 하강 속도와 연동되면 더 자연스럽습니다.
        float fSwayAmplitude = 1.2f; // 흔들림 범위 (툴 변수로 빼면 좋음)
        float fSwayFreq = 2.5f; // 흔들림 속도
        float fSway = cos(fTime * fSwayFreq + dtid.x) * fSwayAmplitude;
        
        // 3. 최종 위치 적용
        // _41(X), _43(Z)에 흔들림을 주고, _42(Y)는 중력 방향으로 뺍니다.
        currentData.matTransform._41 += fSway * g_InputB.fTimeDelta;
        currentData.matTransform._42 -= fVerticalSpeed;

        // 4. 자전 (Rotation) - 낙엽이 팔랑거리는 느낌
        // 단순히 Y축 회전만 하는 게 아니라, 흔들림(Sway) 방향에 맞춰 살짝 기울어지게 처리
        float fRoll = fSway * 0.5f; // 좌우 이동 방향으로 기울기
        float fPitch = fTime * 2.0f; // 떨어지면서 계속 도는 회전
        
        // 간단한 회전 행렬 생성 (Pitch & Roll 조합)
        float sp, cp, sr, cr;
        sincos(fPitch, sp, cp);
        sincos(fRoll, sr, cr);

        // Y축 중심 회전 + X축 살짝 기울기 조합 (3x3)
        float3x3 rotMat =
        {
            cr, sr * sp, sr * cp,
            0, cp, -sp,
           -sr, cr * sp, cr * cp
        };
        
        // 기존 스케일 유지하며 회전 적용
        float3 vScale = float3(length(input.vRight.xyz), length(input.vUp.xyz), length(input.vLook.xyz));
        currentData.matTransform[0].xyz = mul(float3(1, 0, 0), rotMat) * vScale.x;
        currentData.matTransform[1].xyz = mul(float3(0, 1, 0), rotMat) * vScale.y;
        currentData.matTransform[2].xyz = mul(float3(0, 0, 1), rotMat) * vScale.z;
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