#include "pch.h"
#include "State_BackdashCatch.h"
#include "GameObject.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "GameInstance.h"

CState_BackdashCatch::CState_BackdashCatch(CActionState* pOwnerComponent, _uint iStateIndex)
	: Super(pOwnerComponent, "BackDashCatch", iStateIndex)
{
}

HRESULT CState_BackdashCatch::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_BackdashCatch::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	CGameObject* pGo = Get_OwnerObject();
	m_pOwnerActionState = pGo->Get_Component<CMonsterActionState>();
	m_pOwnerControlContext = pGo->Get_Component<CMonsterControlContext>();
	return S_OK;
}

HRESULT CState_BackdashCatch::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_BackdashCatch::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	if ((m_bOnce == false) && Is_AnimTrackPositionBetweenRaw(10.f, 25.f))
	{
		m_bOnce = true;
		Set_ApplyGravity(false);
		Set_ZeroDeAccelRate();
	}
	
	if (Is_AnimFinished())
	{
		Change_MonsterState(m_umapState["EndCatch"]);
		return;
	}

	m_pOwnerControlContext->Update_TurnToTarget_XZ(fTimeDelta);

	_float fCurDistance = m_pOwnerControlContext->Get_Distance();

	if (fCurDistance >= (m_fDesiredCatchDistance - m_fDistanceTolerance))
	{
		Set_ZeroHorizontalVelocity();
	}
	else
	{
		_float fRemain = m_fDesiredCatchDistance - fCurDistance;
		_float fStep = m_fBackMovePerSec * fTimeDelta;
		if (fStep > fRemain)
			fStep = fRemain;

		m_pOwnerActionState->SetCCTImpuls_Conversion(Vec3(0.f, 0.f, -fStep));
	}
}

HRESULT CState_BackdashCatch::End()
{
	m_bOnce = false;
	Set_ApplyGravity(true);
	Reset_DeAccelRate();
	m_pOwnerActionState->SetCCTImpuls(Vec3::Zero);
	return Super::End();
}

CState_BackdashCatch* CState_BackdashCatch::Create(CActionState* pOwnerComponent, _uint iStateIndex, void* pArg)
{
	CState_BackdashCatch* pInstance = new CState_BackdashCatch(pOwnerComponent, iStateIndex);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_BackdashCatch::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_BackdashCatch::Free()
{
	Super::Free();
}
