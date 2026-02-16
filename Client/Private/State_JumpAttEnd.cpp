#include "pch.h"
#include "State_JumpAttEnd.h"

#include "Player.h"
#include "Weapon.h"

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
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_JumpAttEnd::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_fStateElapsed >= m_tKeyTimer.fMaxTime)
	{
		Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HOLD));
	}
}

HRESULT CState_JumpAttEnd::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Change_Weapon(CPlayer::Part::SWORD, ENUM_TO_UINT(CWeapon::State::HOLD));

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
