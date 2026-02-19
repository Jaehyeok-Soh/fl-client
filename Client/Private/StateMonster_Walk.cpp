#include "pch.h"
#include "StateMonster_Walk.h"
#include "Monster_base.h"
#include "GameInstance.h"

CStateMonster_Walk::CStateMonster_Walk(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Walk")
{
}

HRESULT CStateMonster_Walk::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Walk::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Walk::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CStateMonster_Walk::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CStateMonster_Walk::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CStateMonster_Walk* CStateMonster_Walk::Create(CActionState* pOwnerComponent, void* pArg)
{
	CStateMonster_Walk* pInstance = new CStateMonster_Walk(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStateMonster_Walk::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateMonster_Walk::Free()
{
	Super::Free();
}
