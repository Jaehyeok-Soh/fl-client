#include "pch.h"
#include "State_SpecialDash.h"

#include "GameObject.h"
#include "PhysicsCCT.h"
#include "Transform.h"
#include "PlayerActionState.h"

#include "GameInstance.h"

#define Duration 30.f / ANIMTIC

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

    // pivot 갱신을 위해 true로 해둠
    static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Set_SpecialDashOn(true);
    m_vPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();

    CTransform*     pPlayerTransform    = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT*    pPlayerCCT          = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    if (pPlayerTransform == nullptr || pPlayerCCT == nullptr) return E_FAIL;

    // pivot을 기준으로 접선 방향을 바라 보게 만듦
    {
        pPlayerTransform->Look_At_XZ(m_vPivot);
        Vec3 vRightDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
        vRightDir.Normalize();
        pPlayerTransform->Look_At_Dir(vRightDir * -1.f);
    }

    // distance 기록
    {
        Vec3 vDistance = m_vPivot - pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
        vDistance.y = 0.f;
        m_fDistance = vDistance.Length();
    }

    m_pGameInstance->Request_SloMo(0.3f, 0.5f);

    return S_OK;
}

void CState_SpecialDash::Update(const _float fTimeDelta)
{
    // animation 전 미리 탈출
    if (m_fStateElapsed >= (23.f / ANIMTIC) + 0.5f)
    {
        if (Check_Collis(fTimeDelta))
            return;

        if (Check_Keys(fTimeDelta))
            return;

        Change_PlayerState(STATEKEY::LOOPDONE);			// 다음 state로 change
        return;
    }

    // pivot 갱신 및 자연스럽게 바라보도록
    if (m_fStateElapsed >= 23.f / ANIMTIC)
    {
       Vec3 vNewPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();

       SetupLook_PointLerp(fTimeDelta, vNewPivot, 10.f);
    }

    // 움직임
    else
    {
        CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
        CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();

        // 가속 누적을 막기 위해 zero로 셋팅
        {
            pPlayerCCT->SetZeroHorizontalVelocity();
        }

        // pivot과 거리 기반으로 원심력을 줌
        {
            Vec3 vCurPos = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

            Vec3 vAccDir = m_vPivot - vCurPos;
            vAccDir.y = 0.f;
            vAccDir.Normalize();

            //Vec3 vDistance = m_vPivot - vCurPos;
            //vDistance.y = 0.f;
            //m_fDistance = vDistance.Length();

            Move(vAccDir * m_fDistance * 15.f);
        }

        // 접선방향으로 바라보도록 만듦
        {
            pPlayerTransform->Look_At_XZ(m_vPivot);
            Vec3 vRightDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
            vRightDir.Normalize();
            pPlayerTransform->Look_At_Dir(vRightDir * -1.f);
        }
    }

    Super::Update(fTimeDelta);
}

HRESULT CState_SpecialDash::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    pPlayerCCT->SetZeroHorizontalVelocity();

    // pivot 갱신을 위해 true로 해둠
    static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Set_SpecialDashOn(false);

    return S_OK;
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
