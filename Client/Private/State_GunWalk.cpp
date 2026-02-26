#include "pch.h"
#include "State_GunWalk.h"

#include "Player.h"
#include "Weapon.h"

CState_GunWalk::CState_GunWalk(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "GunWalk")
{
}

HRESULT CState_GunWalk::Initialize(void* pArg)
{
	PLAYER_STATE_SPECIFICDESC* pMyDesc = static_cast<PLAYER_STATE_SPECIFICDESC*>(pArg);

	PLAYER_STATEBASE_DESC tSuperDesc = {};
	tSuperDesc.bBlend = true;
	tSuperDesc.bLoop = true;
	tSuperDesc.pOwnerGun = pMyDesc->pOwnerGun;
	tSuperDesc.vecMainAnims = std::move(pMyDesc->vecMainAnims);

	tSuperDesc.FCollis = COLLISIONFLAGS::C_DOWN;
	tSuperDesc.FMoves = MOVEFLAGS::NORMAL;

	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END));

	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]			= ENUM_TO_UINT(CPlayer::State::GUNIDLE);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]			= ENUM_TO_UINT(CPlayer::State::JUMP);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]			= ENUM_TO_UINT(CPlayer::State::RUNSHORT);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]	= ENUM_TO_UINT(CPlayer::State::CROUCH);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]		= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]				= ENUM_TO_UINT(CPlayer::State::SKILL1);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]				= ENUM_TO_UINT(CPlayer::State::SKILL2);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]				= ENUM_TO_UINT(CPlayer::State::COMBO);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]				= ENUM_TO_UINT(CPlayer::State::GUNATTACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]			= ENUM_TO_UINT(CPlayer::State::CHARGE);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]		= ENUM_TO_UINT(CPlayer::State::END);

	tSuperDesc.vecChangeState_ByKey = vecChangeState_ByKey;


	if (FAILED(Super::Initialize(&tSuperDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GunWalk::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GunWalk::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Change_Weapon(CPlayer::Part::GUN, ENUM_TO_UINT(CWeapon::State::HAND));

	return S_OK;
}

void CState_GunWalk::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_GunWalk::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Change_Weapon(CPlayer::Part::GUN, ENUM_TO_UINT(CWeapon::State::HOLD));

	return S_OK;
}

CState_GunWalk* CState_GunWalk::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_GunWalk* pInstance = new CState_GunWalk(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GunWalk::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GunWalk::Free()
{
	Super::Free();
}
