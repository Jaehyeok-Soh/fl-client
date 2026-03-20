#include "pch.h"
#include "UITutorial_PopUp_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "CameraMan.h"
#include "Canvas.h"
#include "Player.h"
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

// Common Params Bool
#define POPUP_3 0
#define POPUP_4 1

CUITutorial_PopUp_Image::CUITutorial_PopUp_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUITutorial_PopUp_Image::CUITutorial_PopUp_Image(const CUITutorial_PopUp_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUITutorial_PopUp_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Image::Initialize(void* pArg)
{
	TUTORIAL_POPUP_IMAGE_DESC* pDesc = static_cast<TUTORIAL_POPUP_IMAGE_DESC*>(pArg);
	m_eTutorialTypeID = static_cast<EUITutorialPopUpTypeID>(pDesc->iTutorialTypeID);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;



	return S_OK;
}


HRESULT CUITutorial_PopUp_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	m_vMoveOffset = Vec2{ 0.f, -200.f };

	Set_Active(false);
	m_pParentCanvasCache->Get_CommonParam_bool_Ref()[POPUP_3] = (false);
	m_pParentCanvasCache->Get_CommonParam_bool_Ref()[POPUP_4] = (false);



	return S_OK;
}

void CUITutorial_PopUp_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_PopUp_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUITutorial_PopUp_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_PopUp_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITutorial_PopUp_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Image::Ready_Components(TUTORIAL_POPUP_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Image::Attach_Personal_Info()
{
	m_fOriginWidth = m_fWidth;

	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayer = dynamic_cast<CPlayer*>(pResult);
	if (nullptr == m_pPlayer)
		return E_FAIL;

	return S_OK;
}

void CUITutorial_PopUp_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					if (m_isTriggered)
					{
						this->Set_Visible();
						this->Set_Active(true);
					}
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
		m_pGameInstance->Subscribe<TUTORIAL_POPUP_TRIGGER>([this](EUITutorialPopUpTypeID ID)
			{
				if ((this->m_eTutorialTypeID) == ID)
				{
					if (!m_isFirstEntered)
					{
						m_isTriggered = true;
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
					m_isTriggered = false;
					this->Set_Invisible();
				}
			})
	);
}

void CUITutorial_PopUp_Image::Tick_By_Type(const _float fTimeDelta)
{
	_bool is = { false };

	switch (m_eTutorialTypeID)
	{
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_1:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::JUMP)
			is = true;
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_2:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::SLIDE)
			is = true;
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_3:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::CROUCH)
		{
			if (!m_isFirstTriggered)
			{
				m_isFirstEntered = true;

				Set_Invisible();
				m_pParentCanvasCache->Get_CommonParam_bool_Ref()[POPUP_3] = true;
				m_pGameInstance->Broadcast<TUTORIAL_POPUP_TRIGGER>(EUITutorialPopUpTypeID::TUTORIAL_POPUP_3_1);
				m_isFirstTriggered = true;
			}
		}
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_3_1:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::JUMPBULLET)
		{
			is = true;
		}
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_4:
	{
		Vec3 vLook = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
		_float f = vLook.Dot(Vec3{ 0.f, 1.f, 0.f });

		if (f > 0.6f)
		{
			if (!m_isFirstTriggered)
			{
				m_isFirstEntered = true;
				Set_Invisible();
				m_pParentCanvasCache->Get_CommonParam_bool_Ref()[POPUP_4] = true;
				m_pGameInstance->Broadcast<TUTORIAL_POPUP_TRIGGER>(EUITutorialPopUpTypeID::TUTORIAL_POPUP_4_1);
				m_isFirstTriggered = true;
			}
		}
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_4_1:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::JUMPBULLET)
		{
			is = true;
		}
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_5:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::JUMPWALL)
		{
			is = true;
		}
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_6:
	{
		if (KEY_BUTTON_DOWN(DIK_V))
		{
			is = true;
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_7:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::COMBO)
		{
			is = true;
		}
	}
	break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_8:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::RUNSHORT)
		{
			is = true;
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_9:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::GUNATTACK)
		{
			is = true;
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_10:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_11:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::SKILL1)
		{
			is = true;
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_12:
		break;
	case Client::EUITutorialPopUpTypeID::TUTORIAL_POPUP_13:
	{
		if (m_pPlayer->Get_CurState() == CPlayer::State::SKILL2)
		{
			is = true;
		}
	}
		break;
	case Client::EUITutorialPopUpTypeID::END:
	default:
		break;
	}

	if (is)
	{
		if (!m_isFirstEntered)
		{
			m_isFirstEntered = true;

			m_pGameInstance->Broadcast<TUTORIAL_POPUP_CLEAR>(m_eTutorialTypeID);
		}
	}

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_BG:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON_BG:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON1:
		break;
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON2:
		break;
	}
}

void CUITutorial_PopUp_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON:
		Ready_LerpChange(0.5f, 2.f, 1.f, 1.f, m_fDelay);
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_BG:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON_BG:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON1:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON2:
		Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);
		break;
	}
}

_bool CUITutorial_PopUp_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON:
	{
		_bool is = Tick_LerpChange(&m_fBrightness, fTimeDelta);
		if (is)
		{
			m_isFin_Event = true;
			return true;
		}
	}
	break;

	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_BG:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON_BG:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON1:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON2:
	{
		_bool is = Tick_Fade(fTimeDelta);
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

void CUITutorial_PopUp_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON_BG:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON1:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON2:
		break;

	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_BG:
		Ready_Fade(0.5f, 1.f, 0.f, m_fDelay);
		Ready_LerpChange(0.5f, m_fWidth, 0.1f, 1.f, m_fDelay);
		break;
	}
}

_bool CUITutorial_PopUp_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_ICON_BG:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON1:
	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_KEY_ICON2:
		Set_Active(false);
		m_isFin_Event = true;
		return true;

	case DTO::EUIDImageSubClassType::TUTORIAL_POPUP_BG:
	{
		_bool isFade = Tick_Fade(fTimeDelta);
		_bool is = Tick_LerpChange(&m_fWidth, fTimeDelta);

		if (is && isFade)
		{
			m_fWidth = m_fOriginWidth;
			m_isFin_Event = true;
			Set_Active(false);
			return true;
		}
	}
	break;
	}

	return false;
}

CUITutorial_PopUp_Image* CUITutorial_PopUp_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_PopUp_Image* pInstance = new CUITutorial_PopUp_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_PopUp_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_PopUp_Image::Clone(void* pArg)
{
	CUITutorial_PopUp_Image* pInstance = new CUITutorial_PopUp_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_PopUp_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_PopUp_Image::Free()
{
	Super::Free();
}
