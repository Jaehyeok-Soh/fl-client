#include "pch.h"
#include "State_Stun.h"

#include "GameInstance.h"

CState_Stun::CState_Stun(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "SpHit_Stun")
{
}

HRESULT CState_Stun::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Stun::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Stun::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    m_pGameInstance->Play_OneShot(0, m_iSoundHash, 1.f);

    return S_OK;
}

void CState_Stun::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    if (m_fStateElapsed >= m_fStunCoolTime)
    {
        if (Check_Keys(fTimeDelta))
            return;

        Change_PlayerState(STATEKEY::LOOPDONE);			// 다음 state로 change
    }
}

HRESULT CState_Stun::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    return S_OK;
}

_bool CState_Stun::Can_CheckKey(const _float fTimeDelta)
{
    return false; // 우선 key check를 하지 않음
}

CState_Stun* CState_Stun::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_Stun* pInstance = new CState_Stun(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_Stun::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_Stun::Free()
{
    Super::Free();
}
