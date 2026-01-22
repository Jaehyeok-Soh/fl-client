#include "Tool_Defines.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
#include "Level_UI.h"

//==========
// UI
//==========
#include "UI_Inspector.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	m_GuiElements.fill(nullptr);
}

HRESULT CLevel_UI::Initialize()
{
	return S_OK;
}

HRESULT CLevel_UI::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("UI");

	if (FAILED(Ready_UI_Inspector()))
		return E_FAIL;	

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
	CImGui_Base* p = CUI_Inspector::Create("UI_Inspector", CLevel_Loading::Create(m_pDevice, m_pDeviceContext, ELevelType::UI), m_pDevice, m_pDeviceContext);
	if (nullptr == p)
		return E_FAIL;	

	m_GuiElements[ENUM_TO_SZET(Elements::INSPECTOR)] = p;
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
	Safe_Release(m_pImGuiManager);

	for (CImGui_Base* pElement : m_GuiElements)
	{
		Safe_Release(pElement);
	}
	m_GuiElements.fill(nullptr);

	Super::Free();
}
