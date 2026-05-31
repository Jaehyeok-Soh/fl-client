#include "pch.h"
#include "State_HitFlyEnd.h"

CState_HitFlyEnd::CState_HitFlyEnd(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "HitFlyEnd")
{
}

HRESULT CState_HitFlyEnd::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitFlyEnd::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitFlyEnd::Start(void* pArg, _bool bForce)
{
	if (IsRemotePlayer())
	{
		if (FAILED(Super::Start(pArg, bForce)))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_HitFlyEnd::Update(const _float fTimeDelta)
{
	if (IsRemotePlayer())
	{
		Super::Update(fTimeDelta);
		return;
	}

	Super::Update(fTimeDelta);
}

HRESULT CState_HitFlyEnd::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

_bool CState_HitFlyEnd::Can_Captablity_Move() const
{
	return false;
}

CState_HitFlyEnd* CState_HitFlyEnd::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_HitFlyEnd* pInstance = new CState_HitFlyEnd(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_HitFlyEnd::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_HitFlyEnd::Free()
{
	Super::Free();
}
