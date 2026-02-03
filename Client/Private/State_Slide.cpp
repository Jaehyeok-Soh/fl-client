#include "pch.h"
#include "State_Slide.h"

#include "Player.h"

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

    return S_OK;
}

void CState_Slide::OwnMove(const _float fTimeDelta)
{
    CStateBase::Move_Front(fTimeDelta);
}

void CState_Slide::Change_State(STATEKEY eKey)
{
    if (ENUM_TO_UINT(ANI::SKY) == m_iMainAnimIdx)
    {
        switch(eKey)
        {
        case STATEKEY::LCRTL_UP:
        case STATEKEY::LOOPDONE:
            Request_Change_State(ENUM_TO_UINT(CPlayer::State::LAND)); // ¿ø·¡ FALL
            return;
        }
    }

    Super::Change_State(eKey);
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
