#include "pch.h"
#include "State_JumpBack.h"

CState_JumpBack::CState_JumpBack(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpBack")
{
}

HRESULT CState_JumpBack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_JumpBack::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_JumpBack::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_JumpBack* CState_JumpBack::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpBack* pInstance = new CState_JumpBack(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpBack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpBack::Free()
{
	Super::Free();
}
