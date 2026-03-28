#include "pch.h"
#include "State_GimmikCamera.h"
#include "GameObject.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_GimmikCamera::CState_GimmikCamera(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "GimmikCamera", iStateIndex)
{
}

HRESULT CState_GimmikCamera::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GimmikCamera::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_GimmikCamera::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_fElapsed = 0.f;
	return S_OK;
}

void CState_GimmikCamera::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (Is_AnimFinished())
	{
		m_fElapsed += fTimeDelta;
		if (m_fElapsed >= m_fHoldTime)
		{
			Change_MonsterState(m_umapState["GimmikRunLoop"]);
		}
	}
}

HRESULT CState_GimmikCamera::End()
{
	return Super::End();
}

CState_GimmikCamera* CState_GimmikCamera::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_GimmikCamera* pInstance = new CState_GimmikCamera(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_GimmikCamera::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_GimmikCamera::Free()
{
	Super::Free();
}
