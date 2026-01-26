#include "pch.h"
#include "State_Crouch.h"

CState_Crouch::CState_Crouch(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Crouch")
{
}

HRESULT CState_Crouch::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Crouch::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Crouch::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_Crouch::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_Crouch::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_Crouch* CState_Crouch::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Crouch* pInstance = new CState_Crouch(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Crouch::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Crouch::Free()
{
	Super::Free();
}
