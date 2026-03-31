#include "pch.h"
#include "UIConversation_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Canvas.h"
#include "IInteractable.h"
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include "QuestManager.h"
#include "DialogueManager.h"
#include <UI_Manager.h>

#define TEXT_SPEED 0.1f


CUIConversation_Text::CUIConversation_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIConversation_Text::CUIConversation_Text(const CUIConversation_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIConversation_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Text::Initialize(void* pArg)
{
	CONVERSATION_TEXT_DESC* pDesc = static_cast<CONVERSATION_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}


void CUIConversation_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIConversation_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUIConversation_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIConversation_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIConversation_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Text::Ready_Components(CONVERSATION_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIConversation_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::CONVERSATION_NAME:
		break;
	case DTO::EUITextSubClassType::CONVERSATION_TEXT:
		break;
	case DTO::EUITextSubClassType::CONVERSATION_CURRENT_TEXT:
		m_iProgressConversation = 1;
		break;
	}
	return S_OK;
}

HRESULT CUIConversation_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIConversation_Text::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_ENTER>([this](CGameObject* pObj)
			{
				switch (m_eTextSubClassType)
				{
				case DTO::EUITextSubClassType::CONVERSATION_NAME:
				{
				}
					break;
				case DTO::EUITextSubClassType::CONVERSATION_TEXT:
				{
					auto* p = dynamic_cast<IInteractable*>(pObj);
					if (p == nullptr)
						return;

					p->Interact();
					m_pConversationTarget = pObj;
				}
					break;
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<INTERACT_LOST>([this](CGameObject* pObj)
			{
				switch (m_eTextSubClassType)
				{
				case DTO::EUITextSubClassType::CONVERSATION_NAME:
				{
				}
					break;
				case DTO::EUITextSubClassType::CONVERSATION_TEXT:
				{
					m_pConversationTarget = nullptr;
				}
					break;
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{				
				this->Set_Visible();
				this->Set_Active(true);

				if (nullptr != this->m_pConversationTarget)
				{
					this->m_pConversationTarget; // 이게 지금 말하고 있는 대상일 듯

				}

			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{
				this->Set_Invisible();
			}));
}

void CUIConversation_Text::Initialize_Visible_Event()
{
}

void CUIConversation_Text::Initialize_InVisible_Event()
{
}

_bool CUIConversation_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

_bool CUIConversation_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIConversation_Text::Tick_By_Type(const _float fTimeDelta)
{
	auto* pDialogue = CDialogueManager::GetInstance()->GetDialogue();
	if (nullptr == pDialogue)
		return;

	if (m_pParentCanvasCache->Get_CommonParam_bool()[0] && m_pParentCanvasCache->Get_CommonParam_bool()[1])
	{
		m_pGameInstance->Broadcast<DIALOGUE_NEXT>();

		m_pParentCanvasCache->Get_CommonParam_bool_Ref()[0] = false;
		m_pParentCanvasCache->Get_CommonParam_bool_Ref()[1] = false;
	}

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::CONVERSATION_NAME:
	{
		m_wstrText = pDialogue->wstrSpeakerName;
	}
	break;
	case DTO::EUITextSubClassType::CONVERSATION_TEXT:
		Tick_For_ConversationText(fTimeDelta, pDialogue);
		break;
	case DTO::EUITextSubClassType::CONVERSATION_CURRENT_TEXT:
		Tick_For_ConversationCurrentText(fTimeDelta, pDialogue);
		break;
	}
}

void CUIConversation_Text::Tick_For_ConversationText(const _float fTimeDelta, auto* pDialogue)
{
	if (KEY_BUTTON_DOWN(DIK_SPACE))
	{
		if (!m_isFinCurrentConversation)
		{
			m_wstrText = m_wstrCurrentText;
			m_isFinCurrentConversation = true;
			m_iProgressConversation = (_uint)m_wstrCurrentText.size();
			return;
		}
		else
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[0] = true;

			m_isFinCurrentConversation = false;
			m_fConversation_TimeAcc = 0.f;
			m_iProgressConversation = 0;
			return;
		}
	}

	if (m_isFinCurrentConversation)
		return;

	m_wstrCurrentText = pDialogue->wstrContentText;

	m_fConversation_TimeAcc += fTimeDelta;
	if (m_fConversation_TimeAcc > TEXT_SPEED)
	{
		m_fConversation_TimeAcc = 0.f;
		m_iProgressConversation++;
	}

	if (m_iProgressConversation <= m_wstrCurrentText.size())
		m_wstrText = m_wstrCurrentText.substr(0, m_iProgressConversation);
	else
		m_isFinCurrentConversation = true;
}

void CUIConversation_Text::Tick_For_ConversationCurrentText(const _float fTimeDelta, auto* pDialogue)
{
	if (KEY_BUTTON_DOWN(DIK_SPACE))
	{
		if (!m_isFinCurrentConversation)
		{
			m_wstrText = m_wstrCurrentText;
			m_isFinCurrentConversation = true;
			m_iProgressConversation = (_uint)m_wstrCurrentText.size();
			return;
		}
		else
		{
			m_pParentCanvasCache->Get_CommonParam_bool_Ref()[1] = true;
			m_isFinCurrentConversation = false;
			m_fConversation_TimeAcc = 0.f;
			m_iProgressConversation = 1;
			return;
		}
	}

	Tick_Fade_Text(fTimeDelta);

	if (m_isFinCurrentConversation)
		return;

	m_wstrCurrentText = pDialogue->wstrContentText;

	m_fConversation_TimeAcc += fTimeDelta;
	if (m_fConversation_TimeAcc > TEXT_SPEED)
	{
		m_fConversation_TimeAcc = 0.f;
		Ready_Fade_Text(TEXT_SPEED, 0.7f, 1.f, m_fDelay);
		m_iProgressConversation++;
	}

	if (m_iProgressConversation <= m_wstrCurrentText.size())
		m_wstrText = m_wstrCurrentText.substr(0, m_iProgressConversation);
	else
		m_isFinCurrentConversation = true;
}

CUIConversation_Text* CUIConversation_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIConversation_Text* pInstance = new CUIConversation_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIConversation_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIConversation_Text::Clone(void* pArg)
{
	CUIConversation_Text* pInstance = new CUIConversation_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIConversation_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIConversation_Text::Free()
{
	Super::Free();
}
