#include "pch.h"
#include "State_DashFront.h"

CState_DashFront::CState_DashFront(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "DashFront")
{
}

HRESULT CState_DashFront::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DashFront::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_DashFront::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_DashFront::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_DashFront::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

void CState_DashFront::OwnMove(const _float fTimeDelta)
{
	Move_Front(fTimeDelta, m_fDashOffset);
}

CState_DashFront* CState_DashFront::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_DashFront* pInstance = new CState_DashFront(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_DashFront::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_DashFront::Free()
{
	Super::Free();
}
