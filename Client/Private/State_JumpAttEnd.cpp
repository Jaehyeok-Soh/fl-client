#include "pch.h"
#include "State_JumpAttEnd.h"

#include "Player.h"
#include "Weapon.h"

#include "GameInstance.h"

CState_JumpAttEnd::CState_JumpAttEnd(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpAttEnd")
{
}

HRESULT CState_JumpAttEnd::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpAttEnd::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpAttEnd::Start(void* pArg, _bool bForce)
{
	if (FAILED(Start_AttackState(pArg)))
		return E_FAIL;

	Start_Att(ENUM_TO_UINT(CPlayer::State::JUMPATTEND));

	switch (m_iMainAnimIdx)
	{
	case 0:
		m_fCapHitMoveTime = Get_MoveBoneTime(68.f, 1.3f);
		break;

	case 1:
		m_fCapHitMoveTime = Get_MoveBoneTime(157.f, 1.3f);
		break;
	}

	return S_OK;
}

void CState_JumpAttEnd::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_fStateElapsed >= (2.3f / 1.3f))
	{
		Change_Weapon();
		Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HOLD));
	}
}

HRESULT CState_JumpAttEnd::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	End_Att(ENUM_TO_UINT(CPlayer::State::JUMPATTEND));

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

CState_JumpAttEnd* CState_JumpAttEnd::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpAttEnd* pInstance = new CState_JumpAttEnd(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpAttEnd::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpAttEnd::Free()
{
	Super::Free();
}
