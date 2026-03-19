#include "pch.h"
#include "State_MoonCharge.h"

#include "Player.h"

#include "GameInstance.h"
CState_MoonCharge::CState_MoonCharge(CActionState* pOwnerComponent)
	: Super(pOwnerComponent)
{
}

HRESULT CState_MoonCharge::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonCharge::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_MoonCharge::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_bShakeActived = false;
	m_bLookMonsterYet = true;

	switch (m_iMainAnimIdx)
	{
	case static_cast<_int>(CPlayer::MELEE::SWORD):
		m_tKeyTimer.fMaxTime = 1.f;
		m_fHoldWeaponTime = 2.f;
		break;

	case static_cast<_int>(CPlayer::MELEE::DUAL):
		m_tKeyTimer.fMaxTime = 55.f / ANIMTIC;
		m_fHoldWeaponTime = 100.f / ANIMTIC;
		break;
	}

	return S_OK;
}

void CState_MoonCharge::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_bLookMonsterYet && Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
	{
		LookAt_Monser();
		m_bLookMonsterYet = false;
	}

	if (m_bShakeActived == false && m_fStateElapsed >= (1.f / 1.2f))
	{
		CAM_SHAKING_DATA data{};
		data.fTime = 0.2f;
		data.fPower = 0.3f;
		CGameInstance::GetInstance()->Camera_Shaking(data);
		m_bShakeActived = true;
	}
}

HRESULT CState_MoonCharge::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_MoonCharge* CState_MoonCharge::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_MoonCharge* pInstance = new CState_MoonCharge(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_MoonCharge::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_MoonCharge::Free()
{
	Super::Free();
}
