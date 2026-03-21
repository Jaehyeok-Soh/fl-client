#include "pch.h"
#include "UIConversation_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
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
	return S_OK;
}

HRESULT CUIConversation_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIConversation_Text::Bind_Events()
{
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
				}
				break;
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{				
				this->Set_Visible();
				this->Set_Active(true);
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{
				this->Set_Invisible();
			})
	);
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

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::CONVERSATION_NAME:
	{
		m_wstrText = pDialogue->wstrSpeakerName;
	}
	break;
	case DTO::EUITextSubClassType::CONVERSATION_TEXT:
	{
		if (KEY_BUTTON_DOWN(DIK_SPACE))
			m_pGameInstance->Broadcast<DIALOGUE_NEXT>();

		m_wstrText = pDialogue->wstrContentText;
	}
	break;
	}
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
