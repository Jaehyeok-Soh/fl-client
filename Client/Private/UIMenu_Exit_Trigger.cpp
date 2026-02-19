#include "pch.h"
#include "UIMenu_Exit_Trigger.h"
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

CUIMenu_Exit_Trigger::CUIMenu_Exit_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUITrigger(pDevice, pDeviceContext)
{
}

CUIMenu_Exit_Trigger::CUIMenu_Exit_Trigger(const CUIMenu_Exit_Trigger& rhs)
	:CUITrigger(rhs)
{
}

HRESULT CUIMenu_Exit_Trigger::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Exit_Trigger::Initialize(void* pArg)
{
	UI_MENU_EXIT_TRIGGER_DESC* pDesc = static_cast<UI_MENU_EXIT_TRIGGER_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Exit_Trigger::Attach_Personal_Info()
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

HRESULT CUIMenu_Exit_Trigger::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIMenu_Exit_Trigger::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_ENTER))
	{
		Fire_ToTargets(ETriggerEventType::PRESS_ENTER);
	}
}

void CUIMenu_Exit_Trigger::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMenu_Exit_Trigger::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMenu_Exit_Trigger::Ready_Before_Render(const _float fTimeDelta)
{

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMenu_Exit_Trigger::Render()
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

void CUIMenu_Exit_Trigger::Fire_ToTargets(ETriggerEventType eEvent)
{
	for (auto* pUI : m_pTriggerUI[ENUM_TO_UINT(eEvent)])
		if (pUI) pUI->OnUIEvent(eEvent, this);

	for (auto* pCanvas : m_pTriggerCanvas[ENUM_TO_UINT(eEvent)])
		if (pCanvas) pCanvas->OnCanvasEvent(eEvent, this);
}

void CUIMenu_Exit_Trigger::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
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

HRESULT CUIMenu_Exit_Trigger::Ready_Components(UI_MENU_EXIT_TRIGGER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIMenu_Exit_Trigger::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();

	return S_OK;
}

CUIMenu_Exit_Trigger* CUIMenu_Exit_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMenu_Exit_Trigger* pInstance = new CUIMenu_Exit_Trigger(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMenu_Exit_Trigger::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMenu_Exit_Trigger::Clone(void* pArg)
{
	CUIMenu_Exit_Trigger* pInstance = new CUIMenu_Exit_Trigger(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMenu_Exit_Trigger::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMenu_Exit_Trigger::Free()
{
	Super::Free();
}
