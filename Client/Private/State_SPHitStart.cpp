#include "pch.h"
#include "PhysicsCCT.h"
#include "GameObject.h"
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
    if (CPhysicsCCT* cct = Get_OwnerObject()->Get_Component<CPhysicsCCT>())
    {
        cct->SetHeight(0.1f);
    }
    m_bOnce = false;
    return S_OK;
}

void CState_SPHitStart::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
    if (m_bOnce == false && Is_AnimTrackPositionBetweenRaw(55.f, 65.f))
    {
        m_bOnce = true;
        Set_ApplyYLerp(false);
        Set_ApplyGravity(true);
    }
}

HRESULT CState_SPHitStart::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Set_ApplyYLerp(false);
    Set_ApplyGravity(true);

    CPhysicsCCT* cct = { nullptr };
    if (cct = Get_OwnerObject()->Get_Component<CPhysicsCCT>())
    {
        cct->SetHeight(0.7f); // todo_eunbi : player cct height °ª ¹Ù²î¸é ¿©±âµµ ¹Ù²ãÁà¾ß ÇÔ
    }
    // hp ±ð¾Æ¾ß ÇÏÁö ¾Ê³ª..?

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
