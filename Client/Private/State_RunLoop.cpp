#include "pch.h"
#include "State_RunLoop.h"
#include "Player.h"
#include "MainPlayer.h"
#include "GameInstance.h"

CState_RunLoop::CState_RunLoop(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "RunLoop")
{
}

HRESULT CState_RunLoop::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunLoop::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunLoop::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;	

	Set_ApplyYLerp(true);

	Look_Impuls();

	return S_OK;
}

void CState_RunLoop::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	//if (Align_Attack(ENUM_TO_UINT(CPlayer::State::LEFTMELEE)))
	//	return;

	
	//// move를 해라, 근데 움직임이 없다면 idle로 바꿔라
	//if (Align_Movement(fTimeDelta) == false)
	//	Request_Change_State(ENUM_TO_UINT(CPlayer::State::IDLE));
}

HRESULT CState_RunLoop::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	Set_ApplyYLerp(false);

	m_fDuration = 0.f;
	return S_OK;
}

CState_RunLoop* CState_RunLoop::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_RunLoop* pInstance = new CState_RunLoop(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_RunLoop::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_RunLoop::Free()
{
	Super::Free();
}