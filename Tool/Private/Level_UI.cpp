#include "pch.h"
#include "Level_UI.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

CLevel_UI::CLevel_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
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

	return S_OK;
}

void CLevel_UI::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
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

	ImGui::ShowDemoWindow();
	//////////////////////////
	m_pImGuiManager->Render_Viewport(nullptr);
	m_pImGuiManager->Render_End();

	return S_OK;
}

HRESULT CLevel_UI::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	return S_OK;
}

HRESULT CLevel_UI::Ready_UI_Layer(const wstring& wstrLayerTag)
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
	Safe_Release(m_pImGuiManager);
	Super::Free();
}
