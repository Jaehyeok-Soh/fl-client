#include "pch.h"
#include "State_Fall.h"

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

	return S_OK;
}

void CState_Fall::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_Fall::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_Fall::OwnMove(const _float fTimeDelta)
{
	CStateBase::Move_Down(fTimeDelta);
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
