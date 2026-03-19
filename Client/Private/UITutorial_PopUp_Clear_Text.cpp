#include "pch.h"
#include "UITutorial_PopUp_Clear_Text.h"
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

CUITutorial_PopUp_Clear_Text::CUITutorial_PopUp_Clear_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUITutorial_PopUp_Clear_Text::CUITutorial_PopUp_Clear_Text(const CUITutorial_PopUp_Clear_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUITutorial_PopUp_Clear_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Text::Initialize(void* pArg)
{
	TUTORIAL_POPUP_CLEAR_TEXT_DESC* pDesc = static_cast<TUTORIAL_POPUP_CLEAR_TEXT_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	m_vMoveOffset = Vec2{ 0.f, -200.f };
	return S_OK;
}


void CUITutorial_PopUp_Clear_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITutorial_PopUp_Clear_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUITutorial_PopUp_Clear_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITutorial_PopUp_Clear_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUITutorial_PopUp_Clear_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Text::Ready_Components(TUTORIAL_POPUP_CLEAR_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUITutorial_PopUp_Clear_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUITutorial_PopUp_Clear_Text::Bind_Events()
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

void CUITutorial_PopUp_Clear_Text::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	Ready_Fade_Text(0.5f, 0.f, 0.8f, m_fDelay);
}

void CUITutorial_PopUp_Clear_Text::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	Ready_Fade_Text(0.5f, 0.8f, 0.f, m_fDelay);
}

_bool CUITutorial_PopUp_Clear_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool is = Tick_Fade_Text(fTimeDelta);
	if (is)
	{
		m_isFin_Event = true;
		return true;
	}
	return false;
}

_bool CUITutorial_PopUp_Clear_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool is = Tick_Fade_Text(fTimeDelta);
	if (is)
	{
		m_isFin_Event = true;
		Set_Active(false);
		return true;
	}
	return false;
}

CUITutorial_PopUp_Clear_Text* CUITutorial_PopUp_Clear_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITutorial_PopUp_Clear_Text* pInstance = new CUITutorial_PopUp_Clear_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITutorial_PopUp_Clear_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITutorial_PopUp_Clear_Text::Clone(void* pArg)
{
	CUITutorial_PopUp_Clear_Text* pInstance = new CUITutorial_PopUp_Clear_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITutorial_PopUp_Clear_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITutorial_PopUp_Clear_Text::Free()
{
	Super::Free();
}
