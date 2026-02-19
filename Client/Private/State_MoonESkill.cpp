#include "pch.h"
#include "State_MoonESkill.h"

#include "Player.h"

CState_MoonESkill::CState_MoonESkill(CActionState* pOwnerComponent)
	:Super(pOwnerComponent, "Skill_E")
{
}

HRESULT CState_MoonESkill::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	m_iPlayerState = ENUM_TO_UINT(CPlayer::State::SKILL1);

	return S_OK;
}

HRESULT CState_MoonESkill::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonESkill::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_MoonESkill::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_MoonESkill::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_MoonESkill* CState_MoonESkill::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_MoonESkill* pInstance = new CState_MoonESkill(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_MoonESkill::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_MoonESkill::Free()
{
	__super::Free();
}
