#include "pch.h"
#include "TriggerBox_GlobalEvent_BroadCaster.h"
#include "Transform.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CTriggerBox_GlobalEvent_BroadCaster::CTriggerBox_GlobalEvent_BroadCaster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTriggerBox(pDevice ,pContext) 
    , m_vecGlobalBroadcastType{}
{
    m_eTriggerBoxType = CTriggerBox::Type::GLOBALEVENT_BROADCASTER;
}

CTriggerBox_GlobalEvent_BroadCaster::CTriggerBox_GlobalEvent_BroadCaster(const CTriggerBox_GlobalEvent_BroadCaster& rhs)
    : CTriggerBox(rhs)
    , m_vecGlobalBroadcastType{ rhs.m_vecGlobalBroadcastType }
{
}

HRESULT CTriggerBox_GlobalEvent_BroadCaster::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CTriggerBox_GlobalEvent_BroadCaster::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;


    TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC* pDesc = static_cast<TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC*>(pArg);

    m_vecGlobalBroadcastType = pDesc->vecGlobalBroadcastType;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTriggerBox_GlobalEvent_BroadCaster::Ready_Component(TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC* pDesc)
{


    return S_OK;
}


HRESULT CTriggerBox_GlobalEvent_BroadCaster::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CTriggerBox_GlobalEvent_BroadCaster::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_GlobalEvent_BroadCaster::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);


    /* Player À§Ä¡¶û */
}

void CTriggerBox_GlobalEvent_BroadCaster::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CTriggerBox_GlobalEvent_BroadCaster::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTriggerBox_GlobalEvent_BroadCaster::Render()
{
    return S_OK;
}

HRESULT CTriggerBox_GlobalEvent_BroadCaster::BroadCast()
{
    for (auto& Type : m_vecGlobalBroadcastType)
    {
        m_pGameInstance->BroadCaset_RegisterGlobalEvent(ENUM_TO_UINT(Type));
    }

    m_isTriggerEventPlay = true;

    return S_OK;
}

void CTriggerBox_GlobalEvent_BroadCaster::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_GlobalEvent_BroadCaster::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CTriggerBox_GlobalEvent_BroadCaster::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_GlobalEvent_BroadCaster::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);

    if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
    {
        if(!m_isTriggerEventPlay)
            this->BroadCast();
    }

}

void CTriggerBox_GlobalEvent_BroadCaster::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

CTriggerBox_GlobalEvent_BroadCaster* CTriggerBox_GlobalEvent_BroadCaster::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTriggerBox_GlobalEvent_BroadCaster* pTriggerBox = new CTriggerBox_GlobalEvent_BroadCaster(pDevice ,pContext);

    if (FAILED(pTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Level Change Is Failed To Craete ");
        return nullptr;
    }

    return pTriggerBox;
}

CGameObject* CTriggerBox_GlobalEvent_BroadCaster::Clone(void* pArg)
{
    CTriggerBox_GlobalEvent_BroadCaster* pTriggerBox = new CTriggerBox_GlobalEvent_BroadCaster(*this);

    if (FAILED(pTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Level Change Is Failed To Clone ");
        return nullptr;
    }

    return pTriggerBox;
}

void CTriggerBox_GlobalEvent_BroadCaster::Free()
{
    Super::Free();

    return;
}

