#include "pch.h"
#include "State_SPHitStart.h"

CState_SPHitStart::CState_SPHitStart(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "SpHit_Start")
{
}

HRESULT CState_SPHitStart::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_SPHitStart::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_SPHitStart::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    Set_ApplyYLerp(true);
    Set_ApplyGravity(false);
    Set_ZeroVerticalVelocity();

    return S_OK;
}

void CState_SPHitStart::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

HRESULT CState_SPHitStart::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Set_ApplyYLerp(false);
    Set_ApplyGravity(true);
    // hp 깎아야 하지 않나..?

    return S_OK;
}

_bool CState_SPHitStart::Can_CheckKey(const _float fTimeDelta)
{
    return Is_MainAnimFinished();
}

CState_SPHitStart* CState_SPHitStart::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_SPHitStart* pInstance = new CState_SPHitStart(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_SPHitStart::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_SPHitStart::Free()
{
    Super::Free();
}
