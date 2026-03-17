#include "pch.h"
#include "UITutorial_PopUp_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "Canvas.h"
#include "Player.h"
#include "WorldUI_Component.h"
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"
#include <UI_Manager.h>

#define POPUP_3 0
#define POPUP_4 1

CUITutorial_PopUp_Text::CUITutorial_PopUp_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUITutorial_PopUp_Text::CUITutorial_PopUp_Text(const CUITutorial_PopUp_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUITutorial_PopUp_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Text::Initialize(void* pArg)
{
	TUTORIAL_POPUP_TEXT_DESC* pDesc = static_cast<TUTORIAL_POPUP_TEXT_DESC*>(pArg);
	m_eTutorialTypeID = static_cast<EUITutorialPopUpTypeID>(pDesc->iTutorialTypeID);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	m_vMoveOffset = Vec2{ 0.f, -200.f };

	return S_OK;
}


void CUITutorial_PopUp_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_PopUp_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUITutorial_PopUp_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_PopUp_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUITutorial_PopUp_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Text::Ready_Components(TUTORIAL_POPUP_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Text::Attach_Personal_Info()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT1:
		break;
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT2:
		break;
	default:
		return E_FAIL;
	}
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(pResult);
	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

HRESULT CUITutorial_PopUp_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUITutorial_PopUp_Text::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<TUTORIAL_POPUP_TRIGGER>([this](EUITutorialPopUpTypeID ID)
			{
				if ((this->m_eTutorialTypeID) == ID)
				{
					if (!m_isFirstEntered)
					{
						this->Set_Visible();
						this->Set_Active(true);
					}
				}
			})
	);

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<TUTORIAL_POPUP_CLEAR>([this](EUITutorialPopUpTypeID ID)
			{
				if ((this->m_eTutorialTypeID) == ID)
				{
					m_isFirstEntered = true;
					this->Set_Invisible();
				}
			})
	);
}

void CUITutorial_PopUp_Text::Initialize_Visible_Event()
{
	m_isFin_Event = false;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT1:
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT2:
		Ready_Fade_Text(0.5f, 0.f, 1.f, m_fDelay);
		break;
	}
}

void CUITutorial_PopUp_Text::Initialize_InVisible_Event()
{
	m_isFin_Event = false;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT1:
		break;
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT2:
		break;
	}
}

_bool CUITutorial_PopUp_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT1:
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT2:
	{
		_bool is = Tick_Fade_Text(fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			return true;
		}
	}
	break;
	}
	return false;
}

_bool CUITutorial_PopUp_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT1:
		break;
	case DTO::EUITextSubClassType::TUTORIAL_POPUP_TEXT2:
		break;
	}
	return true;
}

void CUITutorial_PopUp_Text::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eTutorialTypeID)
	{
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_1:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_2:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_3:
	{
		if (m_pParentCanvasCache->Get_CommonParam_bool()[POPUP_3])
		{
			Set_Invisible();
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_3_1:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_4:
	{
		if (m_pParentCanvasCache->Get_CommonParam_bool()[POPUP_4])
		{
			Set_Invisible();
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_4_1:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_5:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_6:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_7:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_8:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_9:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_10:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_11:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_12:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_13:
		break;
	case Client::EUITutorialPopUpTypeID::END:
		break;
	default:
		break;
	}
}

CUITutorial_PopUp_Text* CUITutorial_PopUp_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_PopUp_Text* pInstance = new CUITutorial_PopUp_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_PopUp_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_PopUp_Text::Clone(void* pArg)
{
	CUITutorial_PopUp_Text* pInstance = new CUITutorial_PopUp_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_PopUp_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_PopUp_Text::Free()
{
	Super::Free();
}
