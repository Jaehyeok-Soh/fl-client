#include "pch.h"
#include "UITrigger.h"
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

CUITrigger::CUITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUITrigger::CUITrigger(const CUITrigger& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUITrigger::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITrigger::Initialize(void* pArg)
{
	UI_TRIGGER_DESC* pDesc = static_cast<UI_TRIGGER_DESC*>(pArg);
	m_eSubClassType = pDesc->eTriggerSubClass;
	m_tTriggerData = std::move(pDesc->tTriggerData);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITrigger::Bind_Cache(_uint iLevelID)
{
	// Hover Enter Canvas
	for (const _string& str : m_tTriggerData.vecHoverEnterTriggerCanvas)
	{
		auto* pCanvas = m_pUIManager->Find_Canvas(iLevelID, str);
		if (nullptr == pCanvas)
			return E_FAIL;

		m_pTriggerCanvas[ENUM_TO_UINT(ETriggerEventType::HOVER_ENTER)].push_back(pCanvas);
	}

	// Hover Exit Canvas
	for (const _string& str : m_tTriggerData.vecHoverExitTriggerCanvas)
	{
		auto* pCanvas = m_pUIManager->Find_Canvas(iLevelID, str);
		if (nullptr == pCanvas)
			return E_FAIL;

		m_pTriggerCanvas[ENUM_TO_UINT(ETriggerEventType::HOVER_EXIT)].push_back(pCanvas);
	}

	// Press Enter Canvas
	for (const _string& str : m_tTriggerData.vecPressEnterTriggerCanvas)
	{
		auto* pCanvas = m_pUIManager->Find_Canvas(iLevelID, str);
		if (nullptr == pCanvas)
			return E_FAIL;

		m_pTriggerCanvas[ENUM_TO_UINT(ETriggerEventType::PRESS_ENTER)].push_back(pCanvas);
	}

	// Press Exit Canvas
	for (const _string& str : m_tTriggerData.vecPressExitTriggerCanvas)
	{
		auto* pCanvas = m_pUIManager->Find_Canvas(iLevelID, str);
		if (nullptr == pCanvas)
			return E_FAIL;

		m_pTriggerCanvas[ENUM_TO_UINT(ETriggerEventType::PRESS_EXIT)].push_back(pCanvas);
	}

	// Hover Enter UI
	for (const _string& str : m_tTriggerData.vecHoverEnterTriggerUI)
	{
		auto* pUI = m_pUIManager->Find_GenericUI(iLevelID, str);
		if (nullptr == pUI)
			return E_FAIL;

		m_pTriggerUI[ENUM_TO_UINT(ETriggerEventType::HOVER_ENTER)].push_back(pUI);
	}

	// Hover Exit UI
	for (const _string& str : m_tTriggerData.vecHoverExitTriggerUI)
	{
		auto* pUI = m_pUIManager->Find_GenericUI(iLevelID, str);
		if (nullptr == pUI)
			return E_FAIL;

		m_pTriggerUI[ENUM_TO_UINT(ETriggerEventType::HOVER_EXIT)].push_back(pUI);
	}

	// Press Enter UI
	for (const _string& str : m_tTriggerData.vecPressEnterTriggerUI)
	{
		auto* pUI = m_pUIManager->Find_GenericUI(iLevelID, str);
		if (nullptr == pUI)
			return E_FAIL;

		m_pTriggerUI[ENUM_TO_UINT(ETriggerEventType::PRESS_ENTER)].push_back(pUI);
	}

	// Press Exit UI
	for (const _string& str : m_tTriggerData.vecPressExitTriggerUI)
	{
		auto* pUI = m_pUIManager->Find_GenericUI(iLevelID, str);
		if (nullptr == pUI)
			return E_FAIL;

		m_pTriggerUI[ENUM_TO_UINT(ETriggerEventType::PRESS_EXIT)].push_back(pUI);
	}

	return S_OK;
}

HRESULT CUITrigger::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUITrigger::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITrigger::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUITrigger::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITrigger::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITrigger::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}


void CUITrigger::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
}

void CUITrigger::Check_FinEvent(ETriggerEventType eEvent)
{
	if (eEvent == ETriggerEventType::END || eEvent == ETriggerEventType::HOVER_ENTER || eEvent == ETriggerEventType::HOVER_EXIT)
		return;

	for (const auto* pTrigger : m_pTriggerUI[ENUM_TO_UINT(eEvent)])
	{
		if (!pTrigger->Get_FinEvent())
		{
			m_isInteract = false;
			return;
		}
	}

	for (auto* pCanvas : m_pTriggerCanvas[ENUM_TO_UINT(eEvent)])
	{
		if (!pCanvas->Check_FinEvent())
		{
			m_isInteract = false;
			return;
		}
	}
	m_eCurTriggerType = ETriggerEventType::END;
	m_isInteract = true;
}

HRESULT CUITrigger::Ready_Components(UI_TRIGGER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUITrigger::Bind_ShaderResources()
{
	return S_OK;
}

void CUITrigger::Free()
{
	Super::Free();
}
