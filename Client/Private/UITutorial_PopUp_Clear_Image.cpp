#include "pch.h"
#include "UITutorial_PopUp_Clear_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUITutorial_PopUp_Clear_Image::CUITutorial_PopUp_Clear_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUITutorial_PopUp_Clear_Image::CUITutorial_PopUp_Clear_Image(const CUITutorial_PopUp_Clear_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUITutorial_PopUp_Clear_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Image::Initialize(void* pArg)
{
	TUTORIAL_POPUP_CLEAR_IMAGE_DESC* pDesc = static_cast<TUTORIAL_POPUP_CLEAR_IMAGE_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;



	return S_OK;
}


HRESULT CUITutorial_PopUp_Clear_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	m_vMoveOffset = Vec2{ 0.f, -200.f };
	return S_OK;
}

void CUITutorial_PopUp_Clear_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_PopUp_Clear_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUITutorial_PopUp_Clear_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_PopUp_Clear_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITutorial_PopUp_Clear_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Image::Ready_Components(TUTORIAL_POPUP_CLEAR_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Image::Attach_Personal_Info()
{
	return S_OK;
}

void CUITutorial_PopUp_Clear_Image::Bind_Events()
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


	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_BG:
	{
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<TUTORIAL_POPUP_CLEAR>([this](EUITutorialPopUpTypeID ID)
				{
					this->Set_Visible();
					this->Set_Active(true);
				})
		);

		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::TUTORIAL_POPUP_EVENT2 == Desc.eEventID)
					{
						this->Set_Invisible();
					}
				})
		);

	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_ICON:
	{
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<TUTORIAL_POPUP_CLEAR>([this](EUITutorialPopUpTypeID ID)
				{
					this->Set_Visible();
					this->Set_Active(true);
				})
		);

		m_vecEventHandles.push_back(
			m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
				{
					if (EUIEventID::TUTORIAL_POPUP_EVENT2 == Desc.eEventID)
					{
						this->Set_Invisible();
					}
				})
		);
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_CIRCLE_FX:
	{
		m_vecEventHandles.push_back(
			m_pGameInstance->Subscribe<TUTORIAL_POPUP_CLEAR>([this](EUITutorialPopUpTypeID ID)
				{
					this->Set_Visible();
					this->Set_Active(true);
				})
		);
	}
	break;
	}
}

void CUITutorial_PopUp_Clear_Image::Tick_By_Type(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_BG:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_ICON:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_CIRCLE_FX:
	{
	}
	break;
	}
}

void CUITutorial_PopUp_Clear_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_BG:
	{
		// UI SOUND
		m_pGameInstance->Play_OneShot(0, Engine_Utils::ToHash("UI_TUTORIAL_POPUP_CLEAR"), 1.f);

		Ready_Fade(0.5f, 0.f, 1.f, 0.f);
		Ready_ChageColor(0.2f, 
			Vec4{ 0.f, 0.f, 0.f, 0.f }, Vec4{ 0.f, 0.f, 0.f, 0.f }, 
			Vec4{ 1.f, 0.98f, 0.8f, 0.f }, Vec4{ 1.f, 0.98f, 0.8f, 0.f },
			3.f, m_fDelay);

		m_fProgress_Ratio = 0.f;
	}
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_ICON:
	{
		Ready_LerpChange(0.5f, 1.f, 0.f, 5.f, 0.f, true);
		m_fAlpha_Ratio = 1.f;
	}
	break;

	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_CIRCLE_FX:
	{
		Ready_LerpChange(0.3f, 0.f, 400.f, 3.f, 0.f);
		m_fWidth = 0.1f;
		m_fHeight = 0.1f;
		m_fAlpha_Ratio = 1.f;
	}
	break;
	}
}

_bool CUITutorial_PopUp_Clear_Image::Tick_Visible_Event(const _float fTimeDelta)
{

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_BG:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool isColor = Tick_ChageColor(fTimeDelta);

		if (isColor && !m_isPaulse)
		{
			Ready_ChageColor(0.2f,
				Vec4{ 1.f, 0.98f, 0.8f, 0.f }, Vec4{ 1.f, 0.98f, 0.8f, 0.f },
				Vec4{ 0.f, 0.f, 0.f, 0.f }, Vec4{ 0.f, 0.f, 0.f, 0.f },
				3.f, m_fDelay);

			m_isPaulse = true;
		}

		if (isFade && isColor && m_isPaulse)
		{
			m_isFin_Event = true;
			m_isPaulse = false;
			return true;
		}

	}
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_ICON:
	{
		_bool is = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);

		if (is)
		{
			UIEVENT_DESC Desc = {};
			Desc.eEventID = EUIEventID::TUTORIAL_POPUP_EVENT1;
			m_pUIManager->Get_UIEvents().Broadcast(Desc);
			return true;
		}
	}
	break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_CIRCLE_FX:
	{
		_float f = {};
		_bool is = Tick_LerpChange(&f, fTimeDelta);

		m_fProgress_Ratio = 0.f;
		m_fWidth = f;
		m_fHeight = f;

		if (is)
		{
			m_fAlpha_Ratio -= fTimeDelta;
			if (m_fAlpha_Ratio <= 0.f)
			{
				Move_Size(0.1f, 0.1f);
				Set_Invisible();
				UIEVENT_DESC Desc = {};
				Desc.eEventID = EUIEventID::TUTORIAL_POPUP_EVENT2;
				m_pUIManager->Get_UIEvents().Broadcast(Desc);

				return true;
			}
		}
	}
	break;
	}
	return false;
}

void CUITutorial_PopUp_Clear_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_BG:
		Ready_Fade(0.5f, 1.f, 0.f, 0.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_ICON:
		Ready_Fade(0.5f, 1.f, 0.f, 0.f);
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_CIRCLE_FX:
		break;
	}
}

_bool CUITutorial_PopUp_Clear_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_BG:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		if(isFade	)
		{
			m_isFin_Event = true;
			Set_Active(false);
			m_fProgress_Ratio = 1.f;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_ICON:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		if (isFade)
		{
			m_isFin_Event = true;
			Set_Active(false);
			m_fProgress_Ratio = 1.f;
			return true;
		}
	}
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_CLEAR_CIRCLE_FX:
	{
		Set_Active(false);
		return true;
	}
	break;
	}

	return false;
}

CUITutorial_PopUp_Clear_Image* CUITutorial_PopUp_Clear_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_PopUp_Clear_Image* pInstance = new CUITutorial_PopUp_Clear_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_PopUp_Clear_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_PopUp_Clear_Image::Clone(void* pArg)
{
	CUITutorial_PopUp_Clear_Image* pInstance = new CUITutorial_PopUp_Clear_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_PopUp_Clear_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_PopUp_Clear_Image::Free()
{
	Super::Free();
}
