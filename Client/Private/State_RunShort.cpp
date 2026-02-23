#include "pch.h"
#include "State_RunShort.h"

CState_RunShort::CState_RunShort(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "RunShort")
{
}

HRESULT CState_RunShort::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunShort::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunShort::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_ApplyYLerp(true);

	return S_OK;
}

void CState_RunShort::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_RunShort::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyYLerp(false);

	return S_OK;
}

CState_RunShort* CState_RunShort::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_RunShort* pInstance = new CState_RunShort(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_RunShort::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_RunShort::Free()
{
	Super::Free();
}
