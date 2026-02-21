#include "pch.h"
#include "UICommon_Trigger.h"
#include "Client_Defines.h"
#include <iostream>
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
#include "Canvas.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUICommon_Trigger::CUICommon_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUITrigger(pDevice, pDeviceContext)
{
}

CUICommon_Trigger::CUICommon_Trigger(const CUICommon_Trigger& rhs)
	:CUITrigger(rhs)
{
}

HRESULT CUICommon_Trigger::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommon_Trigger::Initialize(void* pArg)
{
	UI_TRIGGER_DESC* pDesc = static_cast<UI_TRIGGER_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUICommon_Trigger::Attach_Personal_Info()
{
	switch (m_eSubClassType)
	{
	case DTO::EUITriggerSubClassType::NONE_OWNER:
		return S_OK;
	case DTO::EUITriggerSubClassType::MENU_TAB_TRIGGER:
		return S_OK;
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUICommon_Trigger::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUICommon_Trigger::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_ENTER))
	{
		m_eCurTriggerType = ETriggerEventType::PRESS_ENTER;
		Fire_ToTargets(ETriggerEventType::PRESS_ENTER);
	}
	else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_EXIT))
	{
		m_eCurTriggerType = ETriggerEventType::PRESS_EXIT;
		Fire_ToTargets(ETriggerEventType::PRESS_EXIT);
	}
	else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVER_ENTER))
	{
		Fire_ToTargets(ETriggerEventType::HOVER_ENTER);
	}
	else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVER_EXIT))
	{
		Fire_ToTargets(ETriggerEventType::HOVER_EXIT);
	}
}

void CUICommon_Trigger::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUICommon_Trigger::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUICommon_Trigger::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUICommon_Trigger::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();

	return S_OK;
}

void CUICommon_Trigger::Fire_ToTargets(ETriggerEventType eEvent)
{
	for (auto* pUI : m_pTriggerUI[ENUM_TO_UINT(eEvent)])
		if (pUI) pUI->OnUIEvent(eEvent, this);

	for (auto* pCanvas : m_pTriggerCanvas[ENUM_TO_UINT(eEvent)])
		if (pCanvas) pCanvas->OnCanvasEvent(eEvent, this);
}

void CUICommon_Trigger::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	switch (eEvent)
	{
	case Client::ETriggerEventType::HOVER_ENTER:
		break;
	case Client::ETriggerEventType::HOVER_EXIT:
		break;
	case Client::ETriggerEventType::PRESS_ENTER:
		if (m_isInteract)
		{
			m_eCurTriggerType = ETriggerEventType::PRESS_ENTER;
			Set_NonInteractable();
		}
		else
		{
			m_eCurTriggerType = ETriggerEventType::PRESS_ENTER;
			Set_Interactable();
		}

		break;
	case Client::ETriggerEventType::PRESS_EXIT:
		break;
	case Client::ETriggerEventType::END:
	default:
		break;
	}
}

void CUICommon_Trigger::Initialize_Interactable_Event()
{
	m_fDelayTimeAcc = 0.f;
}

void CUICommon_Trigger::Initialize_NonInteractable_Event()
{
}

_bool CUICommon_Trigger::Tick_Interactable_Event(const _float fTimeDelta)
{
	_bool isFin = Check_FinEvent(m_eCurTriggerType);
	m_fDelayTimeAcc += fTimeDelta;
	_bool isDelay = (m_fDelayTimeAcc >= m_fDelay);

	if (isFin && isDelay)
	{
		m_isVisible = true;
		m_eCurTriggerType = ETriggerEventType::END;
		return true;
	}
	return false;
}

_bool CUICommon_Trigger::Tick_NonInteractable_Event(const _float fTimeDelta)
{
	m_isVisible = false;
	return true;
}

HRESULT CUICommon_Trigger::Ready_Components(UI_TRIGGER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUICommon_Trigger::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();

	return S_OK;
}

CUICommon_Trigger* CUICommon_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUICommon_Trigger* pInstance = new CUICommon_Trigger(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUICommon_Trigger::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUICommon_Trigger::Clone(void* pArg)
{
	CUICommon_Trigger* pInstance = new CUICommon_Trigger(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUICommon_Trigger::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUICommon_Trigger::Free()
{
	Super::Free();
}
