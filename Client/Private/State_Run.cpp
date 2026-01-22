#include "Client_Defines.h"
#include "Player.h"
#include "MainPlayer.h"
#include "GameInstance.h"
#include "State_Run.h"

CState_Run::CState_Run(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Run")
{
}

HRESULT CState_Run::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Run::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Run::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;		

	return S_OK;
}

void CState_Run::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	m_fDuration += fTimeDelta;
	if (m_fDuration >= m_fInterval)
	{
		if (dynamic_cast<CMainPlayer*>(Get_OwnerObject()))
			CGameInstance::GetInstance()->Play_RandOneShot(L"Run", 0.1f, 5);
		m_fDuration -= m_fInterval;
	}

	if (Align_Attack(ENUM_TO_UINT(CPlayer::State::LEFTMELEE)))
		return;

	if (Align_Movement(fTimeDelta) == false)
		Request_Change_State(ENUM_TO_UINT(CPlayer::State::RUNEND));
}

HRESULT CState_Run::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	m_fDuration = 0.f;
	return S_OK;
}

CState_Run* CState_Run::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Run* pInstance = new CState_Run(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Run::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Run::Free()
{
	Super::Free();
}