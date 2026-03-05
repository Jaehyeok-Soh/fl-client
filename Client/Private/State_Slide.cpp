#include "pch.h"
#include "State_Slide.h"

#include "Player.h"
#include "PhysicsCCT.h"

CState_Slide::CState_Slide(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "Slide")
{
}

HRESULT CState_Slide::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Slide::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Slide::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    if (m_iMainAnimIdx == ENUM_TO_UINT(ANI::SKY))
    {
        m_FCollisions &= ~COLLISIONFLAGS::C_DOWN;
    }

    Set_ApplyYLerp(true);

    // 가속도 구조로 변경하면서 impuls 한번만 주도록 변경
    // ownMove에서 이동함
    // 03/05 소재혁
    {
        CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
        CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

        Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));

        Vec3 accelation = vLook;

        SetCCTImpuls(accelation * 0.5f);
        Set_DeAccelRate(0.5f);
    }

    return S_OK;
}

void CState_Slide::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    if (m_iMainAnimIdx == ENUM_TO_UINT(ANI::SKY) &&
        Check_OnGround(0.3f))
    {
        m_FCollisions |= COLLISIONFLAGS::C_DOWN;
        Request_ChangeAnimation(m_vecMainAnims[0], false, false, true);

        m_iMainAnimIdx = ENUM_TO_UINT(ANI::NORMAL);
    }
}

HRESULT CState_Slide::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    m_FCollisions |= COLLISIONFLAGS::C_DOWN;

    Set_ApplyYLerp(false);
    Reset_DeAccelRate();

    return S_OK;
}

void CState_Slide::OwnMove(const _float fTimeDelta)
{
    ////CStateBase::Move_Front(fTimeDelta);

    //CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
    //CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

    //Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));

    //Vec3 accelation = vLook;

    //SetCCTImpuls(accelation * 10.f);
}

void CState_Slide::Change_PlayerState(STATEKEY eKey)
{
    if (ENUM_TO_UINT(ANI::SKY) == m_iMainAnimIdx)
    {
        //_uint {MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, CHARGE, LOOPDONE , END}
        switch(eKey)
        {
        case STATEKEY::LCRTL_UP:
        case STATEKEY::LOOPDONE:
            Request_Change_State(ENUM_TO_UINT(CPlayer::State::LAND)); // 원래 FALL
            return;

        case STATEKEY::LM:
           return; // 원래 FALL
        }
    }

    Super::Change_PlayerState(eKey);
}

void CState_Slide::Set_NextStateDesc(_uint iNextState)
{
    switch (iNextState)
    {
    case ENUM_TO_UINT(CPlayer::State::COMBO):
        m_tNextStateDesc.iMainAnimIdx = 1;
        break;

    default:
        m_tNextStateDesc.iMainAnimIdx = 0;
    }
}

CState_Slide* CState_Slide::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_Slide* pInstance = new CState_Slide(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_Slide::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_Slide::Free()
{
    Super::Free();
}
