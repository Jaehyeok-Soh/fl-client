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

	switch (m_iMainAnimIdx)
	{
	case static_cast<_int>(CPlayer::MELEE::SWORD):
		m_tKeyTimer.fMaxTime = 1.f;
		m_fHoldWeaponTime = 2.f;
		m_fCapHitMoveTime = Get_MoveBoneTime(69.f, 1.3f);
		break;

	case static_cast<_int>(CPlayer::MELEE::DUAL):
		m_tKeyTimer.fMaxTime = 55.f / ANIMTIC_3;
		m_fHoldWeaponTime = 100.f / ANIMTIC_3;
		m_fCapHitMoveTime = Get_MoveBoneTime(190.f, 1.3f);
		break;
	}

	return S_OK;
}

void CState_MoonCharge::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
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
