#include "pch.h"
#include "State_Land.h"

CState_Land::CState_Land(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Land")
{
}

HRESULT CState_Land::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Land::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Land::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_DoubleJump(false);

	return S_OK;
}

void CState_Land::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_Land::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_Land* CState_Land::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Land* pInstance = new CState_Land(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Land::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Land::Free()
{
	Super::Free();
}
