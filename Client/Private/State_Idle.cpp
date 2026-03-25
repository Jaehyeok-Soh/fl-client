#include "pch.h"
#include "State_Idle.h"

#include "Player.h"
#include "ControlContext.h"
#include "PlayerActionState.h"

#include "GameInstance.h"

CState_Idle::CState_Idle(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Idle")
{
}

HRESULT CState_Idle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	m_iBoneHitTypeFlag = CPlayerActionState::BoneHitType::BHT_FORCE_WEAK;

	return S_OK;
}

HRESULT CState_Idle::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Idle::Start(void *pArg, _bool bForce)
{
	if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
	{
		Change_PlayerState(STATEKEY::MOVE);
		return S_OK;
	}

	Set_ZeroVerticalVelocity();
	Set_YLerp(false);

	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_Idle::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_Idle::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_YLerp(true);

	return S_OK;
}

CState_Idle* CState_Idle::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Idle* pInstance = new CState_Idle(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Idle::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Idle::Free()
{
	Super::Free();
}
