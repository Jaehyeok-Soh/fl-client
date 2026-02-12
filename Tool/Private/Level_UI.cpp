#include "pch.h"
#include "Level_UI.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
//==========
// ImGui
//==========
#include "UI_Maker.h"
#include "UI_Hierachy.h"
#include "UI_Inspector.h"
#include "UIComponent_Inspector.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	m_GuiElements.fill(nullptr);
}

HRESULT CLevel_UI::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_UI::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_UI_Inspector()))
		return E_FAIL;	

	if (FAILED(Ready_UI_Object(L"Layer_UI_Object")))
		return E_FAIL;

	ImGui::StyleColorsClassic();
	return S_OK;
}

void CLevel_UI::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	for (CImGui_Base* pElement : m_GuiElements)
	{
		if (pElement)
			pElement->Update(fTimeDelta);
	}
}

HRESULT CLevel_UI::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	for (CImGui_Base* pElement : m_GuiElements)
	{
		if (pElement)
			pElement->Render(nullptr);
	}

	//////////////////////////
	m_pImGuiManager->Render_Viewport(nullptr);
	m_pImGuiManager->Render_End();

	return S_OK;
}

HRESULT CLevel_UI::Ready_UI_Inspector()
{
	m_GuiElements[ENUM_TO_SZET(Elements::UIMAKER)]			= CUI_Maker::Create("[[ UI Creater ]]", this, m_pDevice, m_pDeviceContext);
	m_GuiElements[ENUM_TO_SZET(Elements::INSPECTOR)]		= CUI_Inspector::Create("[[ UI Inspector ]]", this, m_pDevice, m_pDeviceContext);
	m_GuiElements[ENUM_TO_SZET(Elements::HIERACHY)]			= CUI_Hierachy::Create("[[ UI List Viewer ]]", this, m_pDevice, m_pDeviceContext);
	m_GuiElements[ENUM_TO_SZET(Elements::UICOM_ISPECTOR)]	= CUIComponent_Inspector::Create("[[ Component Inspector ]]", this, m_pDevice, m_pDeviceContext);

	return S_OK;
}

HRESULT CLevel_UI::Ready_UI_Object(const _wstring& wstrLayerTag)
{
	return S_OK;
}

CLevel_UI* CLevel_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_UI* pInstance = new CLevel_UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_UI::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_UI::Free()
{
	ImGui::StyleColorsDark();

	Safe_Release(m_pImGuiManager);

	for (CImGui_Base* pElement : m_GuiElements)
	{
		Safe_Release(pElement);
	}
	m_GuiElements.fill(nullptr);

	Super::Free();
}
