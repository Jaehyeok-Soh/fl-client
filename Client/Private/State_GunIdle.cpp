#include "pch.h"
#include "State_GunIdle.h"

#include "Player.h"
#include "Weapon.h"
#include "PlayerActionState.h"

CState_GunIdle::CState_GunIdle(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "GunIdle")
{
}

HRESULT CState_GunIdle::Initialize(void* pArg)
{
	PLAYER_STATE_SPECIFICDESC* pMyDesc = static_cast<PLAYER_STATE_SPECIFICDESC*>(pArg);

	PLAYER_STATEBASE_DESC tSuperDesc = {};
	tSuperDesc.bBlend = true;
	tSuperDesc.bLoop = true;
	tSuperDesc.pOwnerGun = pMyDesc->pOwnerGun;
	tSuperDesc.vecMainAnims = pMyDesc->vecMainAnims;

	tSuperDesc.FCollis = COLLISIONFLAGS::C_DOWN
		| CStateBase_Player::COLLISIONFLAGS::C_Strong
		| CStateBase_Player::COLLISIONFLAGS::C_Fly
		| CStateBase_Player::COLLISIONFLAGS::C_CheckF;

	tSuperDesc.FMoves = MOVEFLAGS::PRESS_CHANGE;

	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END));

	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]			= ENUM_TO_UINT(CPlayer::State::GUNWALK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]			= ENUM_TO_UINT(CPlayer::State::JUMP);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]			= ENUM_TO_UINT(CPlayer::State::DASHBACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]	= ENUM_TO_UINT(CPlayer::State::CROUCH);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]				= ENUM_TO_UINT(CPlayer::State::SKILL1);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]				= ENUM_TO_UINT(CPlayer::State::SKILL2);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]				= ENUM_TO_UINT(CPlayer::State::COMBO);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]				= ENUM_TO_UINT(CPlayer::State::GUNATTACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]			= ENUM_TO_UINT(CPlayer::State::CHARGE);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]		= ENUM_TO_UINT(CPlayer::State::END);

	tSuperDesc.vecChangeState_ByKey = vecChangeState_ByKey;

	tSuperDesc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

	if (FAILED(Super::Initialize(&tSuperDesc)))
		return E_FAIL;

	m_iBoneHitTypeFlag = CPlayerActionState::BoneHitType::BHT_FORCE_WEAK;

	return S_OK;
}

HRESULT CState_GunIdle::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GunIdle::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HAND));

	return S_OK;
}

void CState_GunIdle::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_GunIdle::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::RANGE), ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

CState_GunIdle* CState_GunIdle::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_GunIdle* pInstance = new CState_GunIdle(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GunIdle::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GunIdle::Free()
{
	Super::Free();
}
