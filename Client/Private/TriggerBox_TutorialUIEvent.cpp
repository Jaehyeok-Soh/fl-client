#include "pch.h"
#include "TriggerBox_TutorialUIEvent.h"

#include "GameInstance.h"
#include "QuestManager.h"

CTriggerBox_TutorialUIEvent::CTriggerBox_TutorialUIEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerBox(pDevice, pContext), m_eUITutorialPopUpTypeID{EUITutorialPopUpTypeID::END}
{
}

CTriggerBox_TutorialUIEvent::CTriggerBox_TutorialUIEvent(const CTriggerBox_TutorialUIEvent& rhs)
	: CTriggerBox(rhs), m_eUITutorialPopUpTypeID{rhs.m_eUITutorialPopUpTypeID }
{
}

HRESULT CTriggerBox_TutorialUIEvent::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT);

	return S_OK;
}

HRESULT CTriggerBox_TutorialUIEvent::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	TRIGGERBOX_TUTORIALUIEVENT_DESC* pDesc = static_cast<TRIGGERBOX_TUTORIALUIEVENT_DESC*>(pArg);
	m_eUITutorialPopUpTypeID =  pDesc->eType;

	if (m_eUITutorialPopUpTypeID == EUITutorialPopUpTypeID::END) return E_FAIL;

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CTriggerBox_TutorialUIEvent::Ready_Component(TRIGGERBOX_TUTORIALUIEVENT_DESC* pDesc)
{
	return S_OK;
}

HRESULT CTriggerBox_TutorialUIEvent::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTriggerBox_TutorialUIEvent::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_TutorialUIEvent::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CTriggerBox_TutorialUIEvent::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTriggerBox_TutorialUIEvent::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

}
HRESULT CTriggerBox_TutorialUIEvent::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}


void CTriggerBox_TutorialUIEvent::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CTriggerBox_TutorialUIEvent::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	return;
}

void CTriggerBox_TutorialUIEvent::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CTriggerBox_TutorialUIEvent::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
	{
		m_pGameInstance->Broadcast<TUTORIAL_POPUP_TRIGGER>(m_eUITutorialPopUpTypeID);
	}

	if (m_eQuestEvent == DTO::QUESTEVENT::AREA_ENTER)
	{
		CallQuestEvent(Get_Object_Enum_Tag(), 1);
	}
}

void CTriggerBox_TutorialUIEvent::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (iOtherLayer & PHYSICSFILTERGROUP::PLAYER)
	{
		m_pGameInstance->Broadcast<TUTORIAL_POPUP_CLEAR>(m_eUITutorialPopUpTypeID);
	}

	if (m_eQuestEvent == DTO::QUESTEVENT::AREA_EXIT)
	{
		CallQuestEvent(Get_Object_Enum_Tag(), 1);
	}
}

CTriggerBox_TutorialUIEvent* CTriggerBox_TutorialUIEvent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerBox_TutorialUIEvent* pTriggerBox = new CTriggerBox_TutorialUIEvent(pDevice,pContext);
	if (FAILED(pTriggerBox->Initialize_Prototype()))
	{
		MSG_BOX("TriggerBox TutorialUIEvent is Failed To Create");
		return nullptr;
	}

	return pTriggerBox;
}

CGameObject* CTriggerBox_TutorialUIEvent::Clone(void* pArg)
{
	CTriggerBox_TutorialUIEvent* pTriggerBox = new CTriggerBox_TutorialUIEvent(*this);
	if (FAILED(pTriggerBox->Initialize(pArg)))
	{
		MSG_BOX("TriggerBox TutorialUIEvent is Failed To Clone");
		return nullptr;
	}

	return pTriggerBox;
}

void CTriggerBox_TutorialUIEvent::Free()
{
	Super::Free();
}