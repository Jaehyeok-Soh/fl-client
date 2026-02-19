#include "pch.h"
#include "StateMonster_Idle.h"
#include "Monster_base.h"
#include "GameInstance.h"

CStateMonster_Idle::CStateMonster_Idle(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Idle")
{
}

HRESULT CStateMonster_Idle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Idle::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Idle::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CStateMonster_Idle::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CStateMonster_Idle::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CStateMonster_Idle* CStateMonster_Idle::Create(CActionState* pOwnerComponent, void* pArg)
{
	CStateMonster_Idle* pInstance = new CStateMonster_Idle(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStateMonster_Idle::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateMonster_Idle::Free()
{
	Super::Free();
}
