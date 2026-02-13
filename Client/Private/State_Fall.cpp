#include "pch.h"
#include "State_Fall.h"

#include "Player.h"

CState_Fall::CState_Fall(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "Fall")
{
}

HRESULT CState_Fall::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Fall::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_Fall::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	m_fFallTimeAcc = 0.f;

	return S_OK;
}

void CState_Fall::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Count_FallTime(fTimeDelta);

	if (IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_DOWN))
		Change_PlayerState(ENUM_TO_UINT(CPlayer::State::LAND));
}

HRESULT CState_Fall::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	m_fFallTimeAcc = 0.f;

	return S_OK;
}

void CState_Fall::Count_FallTime(const _float fTimeDelta)
{
	m_fFallTimeAcc += fTimeDelta;
}

void CState_Fall::OwnMove(const _float fTimeDelta)
{
	CStateBase::Move_Down(fTimeDelta);
}

void CState_Fall::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::LAND):
		if(3.f <= m_fFallTimeAcc) // 3초 이상 떨어졌다면
			m_tNextStateDesc.iMainAnimIdx = 1; break; // LAND_Heavy
	default:
		m_tNextStateDesc.iMainAnimIdx = 0;
	}
}

CState_Fall* CState_Fall::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_Fall* pInstance = new CState_Fall(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_Fall::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_Fall::Free()
{
	Super::Free();
}
