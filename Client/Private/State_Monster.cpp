#include "pch.h"
#include "State_Monster.h"
#include "Monster_base.h"
#include "GameInstance.h"

CState_Monster::CState_Monster(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "State_Monster")
{
}

HRESULT CState_Monster::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Monster::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Monster::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_Monster::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_Monster::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_Monster* CState_Monster::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Monster* pInstance = new CState_Monster(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Monster::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Monster::Free()
{
	Super::Free();
}
