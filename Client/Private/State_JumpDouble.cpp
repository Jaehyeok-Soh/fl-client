#include "pch.h"
#include "State_JumpDouble.h"

#include "Player.h"

CState_JumpDouble::CState_JumpDouble(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpDouble")
{
}

HRESULT CState_JumpDouble::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpDouble::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpDouble::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	Set_ApplyGravity(false);

	return S_OK;
}

void CState_JumpDouble::Update(const _float fTimeDelta)
{
	// 바닥 충돌 검사 후 change
	if (m_fStateElapsed > 0.6f &&
		Check_OnGround(0.1f))
	{
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
		return;
	}

	Super::Update(fTimeDelta);
}

HRESULT CState_JumpDouble::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyGravity(true);

	return S_OK;
}

void CState_JumpDouble::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		m_tNextStateDesc.iMainAnimIdx = 0;
	}
}

CState_JumpDouble* CState_JumpDouble::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpDouble* pInstance = new CState_JumpDouble(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpDouble::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpDouble::Free()
{
	Super::Free();
}
