#include "pch.h"
#include "State_GimmikRunStart.h"
#include "GameObject.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_GimmikRunStart::CState_GimmikRunStart(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "GimmikRunStart", iStateIndex)
{
}

HRESULT CState_GimmikRunStart::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GimmikRunStart::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GimmikRunStart::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_GimmikRunStart::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_GimmikRunStart::End()
{
	return Super::End();
}

CState_GimmikRunStart* CState_GimmikRunStart::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_GimmikRunStart* pInstance = new CState_GimmikRunStart(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GimmikRunStart::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GimmikRunStart::Free()
{
	Super::Free();
}
