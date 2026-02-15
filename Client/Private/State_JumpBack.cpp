#include "pch.h"
#include "State_JumpBack.h"

#include "Player.h"

CState_JumpBack::CState_JumpBack(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpBack")
{
}

HRESULT CState_JumpBack::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBack::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBack::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_ApplyGravity(false);

	return S_OK;
}

void CState_JumpBack::Update(const _float fTimeDelta)
{
	// 바닥 충돌 검사 후 change
	if (IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_DOWN))
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
		return;
	}

	Super::Update(fTimeDelta);
}

HRESULT CState_JumpBack::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyGravity(true);

	return S_OK;
}

void CState_JumpBack::OwnMove(const _float fTimeDelta)
{
	Move_Backward(fTimeDelta);
}

void CState_JumpBack::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		m_tNextStateDesc.iMainAnimIdx = 0;
	}
}


CState_JumpBack* CState_JumpBack::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpBack* pInstance = new CState_JumpBack(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpBack::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpBack::Free()
{
	Super::Free();
}
