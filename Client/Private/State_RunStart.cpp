#include "pch.h"
#include "State_RunStart.h"
#include "Player.h"
#include "MainPlayer.h"
#include "GameInstance.h"

CState_RunStart::CState_RunStart(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "RunStart")
{
}

HRESULT CState_RunStart::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunStart::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_RunStart::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	if (dynamic_cast<CMainPlayer*>(Get_OwnerObject()))
		CGameInstance::GetInstance()->Play_RandOneShot(L"MoveStart", 0.1f, 2);
	return S_OK;
}

void CState_RunStart::Update(const _float fTimeDelta)
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

	if (Is_AnimFinished())
	{
		Request_Change_State(ENUM_TO_UINT(CPlayer::State::RUN));
		return;
	}

	if (Align_Movement(fTimeDelta) == false)
	{
		Request_Change_State(ENUM_TO_UINT(CPlayer::State::RUNEND));
		return;
	}
}

HRESULT CState_RunStart::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	m_fDuration = 0.f;
	return S_OK;
}

CState_RunStart* CState_RunStart::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_RunStart* pInstance = new CState_RunStart(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_RunStart::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_RunStart::Free()
{
	Super::Free();
}