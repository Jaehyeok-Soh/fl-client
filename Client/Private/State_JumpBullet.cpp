#include "pch.h"
#include "State_JumpBullet.h"

#include "Player.h"

CState_JumpBullet::CState_JumpBullet(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "JumpBullet")
{
}

HRESULT CState_JumpBullet::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBullet::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_JumpBullet::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	CStateBase::SetupLook_CameraLook();

	return S_OK;
}

void CState_JumpBullet::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_JumpBullet::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_JumpBullet::Set_NextStateDesc(_uint iNextState)
{
	switch (iNextState)
	{
	case ENUM_TO_UINT(CPlayer::State::SLIDE):
		m_tNextStateDesc.iMainAnimIdx = 1; break; // skyslide

	default:
		m_tNextStateDesc.iMainAnimIdx = 0;
	}
}


CState_JumpBullet* CState_JumpBullet::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_JumpBullet* pInstance = new CState_JumpBullet(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_JumpBullet::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_JumpBullet::Free()
{
	Super::Free();
}
