#include "pch.h"
#include "Level_Logo.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
	, m_vecCCS_EventManifest{}
{
	Safe_AddRef(m_pImGuiManager);
}

HRESULT CLevel_Logo::Initialize()
{
	m_pGameInstance->Request_CursorMode(ECursorMode::Tool);

	if (FAILED(Ready_CCS_EventManifest()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_CCS_EventManifest()
{
	if (FAILED(Load_CCS_EventManifest(&m_vecCCS_EventManifest)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;




	return S_OK;
}

void CLevel_Logo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CLevel_Logo::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->ImGuizmo_Render_Begin();
	m_pImGuiManager->Render_Dockspace();


	if (FAILED(Render_CCS_Setting()))
		return E_FAIL;

	//////////////////////////
	// Element Render

	//////////////////////////
	m_pImGuiManager->Render_Viewport(nullptr);
	m_pImGuiManager->Render_End();
	return S_OK;
}
HRESULT CLevel_Logo::Render_CCS_Setting()
{
	ImGui::Separator();

	ImGui::Begin(" CSS Setting Window ");

	static bool bShowManifestEditor = false;
	if (ImGui::Button("Open Event Manifest Editor"))
	{
		bShowManifestEditor = !bShowManifestEditor; // 누를 때마다 On/Off
	}

	if (bShowManifestEditor)
	{
		Render_ImGui_CSS_EventManifest(&m_vecCCS_EventManifest, &bShowManifestEditor);
	}

	ImGui::Separator();


	ImGui::End();

	return S_OK;
}
CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Logo::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	Safe_Release(m_pImGuiManager);
	Super::Free();
}
