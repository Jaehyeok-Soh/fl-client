#include "pch.h"
#include "State_MoonCombo.h"

#include "Player.h"

CState_MoonCombo::CState_MoonCombo(CActionState* pOwnerComponent)
	: Super(pOwnerComponent,"MoonCombo")
{
}

HRESULT CState_MoonCombo::Initialize(void* pArg)
{
	MOONCOMBO_DESC* pDesc = static_cast<MOONCOMBO_DESC*>(pArg);

	PLAYER_COMBOBASE_DESC tMyDesc = {};
	tMyDesc.vCombo_CheckTimes = pDesc->vCombo_CheckTimes;

	tMyDesc.bBlend = true;
	tMyDesc.bLoop = false;
	tMyDesc.FAniFlags = 0;
	tMyDesc.vecPreAnims = {};
	tMyDesc.vecMainAnims = { pDesc->iFirstAnimIdx, pDesc->iSlideAnimIdx,pDesc->iSecondAnimIdx, pDesc->iThirdAnimIdx, pDesc->iFourthAnimIdx };

	TIME_COUNTER tKeyTimer	= {};
	tKeyTimer.bCountTime	= true;
	tKeyTimer.bTimeReset	= false;
	tKeyTimer.fMaxTime		= 1.5f;
	tMyDesc.tKeyTimer		= tKeyTimer;

	tMyDesc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), pDesc->iEndStateIndex);

	//MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, CHARGE, LOOPDONE
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]			= ENUM_TO_UINT(CPlayer::State::WALK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]			= ENUM_TO_UINT(CPlayer::State::JUMP);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]			= ENUM_TO_UINT(CPlayer::State::DASHBACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]	= ENUM_TO_UINT(CPlayer::State::CROUCH);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]				= ENUM_TO_UINT(CPlayer::State::SKILL1);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]				= ENUM_TO_UINT(CPlayer::State::SKILL2);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]				= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]				= ENUM_TO_UINT(CPlayer::State::GUN);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]			= ENUM_TO_UINT(CPlayer::State::CHARGE);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]		= ENUM_TO_UINT(CPlayer::State::IDLE);

	tMyDesc.vecChangeState_ByKey = vecChangeState_ByKey;

	if (FAILED(Super::Initialize(&tMyDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonCombo::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonCombo::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_MoonCombo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_MoonCombo::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_MoonCombo* CState_MoonCombo::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_MoonCombo* pInstance = new CState_MoonCombo(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_MoonCombo::Create, Failed");
		Safe_Release(pInstance);
	} 
	return pInstance;
}

void CState_MoonCombo::Free()
{
	Super::Free();
}
