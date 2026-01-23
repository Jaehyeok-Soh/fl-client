#include "Tool_Defines.h"
#include "UEMapdataParser.h"
#include "Importer.h"
#include "Converter.h"
#include "ImGui_ToolManager.h"
#include "Panel_ModelConverter.h"
#include "Level_Assimp.h"

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


	MSG_BOX("Assimp");

#pragma region Custom
	Matrix matUECoord = ::XMMatrixSet(
		1.f, 0.f, 0.f, 0.f,		// x' = x
		0.f, 0.f, 1.f, 0.f,		// y' = z
		0.f, -1.f, 0.f, 0.f,	// z' = -y
		0.f, 0.f, 0.f, 1.f
	);

	Matrix matPreTransformScaling10 = Matrix::CreateScale(0.1f, 0.1f, 0.1f);
	Matrix matPreTransformScaling100 = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	Matrix matPreTransformScaling1000 = Matrix::CreateScale(0.001f, 0.001f, 0.001f);

	Matrix matPreTransformIdentity = Matrix::Identity;
	Matrix matPreTransformScaling = {};
	Matrix matPreTransformMapObject = matUECoord * matPreTransformScaling100;

	/*
	//Sword
	{
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Sword", matPreTransformIdentity);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	// Sheath
	{
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Sheath", matPreTransformIdentity);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	//Master
	{
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Master", matPreTransformScaling);
		pConverter->ReadAndExportFile();
		vecMasterBones = pConverter->Get_MasterBone();
		Safe_Release(pConverter);
	}
	// Pain
	{
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Pain", matPreTransformScaling);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	// Deidara
	{
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Deidara", matPreTransformScaling);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}

	{
	//////////
	// Head //
	//////////
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Custom/Head_FrogHead", matPreTransformIdentity, IsCustom);
		pConverter->Set_MatserBone(vecMasterBones);
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Head_Cap");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Head_Turban");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();
	///////////
	// Upper //
	///////////
		pConverter->Initialize(L"Custom/Upper_TShirt");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Upper_Anbu");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Upper_SandVest");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();
	////////////
	// Oneset //
	////////////
		pConverter->Initialize(L"Custom/Oneset_Santa");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Oneset_Beach");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Oneset_Beach2");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

	///////////
	// Lower //
	///////////
		pConverter->Initialize(L"Custom/Lower_Anbu");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Lower_Normal");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Lower_Default");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Lower_SandPants");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Lower_TuckedPants");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();
	//////////
	// Face //
	//////////
		pConverter->Initialize(L"Custom/Face_Normal");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Face_Mask1");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Face_Mask2");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();

		pConverter->Initialize(L"Custom/Face_Mask3");
		pConverter->ReadAndExportFile();
		pConverter->Clear_For_Custom();
		Safe_Release(pConverter);
	}
	*/
#pragma endregion
	
	// Map
	//{
	//	CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Map/Test/", matPreTransformMapObject, false);
	//	pConverter->ReadAndExportFile();
	//	Safe_Release(pConverter);
	//}

	//// Map Parsing
	//{
	//	CUEMapdataParser::MAPPARSER_DESC desc = {};
	//	desc.wstrPath = L"../../Resources/Data/MapData/Prologue/Prologue_Village_Art_0101BigObjs.json";
	//	CUEMapdataParser* pParser = CUEMapdataParser::Create(desc);
	//	if (!pParser)
	//		return E_FAIL;
	//	if (FAILED(pParser->Read_Mapdata()))
	//		return E_FAIL;
	//	if (FAILED(pParser->Write_Mapdata()))
	//		return E_FAIL;
	//	Safe_Release(pParser);
	//}

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
	Safe_Release(m_pImGuiManager);
	Super::Free();
}
