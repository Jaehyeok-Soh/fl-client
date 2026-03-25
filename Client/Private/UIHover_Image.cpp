#include "pch.h"
#include "UIHover_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
#include "CameraMan.h"

#include "MainPlayer.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIHover_Image::CUIHover_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIHover_Image::CUIHover_Image(const CUIHover_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIHover_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIHover_Image::Initialize(void* pArg)
{
	HOVER_IMAGE_DESC* pDesc = static_cast<HOVER_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIHover_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIHover_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIHover_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Tick_By_Type(fTimeDelta);
}

void CUIHover_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIHover_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIHover_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIHover_Image::Ready_Components(HOVER_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIHover_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIHover_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::HOVER_ENTER_MENU_ICON:
	{

	}
		break;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_ICON:
	{

	}
		break;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_BG:
	{
		m_fOriginWidth = m_fWidth;
	}
		break;
	}
	return S_OK;
}

void CUIHover_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::HOVER_POPUP_BEGIN:
		break;
	case DTO::EUIDImageSubClassType::HOVER_ENTER_MENU_ICON:
	{
		/* Hover Enter 이벤트 발송 */
		if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_ENTER))
		{
			UIEVENT_DESC Desc = {};
			Desc.eEventID = EUIEventID::MENU_ENTER_ICON_HOVER_ENTER;
			m_pUIManager->Get_UIEvents().Broadcast(Desc);
		}
		/* Hover Exit 이벤트 발송 */
		if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_EXIT))
		{
			UIEVENT_DESC Desc = {};
			Desc.eEventID = EUIEventID::MENU_ENTER_ICON_HOVER_EXIT;
			m_pUIManager->Get_UIEvents().Broadcast(Desc);
		}

		/* Menu Open 이벤트 발송 */
		if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::PRESS_ENTER))
		{
			UIEVENT_DESC Desc = {};
			Desc.eEventID = EUIEventID::MENU_OPEN;
			m_pUIManager->Get_UIEvents().Broadcast(Desc);

			Set_Invisible();
			Set_NonInteractable();
		}
	}
	break;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_BG:
		break;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_ICON:
		break;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_END:
		break;
	}
}

void CUIHover_Image::Bind_Events()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::HOVER_ENTER_MENU_ICON:
	{
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					this->Set_Visible();
					this->Set_Interactable();
				}
			}));

		// 대화 Event
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
				{
					this->Set_Invisible();
				}));
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
				{
					this->Set_Active(true);
					this->Set_Interactable();
					this->Set_Visible();
				}));

		// 패널 Events
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
					{
						this->Set_Invisible();
					}
				}));
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
					{
						this->Set_Visible();
						this->Set_Interactable();
						this->Set_Active(true);
					}
				}));
	}
	break;

	case DTO::EUIDImageSubClassType::HOVER_POPUP_ICON:
	{
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::MENU_ENTER_ICON_HOVER_ENTER == Desc.eEventID)
					{
						this->Set_Visible();
					}
				}));
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::MENU_ENTER_ICON_HOVER_EXIT == Desc.eEventID)
					{
						this->Set_Invisible();
					}
				}));
	}
	break;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_BG:
	{
		m_fOriginWidth = m_fWidth;

		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::MENU_ENTER_ICON_HOVER_ENTER == Desc.eEventID)
					{
						this->Set_Visible();
					}
				}));
		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::MENU_ENTER_ICON_HOVER_EXIT == Desc.eEventID)
					{
						this->Set_Invisible();
					}
				}));
	}
	break;
	}
}

void CUIHover_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::HOVER_POPUP_BG)
	{
		m_fWidth = 0.1f;
	}
	else
	{
		m_fTimeAcc = 0.f;
		m_fAlpha_Ratio = 0.f;
	}
}

void CUIHover_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
}

_bool CUIHover_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::HOVER_POPUP_BG)
	{
		m_fWidth += fTimeDelta * 500.f;
		Set_Size(m_fWidth, m_fHeight);

		if (m_fWidth >= m_fOriginWidth)
		{
			m_fWidth = m_fOriginWidth;
			m_isFin_Event = true;
			return true;
		}
	}
	else
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc < m_fDelay)
			return false;

		m_fAlpha_Ratio += fTimeDelta * 2.f;
		if (m_fAlpha_Ratio >= 1.f)
		{
			m_fAlpha_Ratio = 1.f;
			m_isFin_Event = true;
			return true;
		}
	}
	return false;
}

_bool CUIHover_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	return true;
}

CUIHover_Image* CUIHover_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIHover_Image* pInstance = new CUIHover_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIHover_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIHover_Image::Clone(void* pArg)
{
	CUIHover_Image* pInstance = new CUIHover_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIHover_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIHover_Image::Free()
{
	Super::Free();
}
