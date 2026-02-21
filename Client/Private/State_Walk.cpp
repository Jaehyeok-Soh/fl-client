#include "pch.h"
#include "State_Walk.h"
#include "Player.h"
#include "GameInstance.h"

//test
#include "ControlContext.h"

CState_Walk::CState_Walk(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Walk")
{
}

HRESULT CState_Walk::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Walk::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Walk::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	//Set_GravityOffset(8.f);
	//Set_ApplyYLerp(true);

	return S_OK;
}

void CState_Walk::Update(const _float fTimeDelta)
{
	/* walk pre ani 돌고 있을 때 */
	if (!Engine_Utils::Has_Flag(m_FAniFlags, STATEANI_FLAG::SA_PreAniDone))
	{
		CStateBase::Turn_byCam(fTimeDelta);

		if (Check_PreMoveKey())
			return;

		if (Super::Check_JumpKey(fTimeDelta))
			return;

		if (Super::Check_DashKey(fTimeDelta))
			return;

		if (Super::Check_CtrlPressKey(fTimeDelta))
			return;

		if (Super::Check_CtrlUpKey(fTimeDelta))
			return;
	}

	Super::Update(fTimeDelta);

	//if (Align_Movement(fTimeDelta) == false)	// 8방향 움직임 
	//{
	//	Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::MOVE)]); // 움직임이 없다면 실행할 
	//	return;
	//}

	//if(Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::DASH)))
	//{
	//	Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::SHIFT)]);
	//	return;
	//}

	//if(Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::SPECIALMV)))
	//{
	//	Request_Change_State(m_vecChangeState_ByKey[ENUM_TO_UINT(STATEKEY::LCRTL_PRESS)]);
	//	return;
	//}
}

HRESULT CState_Walk::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	//Set_ApplyGravity(true);
	Set_ApplyYLerp(false);
	Set_GravityOffset(0.f);

	return S_OK;
}

_bool CState_Walk::Check_PreMoveKey()
{
	/* pre ani일때 만약 wasd 키를 받지 않았다면 */
	if (!CStateBase::Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
	{
		Change_PlayerState(Super::STATEKEY::MOVE);
		return true;
	}

	return false;
}

CState_Walk* CState_Walk::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Walk* pInstance = new CState_Walk(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Walk::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Walk::Free()
{
	Super::Free();
}
