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

    m_fCapHitMoveTime = Get_MoveBoneTime(60.f);

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

    
    CPhysicsCCT* cct = { nullptr };
    if (cct = Get_OwnerObject()->Get_Component<CPhysicsCCT>())
    {
        cct->SetHeight(0.1f);
    }

    if (m_iMainAnimIdx == ENUM_TO_UINT(ANI::SKY))
    {
        m_FCollisions &= ~COLLISIONFLAGS::C_DOWN;

        Set_YLerp(false);
    }

    Set_ApplyYLerp(true);
    m_FMoves &= ~MOVEFLAGS::OWN;


    {
        Look_Impuls(1.f);
        Set_DeAccelRate(0.5f);
    }

    return S_OK;
}

void CState_Slide::Update(const _float fTimeDelta)
{
    //// pre끝나고 own move를 하도록
    //if (Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
    //{
    //    m_FMoves |= MOVEFLAGS::OWN;
    //}

        // sky animation 일때는 바닥 체크를 하고
    if (m_iMainAnimIdx == ENUM_TO_UINT(ANI::SKY) &&
        Check_OnGround(0.3f))
    {
        m_FCollisions |= COLLISIONFLAGS::C_DOWN;
        Request_ChangeAnimation(m_vecMainAnims[0], true, false, true);

        m_iMainAnimIdx = ENUM_TO_UINT(ANI::NORMAL);
    }

    Super::Update(fTimeDelta);
}

HRESULT CState_Slide::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    m_FCollisions |= COLLISIONFLAGS::C_DOWN;

    Set_ApplyYLerp(false);
    Reset_DeAccelRate();

    Set_YLerp(true);

    CPhysicsCCT* cct = { nullptr };
    if (cct = Get_OwnerObject()->Get_Component<CPhysicsCCT>())
    {
        cct->SetHeight(0.7f); // todo_eunbi : player cct height 값 바뀌면 여기도 바꿔줘야 함
    }

    return S_OK;
}

void CState_Slide::OwnMove(const _float fTimeDelta)
{
    //CStateBase::Move_Front(fTimeDelta);

    //CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
    //CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

    //Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));

    //Vec3 accelation = vLook * pPlayerTrans->Get_MovePerSec();

    //Move(accelation);
}

void CState_Slide::Change_PlayerState(STATEKEY eKey, _bool bForce)
{
    if (ENUM_TO_UINT(ANI::SKY) == m_iMainAnimIdx)
    {
        //_uint {MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, CHARGE, LOOPDONE , END}
        switch(eKey)
        {
        case STATEKEY::LCRTL_UP:
        case STATEKEY::LOOPDONE:
            Request_Change_State(ENUM_TO_UINT(CPlayer::State::FALL)); // 원래 FALL
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
    case ENUM_TO_UINT(CPlayer::State::COMBO_DUAL):
        m_tNextStateDesc.iMainAnimIdx = 1;
        break;

    default:
        Super::Set_NextStateDesc(iNextState);
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
