#include "pch.h"
#include "State_GunReload.h"

#include "Player.h"
#include "Weapon.h"
#include "GameInstance.h"

CState_GunReload::CState_GunReload(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "GunReload")
{
}

HRESULT CState_GunReload::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_GunReload::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_GunReload::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    Change_Weapon(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HAND));

    Start_Att(ENUM_TO_UINT(CPlayer::State::GUNRELOAD));

    return S_OK;
}

void CState_GunReload::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    if (Is_MainAnimFinished())		// loop가 아닌데 애니메이션이 끝났다면
    {
        // 1. 총알을 업데이트 하고
        Reload_Gun();

        // 2. 키에 따른 state change

        // 2.1 만약 우클릭 하지 않았다면
        if (MOUSE_RBUTTON_UP)
        {
            GunEnd();
            return;
        }

        // 2.2 우클릭 -> attack
        else
        {
            GUN_START_DESC tStartDesc = {};
            tStartDesc.eMoveState = m_eMoveState;
            Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNATTACK), &tStartDesc);
        }

        return;
    }
}

HRESULT CState_GunReload::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Change_Weapon(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HOLD));

    return S_OK;
}

CState_GunReload* CState_GunReload::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_GunReload* pInstance = new CState_GunReload(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_GunReload::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_GunReload::Free()
{
    Super::Free();
}
