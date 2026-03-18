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

	Tick_By_Type(fTimeDelta);
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
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::QUEST_BEGIN:
		break;
	case DTO::EUITextSubClassType::QUEST_SCENARIO_TEXT:
	break;
	case DTO::EUITextSubClassType::QUEST_TITLE_TEXT:
		break;
	case DTO::EUITextSubClassType::QUEST_CONTENTS_TEXT:
		break;
	case DTO::EUITextSubClassType::QUEST_TRACKING_TEXT:
		break;
	case DTO::EUITextSubClassType::QUEST_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIQuest_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIQuest_Text::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_END>([this]()
			{
				this->Set_Visible();
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_SCENARIO_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
				desc.tScenarioInfo.wstrSubTitle;
				desc.tChapterInfo.tQuestDesc.wstrTitle;
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_CHANGE_CHAPTER_NOTIFY>([this]()
			{
				auto desc = CQuestManager::GetInstance()->Get_QuestInfo();

				switch (this->m_eTextSubClassType)
				{
				case DTO::EUITextSubClassType::QUEST_SCENARIO_TEXT:
					break;
				case DTO::EUITextSubClassType::QUEST_TITLE_TEXT:
					m_wstrText = desc.tChapterInfo.tQuestDesc.wstrTitle;
					break;
				case DTO::EUITextSubClassType::QUEST_CONTENTS_TEXT:
					break;
				case DTO::EUITextSubClassType::QUEST_TRACKING_TEXT:
					break;
				case DTO::EUITextSubClassType::QUEST_END:
					break;
				}
			})

	);

	return;
}

void CUIQuest_Text::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	Ready_Lerp_Movement(Vec2{ -20.f, 0.f }, Vec2{ 0.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
	Ready_Fade_Text(0.5f, 0.f, 1.f, m_fDelay);
}

void CUIQuest_Text::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	Ready_Lerp_Movement(Vec2{ 0.f, 0.f }, Vec2{ -20.f, 0.f }, 0.5f, 3.f, m_fDelay, true);
	Ready_Fade_Text(0.5f, 1.f, 0.f, m_fDelay);
}

_bool CUIQuest_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);
	_bool isMove = Tick_Lerp_Movement(fTimeDelta);

	m_vFontColor.x = m_vFontColor.w;
	m_vFontColor.y = m_vFontColor.w;
	m_vFontColor.z = m_vFontColor.w;
	if (isFade && isMove)
	{
		m_isFin_Event = true;
		return true;
	}

	return false;
}

_bool CUIQuest_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade_Text(fTimeDelta);
	_bool isMove = Tick_Lerp_Movement(fTimeDelta);

	m_vFontColor.x = m_vFontColor.w;
	m_vFontColor.y = m_vFontColor.w;
	m_vFontColor.z = m_vFontColor.w;

	if (isFade && isMove)
	{
		m_isFin_Event = true;
		return true;
	}
	return false;
}

void CUIQuest_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::QUEST_BEGIN:
		break;
	case DTO::EUITextSubClassType::QUEST_SCENARIO_TEXT:
	{
		auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
		m_wstrText = desc.tScenarioInfo.wstrTitle;
	}
	break;
	case DTO::EUITextSubClassType::QUEST_TITLE_TEXT:
	{
		auto desc = CQuestManager::GetInstance()->Get_QuestInfo();
		if (desc.tChapterInfo.eEvent == DTO::EQuestEvent::MONSTER_KILL)
		{

			m_wstrText = desc.tChapterInfo.tQuestDesc.wstrTitle + L"(" + std::to_wstring(desc.tChapterInfo.iCurrentCount) + L"/10)";
		}
		else
			m_wstrText = desc.tChapterInfo.tQuestDesc.wstrTitle;
	}
	break;
	case DTO::EUITextSubClassType::QUEST_CONTENTS_TEXT:
		break;
	case DTO::EUITextSubClassType::QUEST_TRACKING_TEXT:
		break;
	case DTO::EUITextSubClassType::QUEST_END:
		break;
	case DTO::EUITextSubClassType::END:
	default:
		break;
	}
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
