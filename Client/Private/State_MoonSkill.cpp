#include "pch.h"
#include "State_MoonSkill.h"

#include "Player.h"

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



	// 처음에는 바닥 충돌 검사를 하지 않음
	m_FCollisions &= ~COLLISIONFLAGS::C_DOWN;

	return S_OK;
}

void CState_MoonSkill::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	switch (m_iPlayerState)
	{
	case ENUM_TO_UINT(CPlayer::State::SKILL1):
		SkillE_Update(fTimeDelta);
		break;

	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		SkillQ_Update(fTimeDelta);
		break;
	}
}

HRESULT CState_MoonSkill::End()
{
 	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyGravity(true);

	return S_OK;
}

_uint CState_MoonSkill::Get_Capabilities() const
{
	switch (m_iPlayerState)
	{
	case ENUM_TO_UINT(CPlayer::State::SKILL1):
		return	ENUM_TO_UINT(Engine::StateCapability::BEATTACKED);

	case ENUM_TO_UINT(CPlayer::State::SKILL2):
		return	0.f;
	}
}

void CState_MoonSkill::SkillE_Update(const _float fTimeDelta)
{
	if (m_fStateElapsed >= m_tKeyTimer.fMaxTime - 0.3f)
	{
		m_FCollisions |= COLLISIONFLAGS::C_DOWN;
	}

	if (m_fStateElapsed >= 2.5f)
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::IDLE));
	}
}

void CState_MoonSkill::SkillQ_Update(const _float fTimeDelta)
{
	if (m_fStateElapsed >= 5.f)
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::IDLE));
	}
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
