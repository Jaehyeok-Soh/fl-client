#include "pch.h"
#include "State_SpecialDash.h"

#include "GameObject.h"
#include "PhysicsCCT.h"
#include "Transform.h"
#include "PlayerActionState.h"

#define Duration 30.f / ANIMTIC
#define OneTrackTime  50.f / ANIMTIC / 4.f
#define RightAcc 5.f
#define LookAcc 4.f

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

    Set_RootMotion_Apply(false);

    // pivot positionÀ» ¹Ù¶ó º½

    CTransform*     pPlayerTransform    = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT*    pPlayerCCT          = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    if (pPlayerTransform == nullptr || pPlayerCCT == nullptr) return E_FAIL;

    m_vPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();
    pPlayerTransform->Look_At_XZ(m_vPivot);

    Vec3 vLookDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    Vec3 vRightDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    vLookDir.Normalize();
    vRightDir.Normalize();

    m_vDir = vRightDir * 2.f + vLookDir;

    SetCCTInputDirection(m_vDir);
    //Vec3 vDir = vRightDir * 2.f + vLookDir; // 30µµzd

    //SetCCTImpuls(vDir * 30.f);

    return S_OK;
}

void CState_SpecialDash::Update(const _float fTimeDelta)
{
    CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

    //Move(m_vDir * fTimeDelta * 10.f);

    //pPlayerTransform->Look_At_XZ(m_vPivot);

    Super::Update(fTimeDelta);
}

HRESULT CState_SpecialDash::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Set_RootMotion_Apply(true);

    //CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    //pPlayerCCT->SetZeroHorizontalVelocity();

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
