#include "pch.h"
#include "StateMonster_Attack.h"
#include "Monster_base.h"
#include "GameInstance.h"

CStateMonster_Attack::CStateMonster_Attack(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Attack")
{
}

HRESULT CStateMonster_Attack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Attack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CStateMonster_Attack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CStateMonster_Attack::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CStateMonster_Attack::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CStateMonster_Attack* CStateMonster_Attack::Create(CActionState* pOwnerComponent, void* pArg)
{
	CStateMonster_Attack* pInstance = new CStateMonster_Attack(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CStateMonster_Attack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateMonster_Attack::Free()
{
	Super::Free();
}
