#include "pch.h"
#include "State_GunBase.h"

CState_GunBase::CState_GunBase(CActionState* pOwnerComponent, const string& strName)
    : Super(pOwnerComponent, strName)
{
}

HRESULT CState_GunBase::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_GunBase::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_GunBase::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    return S_OK;
}

void CState_GunBase::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

HRESULT CState_GunBase::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    return S_OK;
}

void CState_GunBase::Free()
{
    Super::Free();
}
