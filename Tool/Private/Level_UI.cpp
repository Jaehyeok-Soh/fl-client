#include "pch.h"
#include "Level_UI.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

//==========
// ImGui
//==========
#include "UI_Inspector.h"
#include "UI_Hierachy.h"

//==========
// UI
//==========
#include "ToolUI.h"

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

	if (FAILED(Ready_UI_Inspector()))
		return E_FAIL;	

	if (FAILED(Ready_UI_Object(L"Layer_UI_Object")))
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
	m_GuiElements[ENUM_TO_SZET(Elements::INSPECTOR)] = CUI_Inspector::Create("[[ UI Creater ]]", this, m_pDevice, m_pDeviceContext);
	m_GuiElements[ENUM_TO_SZET(Elements::HIERACHY)] = CUI_Hierachy::Create("[[ UI List Viewer ]]", this, m_pDevice, m_pDeviceContext);

	return S_OK;
}

HRESULT CLevel_UI::Ready_UI_Object(const _wstring& wstrLayerTag)
{
	CGameObject* pResult = { nullptr };


	//// Prototype_UI_Test_Button
	//CToolUI::TOOLUI_DESC Desc = {};
	//Desc.wstrTextureTag = L"Prototype_Component_Button_Test_Texture";
	//Desc.bAlpha = TRUE;
	//Desc.fSizeX = 1.f;
	//Desc.fSizeY = 1.f;
	//Desc.fX = 100.f;
	//Desc.fY = 100.f;
	//Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::UI);

	//pResult = m_pGameInstance->Add_GameObject(Desc.iLevelIndex, L"Prototype_UI_Test_Button", Desc.iLevelIndex, wstrLayerTag, &Desc);
	//if(nullptr == pResult)
	//	return E_FAIL;

	//if (FAILED(pResult->Awake(Desc.iLevelIndex)))
	//	return E_FAIL;

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
