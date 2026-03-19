#include "pch.h"
#include "State_CrouchWalk.h"

#include "Player.h"

CState_CrouchWalk::CState_CrouchWalk(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "CrouchWalk")
{
}

HRESULT CState_CrouchWalk::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_CrouchWalk::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_CrouchWalk::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	//Set_ApplyYLerp(true);

	return S_OK;
}

void CState_CrouchWalk::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_CrouchWalk::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyYLerp(false);

	return S_OK;
}

void CState_CrouchWalk::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::COMBO):
		m_tNextStateDesc.iMainAnimIdx = 1;
		break;

	default:
		Super::Set_NextStateDesc(iNextState);
	}
}

CState_CrouchWalk* CState_CrouchWalk::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_CrouchWalk* pInstance = new CState_CrouchWalk(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_CrouchWalk::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_CrouchWalk::Free()
{
	Super::Free();
}
