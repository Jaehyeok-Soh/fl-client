#include "pch.h"
#include "State_RunShort.h"

#include "ControlContext.h"
#include "Player.h"

CState_RunShort::CState_RunShort(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "RunShort")
{
}

HRESULT CState_RunShort::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunShort::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunShort::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_ApplyYLerp(true);

	return S_OK;
}

void CState_RunShort::Update(const _float fTimeDelta)
{
	//// °­Á¦·Î state change
	//if (m_fStateElapsed >= 0.5f)
	//{
	//	Change_PlayerState(STATEKEY::LOOPDONE);
	//}

	Super::Update(fTimeDelta);
}

HRESULT CState_RunShort::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyYLerp(false);

	return S_OK;
}

void CState_RunShort::Change_PlayerState(STATEKEY eKey)
{
	if (eKey == STATEKEY::LOOPDONE)
	{
		if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DASH)))
		{
			Request_Change_State(ENUM_TO_UINT(CPlayer::State::RUNLOOP), &m_tNextStateDesc);
			return;
		}

		else if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
		{
			Request_Change_State(ENUM_TO_UINT(CPlayer::State::WALK), &m_tNextStateDesc);
			return;
		}

		else
		{
			Super::Change_PlayerState(eKey);
		}
	}

	else
	{
		Super::Change_PlayerState(eKey);
	}
}

CState_RunShort* CState_RunShort::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_RunShort* pInstance = new CState_RunShort(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_RunShort::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_RunShort::Free()
{
	Super::Free();
}
