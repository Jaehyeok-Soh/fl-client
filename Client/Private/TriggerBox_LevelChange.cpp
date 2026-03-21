#include "pch.h"
#include "TriggerBox_LevelChange.h"
#include "Transform.h"
#include "UI_Manager.h"
#include "Level_Loading.h"
#include "GameInstance.h"

CTriggerBox_LevelChange::CTriggerBox_LevelChange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTriggerBox(pDevice ,pContext) 
    , m_eChangeLevelType{ELevelType::END}
{
    m_eTriggerBoxType = CTriggerBox::Type::CHANGE_LEVEL;
}

CTriggerBox_LevelChange::CTriggerBox_LevelChange(const CTriggerBox_LevelChange& rhs)
    : CTriggerBox(rhs)
    , m_eChangeLevelType{rhs.m_eChangeLevelType }
{
}

HRESULT CTriggerBox_LevelChange::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;



    return S_OK;
}

HRESULT CTriggerBox_LevelChange::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;


    TRIGGERBOX_LEVELCHANGE_DESC* pDesc = static_cast<TRIGGERBOX_LEVELCHANGE_DESC*>(pArg);

    m_eChangeLevelType = pDesc->eChangeLevelType;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTriggerBox_LevelChange::Ready_Component(TRIGGERBOX_LEVELCHANGE_DESC* pDesc)
{


    return S_OK;
}


HRESULT CTriggerBox_LevelChange::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CTriggerBox_LevelChange::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_LevelChange::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);


    /* Player À§Ä¡¶û */
}

void CTriggerBox_LevelChange::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CTriggerBox_LevelChange::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTriggerBox_LevelChange::Render()
{
    return S_OK;
}

void CTriggerBox_LevelChange::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_LevelChange::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CTriggerBox_LevelChange::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    if (Super::IsEnabled() == false)
        return;

    Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_LevelChange::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    if (Super::IsEnabled() == false || m_bLockedEnter == false)
        return;

    if (Super::m_bHasQuest)
        m_bLockedEnter = true;

    Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);

    if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
    {
        m_pGameInstance->Request_ChangeLevel(ENUM_TO_UINT(ELevelType::LOADING), CLevel_Loading::Create(m_pDevice, m_pDeviceContext, m_eChangeLevelType));
        Set_Dead();
    }
}

void CTriggerBox_LevelChange::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    if (Super::IsEnabled() == false || m_bLockedExit == false)
        return;

    Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

    if (Super::m_bHasQuest)
    {
        SetEnable(false);
        Super::m_bLockedExit = true;
    }
}

void CTriggerBox_LevelChange::QuestEnter()
{
    Super::QuestEnter();
}

void CTriggerBox_LevelChange::QuestExit()
{
    Super::QuestExit();
}

CTriggerBox_LevelChange* CTriggerBox_LevelChange::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTriggerBox_LevelChange* pTriggerBox = new CTriggerBox_LevelChange(pDevice ,pContext);

    if (FAILED(pTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Level Change Is Failed To Craete ");
        return nullptr;
    }

    return pTriggerBox;
}

CGameObject* CTriggerBox_LevelChange::Clone(void* pArg)
{
    CTriggerBox_LevelChange* pTriggerBox = new CTriggerBox_LevelChange(*this);

    if (FAILED(pTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Level Change Is Failed To Clone ");
        return nullptr;
    }

    return pTriggerBox;
}

void CTriggerBox_LevelChange::Free()
{
    Super::Free();

    return;
}

