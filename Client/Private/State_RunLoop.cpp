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

	return S_OK;
}

void CState_RunLoop::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	m_fDuration += fTimeDelta;
	if (m_fDuration >= m_fInterval)
	{
		if (dynamic_cast<CMainPlayer*>(Get_OwnerObject()))
			CGameInstance::GetInstance()->Play_RandOneShot(L"Run", 0.1f, 5);
		m_fDuration -= m_fInterval;
	}

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