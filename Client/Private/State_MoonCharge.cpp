#include "pch.h"
#include "State_MoonCharge.h"
#include "GameInstance.h"
CState_MoonCharge::CState_MoonCharge(CActionState* pOwnerComponent)
	: Super(pOwnerComponent)
{
}

HRESULT CState_MoonCharge::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	m_fHoldWeaponTime = 2.f;

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
	return S_OK;
}

void CState_MoonCharge::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_bShakeActived == false && m_fStateElapsed >= 1.f)
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
