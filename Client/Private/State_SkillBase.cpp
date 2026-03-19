#include "pch.h"
#include "State_SkillBase.h"

#include "Player.h"
#include "Weapon.h"

CState_SkillBase::CState_SkillBase(CActionState* pOwnerComponent, const string& strName)
	: Super(pOwnerComponent, strName)
{
}

HRESULT CState_SkillBase::Initialize(void* pArg)
{
	Skill_DESC* pDesc = static_cast<Skill_DESC*>(pArg);

	m_iPlayerState = pDesc->iPlayerState;

	PLAYER_STATEBASE_DESC tMyDesc = {};

	tMyDesc.pOwnerGun = pDesc->pOwnerGun;

	tMyDesc.bBlend = true;
	tMyDesc.bLoop = false;
	tMyDesc.FAniFlags = 0;
	tMyDesc.FMoves = MOVEFLAGS::PRESS_CHANGE | MOVEFLAGS::LOOP_DONE;
	tMyDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Strong
					| CStateBase_Player::COLLISIONFLAGS::C_Fly;

	TIME_COUNTER tKey = {};
	if (pDesc->bKeyInput)
	{
		tKey.bCountTime = true;
		tKey.bTimeReset = false;
		tKey.fMaxTime = pDesc->fKeyCoolTime;
	}
	else
	{
		tKey.bCountTime = false;
	}
	tMyDesc.tKeyTimer = tKey;

	//MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, CHARGE, LOOPDONE
	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END));

	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]			= ENUM_TO_UINT(CPlayer::State::WALK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]			= ENUM_TO_UINT(CPlayer::State::JUMP);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]			= ENUM_TO_UINT(CPlayer::State::DASHBACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]	= ENUM_TO_UINT(CPlayer::State::CROUCH);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]		= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]				= ENUM_TO_UINT(CPlayer::State::SKILL1);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]				= ENUM_TO_UINT(CPlayer::State::SKILL2);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]				= ENUM_TO_UINT(CPlayer::State::END);
	//vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]				= ENUM_TO_UINT(CPlayer::State::GUN);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]			= ENUM_TO_UINT(CPlayer::State::CHARGE);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]		= ENUM_TO_UINT(CPlayer::State::IDLE);

	tMyDesc.vecChangeState_ByKey = vecChangeState_ByKey;
	tMyDesc.vecMainAnims = { pDesc->iAnimIdx };

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_SkillBase::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_SkillBase::Start(void* pArg, _bool bForce)
{
	if (FAILED(Start_AttackState(pArg)))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL), ENUM_TO_UINT(CWeapon::State::HAND));

	return S_OK;
}

void CState_SkillBase::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_fStateElapsed >= m_tKeyTimer.fMaxTime + 0.6f)
	{
		Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL), ENUM_TO_UINT(CWeapon::State::NONE));
	}
}

HRESULT CState_SkillBase::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Change_WeaponState(ENUM_TO_UINT(CPlayer::EWEAPON::SKILL), ENUM_TO_UINT(CWeapon::State::NONE));

	return S_OK;
}

void CState_SkillBase::Free()
{
	Super::Free();
}
