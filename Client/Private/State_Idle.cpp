#include "Client_Defines.h"
#include "GameInstance.h"
#include "Player.h"
#include "State_Idle.h"

CState_Idle::CState_Idle(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Idle")
{
}

HRESULT CState_Idle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

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
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_Idle::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (Align_Move(ENUM_TO_UINT(CPlayer::State::RUNSTART)))
		return;

	if (Align_Attack(ENUM_TO_UINT(CPlayer::State::LEFTMELEE)))
		return;
}

HRESULT CState_Idle::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

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
