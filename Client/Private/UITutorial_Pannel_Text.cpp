#include "pch.h"
#include "UITutorial_Pannel_Text.h"
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
#include <UI_Manager.h>

CUITutorial_Pannel_Text::CUITutorial_Pannel_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUITutorial_Pannel_Text::CUITutorial_Pannel_Text(const CUITutorial_Pannel_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUITutorial_Pannel_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Initialize(void* pArg)
{
	TUTORIAL_PANNEL_TEXT_DESC* pDesc = static_cast<TUTORIAL_PANNEL_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUITutorial_Pannel_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_Pannel_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUITutorial_Pannel_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_Pannel_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUITutorial_Pannel_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Ready_Components(TUTORIAL_PANNEL_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Attach_Personal_Info()
{
	if (m_isSpawned)
	{
		Set_Visible();
		m_isSpawned = false;
	}
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUITutorial_Pannel_Text::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);
}

void CUITutorial_Pannel_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
		Ready_Lerp_Movement(Vec2{ 0.f, 235.5 }, Vec2{ 0.f, 0.f }, 1.5f, 3.f, 0.5f, true);
		Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
		Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_PREV_BTN_TEXT:
		Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_NEXT_BTN_TEXT:
		Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
		break;
	}
}

void CUITutorial_Pannel_Text::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;

	Ready_Fade_Text(0.4f, 0.f, 1.f, m_fDelay);
}

_bool CUITutorial_Pannel_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_TITLE_TEXT:
	{
		_bool is = Tick_Lerp_Movement(fTimeDelta);
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (is && isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
		break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_CONTENTS_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_PREV_BTN_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	case DTO::EUITextSubClassType::TUTORIAL_PANNEL_NEXT_BTN_TEXT:
	{
		_bool isFade = Tick_Fade_Text(fTimeDelta);

		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;

			return true;
		}
	}
	break;
	}
	return false;
}

_bool CUITutorial_Pannel_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	if (Tick_Fade(fTimeDelta))
	{
		Request_SetDead();
		m_fAlpha_Ratio = 1.f;
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

HRESULT CUITutorial_Pannel_Text::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	m_isSpawned = true;
	m_isDeadRequest = false;
	return S_OK;
}

HRESULT CUITutorial_Pannel_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

CUITutorial_Pannel_Text* CUITutorial_Pannel_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_Pannel_Text* pInstance = new CUITutorial_Pannel_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_Pannel_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_Pannel_Text::Clone(void* pArg)
{
	CUITutorial_Pannel_Text* pInstance = new CUITutorial_Pannel_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_Pannel_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_Pannel_Text::Free()
{
	Super::Free();
}
