#include "pch.h"
#include "State_StartCatch.h"
#include "GameObject.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_StartCatch::CState_StartCatch(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "StartCatch", iStateIndex)
{
}

HRESULT CState_StartCatch::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_StartCatch::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_StartCatch::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_StartCatch::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_StartCatch::End()
{
	return Super::End();
}

CState_StartCatch* CState_StartCatch::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_StartCatch* pInstance = new CState_StartCatch(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_StartCatch::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_StartCatch::Free()
{
	Super::Free();
}
