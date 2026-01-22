#include "Client_Defines.h"
#include "Player.h"
#include "GameInstance.h"
#include "State_RunEnd.h"

CState_RunEnd::CState_RunEnd(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "RunEnd")
{
}

HRESULT CState_RunEnd::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunEnd::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunEnd::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_RunEnd::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (Is_AnimFinished())
	{
		Request_Change_State(ENUM_TO_UINT(CPlayer::State::IDLE));
		return;
	}

	if (Align_Attack(ENUM_TO_UINT(CPlayer::State::LEFTMELEE)))
		return;

	if (Align_Move(ENUM_TO_UINT(CPlayer::State::RUNSTART)))
		return;
}

HRESULT CState_RunEnd::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

CState_RunEnd* CState_RunEnd::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_RunEnd* pInstance = new CState_RunEnd(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_RunEnd::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_RunEnd::Free()
{
	Super::Free();
}