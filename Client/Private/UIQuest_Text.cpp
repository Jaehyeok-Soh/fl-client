#include "pch.h"
#include "UIQuest_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include <UI_Manager.h>

CUIQuest_Text::CUIQuest_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIQuest_Text::CUIQuest_Text(const CUIQuest_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIQuest_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Text::Initialize(void* pArg)
{
	QUEST_TEXT_DESC* pDesc = static_cast<QUEST_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUIQuest_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIQuest_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIQuest_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIQuest_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIQuest_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Text::Ready_Components(QUEST_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIQuest_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIQuest_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIQuest_Text::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<TUTORIAL_POPUP_TRIGGER>([this](EUITutorialPopUpTypeID ID)
			{
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<TUTORIAL_POPUP_CLEAR>([this](EUITutorialPopUpTypeID ID)
			{
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_SCENARIO_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				desc.tScenarioInfo.wstrSubTitle;
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_CHAPTER_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				desc.tScenarioInfo.wstrSubTitle;
			})
	);
}

void CUIQuest_Text::Initialize_Visible_Event()
{
}

void CUIQuest_Text::Initialize_InVisible_Event()
{
}

_bool CUIQuest_Text::Tick_Visible_Event(const _float fTimeDelta)
{

	return true;
}

_bool CUIQuest_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIQuest_Text* CUIQuest_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIQuest_Text* pInstance = new CUIQuest_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIQuest_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIQuest_Text::Clone(void* pArg)
{
	CUIQuest_Text* pInstance = new CUIQuest_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIQuest_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIQuest_Text::Free()
{
	Super::Free();
}
