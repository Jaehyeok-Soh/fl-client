#include "pch.h"
#include "UIMenu_Trigger.h"
#include "Client_Defines.h"

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

CUIMenu_Trigger::CUIMenu_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUITrigger(pDevice, pDeviceContext)
{
}

CUIMenu_Trigger::CUIMenu_Trigger(const CUIMenu_Trigger& rhs)
	:CUITrigger(rhs)
{
}

HRESULT CUIMenu_Trigger::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Trigger::Initialize(void* pArg)
{
	UI_TRIGGER_DESC* pDesc = static_cast<UI_TRIGGER_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Trigger::Attach_Personal_Info()
{
	switch (m_eSubClassType)
	{
	case DTO::EUITriggerSubClassType::MENU_TAB_TRIGGER:
		return S_OK;
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIMenu_Trigger::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIMenu_Trigger::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_ENTER))
	{
		Fire_ToTargets(ETriggerEventType::PRESS_ENTER);
	}
	else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_EXIT))
	{
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

void CUIMenu_Trigger::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMenu_Trigger::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMenu_Trigger::Ready_Before_Render(const _float fTimeDelta)
{

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMenu_Trigger::Render()
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

void CUIMenu_Trigger::Fire_ToTargets(ETriggerEventType eEvent)
{
	for (auto* pUI : m_pTriggerUI[ENUM_TO_UINT(eEvent)])
		if (pUI) pUI->OnUIEvent(eEvent, this);

	for (auto* pCanvas : m_pTriggerCanvas[ENUM_TO_UINT(eEvent)])
		if (pCanvas) pCanvas->OnCanvasEvent(eEvent, this);
}

void CUIMenu_Trigger::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	switch (eEvent)
	{
	case Client::ETriggerEventType::HOVER_ENTER:
		break;
	case Client::ETriggerEventType::HOVER_EXIT:
		break;
	case Client::ETriggerEventType::PRESS_ENTER:
		if (m_isVisible)
			Set_Invisible();
		else
			Set_Visible();

		if (m_isInteract)
			Set_NonInteractable();
		else
			Set_Interactable();

		break;
	case Client::ETriggerEventType::PRESS_EXIT:
		break;
	case Client::ETriggerEventType::END:
	default:
		break;
	}
}

HRESULT CUIMenu_Trigger::Ready_Components(UI_TRIGGER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIMenu_Trigger::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();

	return S_OK;
}

CUIMenu_Trigger* CUIMenu_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMenu_Trigger* pInstance = new CUIMenu_Trigger(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMenu_Trigger::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMenu_Trigger::Clone(void* pArg)
{
	CUIMenu_Trigger* pInstance = new CUIMenu_Trigger(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMenu_Trigger::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMenu_Trigger::Free()
{
	Super::Free();
}
