#include "Tool_Defines.h"
#include "Level_Loading.h"
#include "ImGui_ToolManager.h"
#include "GameInstance.h"
#include "Level_Animation.h"

CLevel_Animation::CLevel_Animation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
}

HRESULT CLevel_Animation::Initialize()
{
	return S_OK;
}

HRESULT CLevel_Animation::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	MSG_BOX("Animation");

	return S_OK;
}

void CLevel_Animation::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CLevel_Animation::Render()
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

HRESULT CLevel_Animation::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	return S_OK;
}

HRESULT CLevel_Animation::Ready_UI_Layer(const wstring& wstrLayerTag)
{
	return S_OK;
}

CLevel_Animation* CLevel_Animation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Animation* pInstance = new CLevel_Animation(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Animation::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Animation::Free()
{
	Safe_Release(m_pImGuiManager);
	Super::Free();
}
