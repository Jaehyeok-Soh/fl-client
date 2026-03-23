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

    //Set_RootMotion_Apply(true);

    // pivot positionÀ» ¹Ù¶ó º½

    CTransform*     pPlayerTransform    = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT*    pPlayerCCT          = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    if (pPlayerTransform == nullptr || pPlayerCCT == nullptr) return E_FAIL;

    m_vPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();

    pPlayerTransform->Look_At_XZ(m_vPivot);
    Vec3 vRightDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    vRightDir.Normalize();
    pPlayerTransform->Look_At_Dir(vRightDir * -1.f);

    Vec3 vDistance = m_vPivot - pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
    vDistance.y = 0.f;
    m_fDistance = vDistance.Length();

    return S_OK;
}

void CState_SpecialDash::Update(const _float fTimeDelta)
{
    if (m_fStateElapsed >= 23.f / ANIMTIC)
    {
        //if (Check_Collis(fTimeDelta))
        //    return;

        //if (Check_Keys(fTimeDelta))
        //    return;

       Vec3 vNewPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();

       SetupLook_PointLerp(fTimeDelta, vNewPivot, 10.f);
    }


    else
    {
        CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
        pPlayerCCT->SetZeroHorizontalVelocity();

        CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
        Vec3 vCurPos = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

        Vec3 vAccDir = m_vPivot - vCurPos;
        vAccDir.y = 0.f;
        vAccDir.Normalize();

        Move(vAccDir * m_fDistance * 10.f);

        pPlayerTransform->Look_At_XZ(m_vPivot);
        Vec3 vRightDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
        vRightDir.Normalize();
        pPlayerTransform->Look_At_Dir(vRightDir * -1.f);
    }


    //pPlayerCCT->AddFixedMove(m_vDir * fTimeDelta * (8.f - m_fDeSpeed));
    //pPlayerTransform->Look_At_XZ(m_vPivot);

    //m_fDeSpeed += 7.f * fTimeDelta;

    Super::Update(fTimeDelta);
}

HRESULT CState_SpecialDash::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    //Set_RootMotion_Apply(true);

    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    pPlayerCCT->SetZeroHorizontalVelocity();

    //CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
    //pPlayerTransform->Look_At_XZ(m_vPivot);
    //Set_RootMotion_Apply(false);

    return S_OK;
}

_bool CState_SpecialDash::Can_CheckKey(const _float fTimeDelta)
{
    return (m_fStateElapsed >= 23.f / ANIMTIC);
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
