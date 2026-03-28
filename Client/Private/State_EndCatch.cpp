#include "pch.h"
#include "State_EndCatch.h"
#include "GameObject.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_EndCatch::CState_EndCatch(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "EndCatch", iStateIndex)
{
}

HRESULT CState_EndCatch::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	CGameObject* pGo = Get_OwnerObject();
	m_pOwnerActionState = pGo->Get_Component<CMonsterActionState>();
	m_pOwnerControlContext = pGo->Get_Component<CMonsterControlContext>();
	return S_OK;
}

HRESULT CState_EndCatch::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_EndCatch::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_EndCatch::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if (Is_AnimFinished())
	{
		Change_MonsterState(m_umapState["Idle"]);
		return;
	}
	if (Is_AnimTrackPositionBetweenRaw(27.f, 37.f))
	{
		Set_ApplyGravity(false);
		Set_ZeroDeAccelRate();
	}
	else if (Is_AnimTrackPositionBetweenRaw(54.f, 65.f))
	{
		Set_ApplyGravity(true);
		Reset_DeAccelRate();
	}
}

HRESULT CState_EndCatch::End()
{
	Set_ApplyGravity(true);
	Reset_DeAccelRate();
	return Super::End();
}

CState_EndCatch* CState_EndCatch::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_EndCatch* pInstance = new CState_EndCatch(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_EndCatch::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_EndCatch::Free()
{
	Super::Free();
}
