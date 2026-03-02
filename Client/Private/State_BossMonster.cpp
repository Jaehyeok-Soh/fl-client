#include "pch.h"
#include "State_BossMonster.h"
#include "Monster_base.h"
#include "GameInstance.h"

CState_BossMonster::CState_BossMonster(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "State_BossMonster", iStateIndex)
{
}

HRESULT CState_BossMonster::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_BossMonster::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_BossMonster::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_BossMonster::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_BossMonster::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_BossMonster* CState_BossMonster::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_BossMonster* pInstance = new CState_BossMonster(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_BossMonster::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_BossMonster::Free()
{
	Super::Free();
}
