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

    Set_ApplyYLerp(true);

    return S_OK;
}

void CState_Slide::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

HRESULT CState_Slide::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Set_ApplyYLerp(false);

    return S_OK;
}

void CState_Slide::OwnMove(const _float fTimeDelta)
{
    //CStateBase::Move_Front(fTimeDelta);

    CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT* pCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    _float moveps = pPlayerTrans->Get_MovePerSec(); // 속도


    Vec3 vLook = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::LOOK));

    Vec3 disp = vLook * moveps * fTimeDelta;

    pCCT->Move(disp, 0.01f, fTimeDelta);

    Vec3 finalPos = pCCT->GetFootPosition();
    //Vec3 currentPos = m_pOwnerStateComp->Get_Owner()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

    //_float yLerp = std::lerp(currentPos.y, finalPos.y, fTimeDelta);
    //finalPos.y = yLerp;

    pPlayerTrans->Set_Info(TRANSFORM_INFO_STATE::POS, finalPos);
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
