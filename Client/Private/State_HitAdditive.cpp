#include "pch.h"
#include "State_HitAdditive.h"

CState_HitAdditive::CState_HitAdditive(CActionState* pOwnerComponent)
	: Super(pOwnerComponent, "HitAdditive")
{
}

HRESULT CState_HitAdditive::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitAdditive::Awake(const _uint iLevelIndex)
{
	if (FAILED(Super::Awake(iLevelIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CState_HitAdditive::Start(void* pArg, _bool bForce)
{
	if (FAILED(Super::Start(pArg, bForce)))
		return E_FAIL;

	return S_OK;
}

void CState_HitAdditive::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CState_HitAdditive::End()
{
	if (FAILED(Super::End()))
		return E_FAIL;

	return S_OK;
}

_bool CState_HitAdditive::Can_Captablity_Move() const
{
	return false;
}

CState_HitAdditive* CState_HitAdditive::Create(CActionState* pOwnerComponent, void* pArg)
{
	CState_HitAdditive* pInstance = new CState_HitAdditive(pOwnerComponent);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CState_HitAdditive::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CState_HitAdditive::Free()
{
	Super::Free();
}
