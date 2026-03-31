#include "pch.h"
#include "State_MoonSkill.h"

#include "Player.h"
#include "CameraMan_Targeter.h"

#define ANIMTIC (24.f * 1.2f)

CState_MoonSkill::CState_MoonSkill(CActionState* pOwnerComponent, const string& strName)
	:Super(pOwnerComponent, strName)
{
}

HRESULT CState_MoonSkill::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonSkill::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonSkill::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	switch (m_iPlayerState)
	{
	case ENUM_TO_UINT(CPlayer::State::SKILL1):
		// 처음에는 바닥 충돌 검사를 하지 않음
		m_FCollisions &= ~COLLISIONFLAGS::C_DOWN;
		Set_ApplyGravity(false);

		m_vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(CPlayer::State::END);

		break;

	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		//static_cast<CPlayer*>(Get_OwnerObject())->Change_CamState(ENUM_TO_UINT(Client::TargeterState::SKILL));
		m_vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(CPlayer::State::END);
		break;
	}

	return S_OK;
}

void CState_MoonSkill::Update(const _float fTimeDelta)
{
	_bool bChange = false;
	switch (m_iPlayerState)
	{
	case ENUM_TO_UINT(CPlayer::State::SKILL1):
		bChange = SkillE_Update(fTimeDelta);
		break;

	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		bChange = SkillQ_Update(fTimeDelta);
		break;
	}

	if (bChange)
		return;

	Super::Update(fTimeDelta);
}

HRESULT CState_MoonSkill::End()
{
 	if (FAILED(Super::End()))
		return E_FAIL;

	switch (m_iPlayerState)
	{
	case ENUM_TO_UINT(CPlayer::State::SKILL1):
		break;

	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		static_cast<CPlayer*>(Get_OwnerObject())->Change_CamState(ENUM_TO_UINT(Client::TargeterState::NORMAL));
		break;
	}

	Set_ApplyGravity(true);

	return S_OK;
}



_uint CState_MoonSkill::Get_Capabilities() const
{
	switch (m_iPlayerState)
	{
	case ENUM_TO_UINT(CPlayer::State::SKILL1):
		return	Super::Get_Capabilities();

	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		return	0;
	}

	return 0;
}

_bool CState_MoonSkill::Can_Captablity_Move() const
{
	return false;
}

_bool CState_MoonSkill::SkillE_Update(const _float fTimeDelta)
{
	SetupLook_CameraLookLerp(fTimeDelta, 10.f);

	if (m_fStateElapsed >= m_tKeyTimer.fMaxTime - (0.3f / 1.3f))
	{
		Set_ApplyGravity(true);
		m_FCollisions |= COLLISIONFLAGS::C_DOWN;
	}

	if (m_fStateElapsed >= (70.f / (ANIMTIC)))
	{
		{
			if (Check_MoveKey(fTimeDelta))
				return true;

			if (Check_JumpKey(fTimeDelta))
				return true;

			if (Check_DashKey(fTimeDelta))
				return true;

			if (Check_CtrlPressKey(fTimeDelta))
				return true;

			if (Check_CtrlUpKey(fTimeDelta))
				return true;

			if (Check_MeleeKey(fTimeDelta))
				return true;

			if (Check_RangeKey(fTimeDelta))
				return true;

			if (Check_SkillKey(fTimeDelta))
				return true;

			if (Check_FKey(fTimeDelta))
				return true;

			Change_PlayerState(STATEKEY::LOOPDONE);			// 다음 state로 change
			return true;
		}
	}

	return false;
}

_bool CState_MoonSkill::SkillQ_Update(const _float fTimeDelta)
{
	if (m_fStateElapsed >= (130.f / (ANIMTIC)))
	{
		{
			if (Check_MoveKey(fTimeDelta))
				return true;

			if (Check_JumpKey(fTimeDelta))
				return true;

			if (Check_DashKey(fTimeDelta))
				return true;

			if (Check_CtrlPressKey(fTimeDelta))
				return true;

			if (Check_CtrlUpKey(fTimeDelta))
				return true;

			if (Check_MeleeKey(fTimeDelta))
				return true;

			if (Check_RangeKey(fTimeDelta))
				return true;

			if (Check_SkillKey(fTimeDelta))
				return true;

			if (Check_FKey(fTimeDelta))
				return true;

			Change_PlayerState(STATEKEY::LOOPDONE);			// 다음 state로 change
			return true;
		}
	}

	if (m_fStateElapsed >= (1.5f / 1.2f))
	{
		static_cast<CPlayer*>(Get_OwnerObject())->Change_CamState(ENUM_TO_UINT(Client::TargeterState::NORMAL));
		return false;
	}

	if (m_fStateElapsed >= (0.35f / 1.2f))
	{
		static_cast<CPlayer*>(Get_OwnerObject())->Change_CamState(ENUM_TO_UINT(Client::TargeterState::SKILL_SEQUENCE));
	}

	return false;
}

CState_MoonSkill* CState_MoonSkill::Create(CActionState* pOwnerComponent, const string& strName, void* pArg)
{
	CState_MoonSkill* pInstance = new CState_MoonSkill(pOwnerComponent, strName);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_MoonSkill::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_MoonSkill::Free()
{
	__super::Free();
}
