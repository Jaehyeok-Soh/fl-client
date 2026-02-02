#include "pch.h"
#include "Level_Assimp.h"
#include "UEMapdataParser.h"
#include "Importer.h"
#include "Converter.h"
#include "ImGui_ToolManager.h"
#include "Panel_ModelConverter.h"


CLevel_Assimp::CLevel_Assimp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
	m_arrayImGuiPanel.fill(nullptr);
}

HRESULT CLevel_Assimp::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Assimp::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;


	if (FAILED(Ready_GUI()))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_Assimp::Ready_GUI()
{
	CImGui_Panel* pPanel{ nullptr };

	pPanel = CPanel_ModelConverter::Create("Model Converter", this, m_pDevice, m_pDeviceContext);

	if (!pPanel) return E_FAIL;
	else		m_arrayImGuiPanel[ENUM_TO_SZET(CLevel_Assimp::Elements::ModelConverter)] = pPanel;


	return S_OK;
}



void CLevel_Assimp::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CLevel_Assimp::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	m_pImGuiManager->Render_Begin();
	m_pImGuiManager->Render_Dockspace();
	//////////////////////////
	// Element Render

	for (auto& Panel : m_arrayImGuiPanel)
		if (Panel)
			Panel->Render(nullptr);


	//////////////////////////
	m_pImGuiManager->Render_End();

	return S_OK;
}

CLevel_Assimp* CLevel_Assimp::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Assimp* pInstance = new CLevel_Assimp(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Assimp::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Assimp::Free()
{
	for (auto& Panel : m_arrayImGuiPanel)
		Safe_Release(Panel);


	Safe_Release(m_pImGuiManager);
	Super::Free();
}
