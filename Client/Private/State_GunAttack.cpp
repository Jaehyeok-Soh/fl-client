#include "pch.h"
#include "State_GunAttack.h"

#include "Player.h"
#include "ControlContext.h"
#include "Weapon.h"

#include "GameInstance.h"

CState_GunAttack::CState_GunAttack(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "GunAttack")
{
}

HRESULT CState_GunAttack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GunAttack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GunAttack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HAND));

	Set_GunTimer(true);

	Start_Att(ENUM_TO_UINT(CPlayer::State::GUNATTACK));

	return S_OK;
}

void CState_GunAttack::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// 만약 쏠 수 없다면?
	if (!Can_Fire())
	{
		GUN_START_DESC tStartDesc = {};
		tStartDesc.eMoveState = m_eMoveState;

		// reload 할 수 있는지 검사 후 변경
		if (Can_Reload())
		{
			Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNRELOAD),&tStartDesc);
			return;
		}

		// 아니라면 move key 체크 후 change state
		else if(Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
		{
			Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNWALK));
			return;
		}

		else
		{
			Request_Change_State(ENUM_TO_UINT(CPlayer::State::GUNIDLE));
			return;
		}
	}
}

HRESULT CState_GunAttack::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HOLD));
	Reset_GunTimer();

	return S_OK;
}

CState_GunAttack* CState_GunAttack::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_GunAttack* pInstance = new CState_GunAttack(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GunAttack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GunAttack::Free()
{
	Super::Free();
}
