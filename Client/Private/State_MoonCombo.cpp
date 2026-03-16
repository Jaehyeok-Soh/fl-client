#include "pch.h"
#include "State_MoonCombo.h"

#include "Player.h"
#include "PlayerActionState.h"

#include "GameInstance.h"
CState_MoonCombo::CState_MoonCombo(CActionState* pOwnerComponent)
	: Super(pOwnerComponent,"MoonCombo")
{
}

HRESULT CState_MoonCombo::Initialize(void* pArg)
{
	MOONCOMBO_DESC* pDesc = static_cast<MOONCOMBO_DESC*>(pArg);

	PLAYER_COMBOBASE_DESC tMyDesc = {};
	tMyDesc.vCombo_CheckTimes = pDesc->vCombo_CheckTimes;
	tMyDesc.pOwnerGun = pDesc->pOwnerGun;

	tMyDesc.arrCombo_EndTimes = pDesc->arrCombo_EndTimes;
	tMyDesc.fSlide_CheckTime = pDesc->fSlide_CheckTime;

	tMyDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
		| CStateBase_Player::COLLISIONFLAGS::C_Strong
		| CStateBase_Player::COLLISIONFLAGS::C_Fly;

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

	tMyDesc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), pDesc->iEndStateIndex);

	//MOVE, SPACE, SHIFT, LCRTL_PRESS, LCRTL_UP, Q, E, LM, RM, CHARGE, LOOPDONE
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]			= ENUM_TO_UINT(CPlayer::State::WALK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]			= ENUM_TO_UINT(CPlayer::State::JUMP);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]			= ENUM_TO_UINT(CPlayer::State::DASHBACK);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]	= ENUM_TO_UINT(CPlayer::State::CROUCH);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]		= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]				= ENUM_TO_UINT(CPlayer::State::SKILL1);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]				= ENUM_TO_UINT(CPlayer::State::SKILL2);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]				= ENUM_TO_UINT(CPlayer::State::END);
	vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]				= ENUM_TO_UINT(CPlayer::State::GUNATTACK);
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

	m_bShakeActived = false;

	return S_OK;
}

void CState_MoonCombo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_iComboCount == 4)
	{
		// 속도 조절 필요
		if (m_bShakeActived  == false && m_fStateElapsed >= 0.8f / 1.2f)
		{
			CAM_SHAKING_DATA data{};
			data.fTime = 0.2f;
			data.fPower = 0.3f;
			CGameInstance::GetInstance()->Camera_Shaking(data);
			m_bShakeActived = true;
		}
	}
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
