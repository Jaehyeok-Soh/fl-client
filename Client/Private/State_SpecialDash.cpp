#include "pch.h"
#include "State_SpecialDash.h"

#include "GameObject.h"
#include "PhysicsCCT.h"
#include "Transform.h"
#include "PlayerActionState.h"

#define Duration 50.f / ANIMTIC
#define MoveSpeed 5.f
#define AngleSpeed 1.f

CState_SpecialDash::CState_SpecialDash(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "SpecialDash")
{
}

HRESULT CState_SpecialDash::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_SpecialDash::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_SpecialDash::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    m_fAngle = 0.f;

    Set_RootMotion_Apply(false);



    // pivot position을 바라 봄

    CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    if (pPlayerTransform == nullptr || pPlayerCCT == nullptr) return E_FAIL;

    m_vPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();
    pPlayerTransform->Look_At_XZ(m_vPivot);
    pPlayerCCT->SetZeroHorizontalVelocity();

    Vec3 vDiff  = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS) - m_vPivot;

    m_fRadiaus  = Vec3(vDiff.x, 0.f, vDiff.z).Length(); // XZ만
    m_fAngle    = atan2f(vDiff.z, vDiff.x); // 실제 시작 각도

    return S_OK;
}

void CState_SpecialDash::Update(const _float fTimeDelta)
{
    m_fAngle += AngleSpeed * fTimeDelta;

    // 현재 각도에서 접선 방향 계산 (원의 미분)
    // cos(θ)의 미분 = -sin(θ), sin(θ)의 미분 = cos(θ)
    Vec3 vTangent;
    vTangent.x = -sinf(m_fAngle);
    vTangent.y = 0.f;
    vTangent.z = cosf(m_fAngle);

    float fOrbitalSpeed = AngleSpeed * m_fRadiaus; // 선속도

    // 속도 직접 세팅 (가속도 누적 무시)
    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    pPlayerCCT->SetZeroHorizontalVelocity();
    pPlayerCCT->AddFixedMove(vTangent * fOrbitalSpeed);

    Super::Update(fTimeDelta);
}

HRESULT CState_SpecialDash::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Set_RootMotion_Apply(true);

    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    pPlayerCCT->SetZeroHorizontalVelocity();

    return S_OK;
}

_bool CState_SpecialDash::Can_CheckKey(const _float fTimeDelta)
{
    return Is_MainAnimFinished();
}

CState_SpecialDash* CState_SpecialDash::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_SpecialDash* pInstance = new CState_SpecialDash(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_SpecialDash::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_SpecialDash::Free()
{
    Super::Free();
}
