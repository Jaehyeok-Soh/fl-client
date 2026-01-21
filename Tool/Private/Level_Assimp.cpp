#include "Tool_Defines.h"
#include "UEMapdataParser.h"
#include "Importer.h"
#include "Converter.h"
#include "ImGui_ToolManager.h"
#include "Level_Assimp.h"

CLevel_Assimp::CLevel_Assimp(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
	, m_pImGuiManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pImGuiManager);
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

	MSG_BOX("Assimp");
	
	
#pragma region Custom
	_fmatrix matUECoord = ::XMMatrixSet(
		1.f, 0.f, 0.f, 0.f,		// x' = x
		0.f, 0.f, 1.f, 0.f,		// y' = z
		0.f, -1.f, 0.f, 0.f,	// z' = -y
		0.f, 0.f, 0.f, 1.f
	);

	_matrix matPreTransformScaling10 = ::XMMatrixScaling(0.1f, 0.1f, 0.1f);
	_matrix matPreTransformScaling100 = ::XMMatrixScaling(0.01f, 0.01f, 0.01f);
	_matrix matPreTransformScaling1000 = ::XMMatrixScaling(0.001f, 0.001f, 0.001f);

	_float4x4 matPreTransformIdentity = {};
	_float4x4 matPreTransformScaling = {};
	_float4x4 matPreTransformMapObject = {};
	_bool IsCustom = true;
	::XMStoreFloat4x4(&matPreTransformIdentity, ::XMMatrixIdentity());
	::XMStoreFloat4x4(&matPreTransformScaling, ::XMMatrixScaling(0.001f, 0.001f, 0.001f));
	::XMStoreFloat4x4(&matPreTransformMapObject, matUECoord * ::XMMatrixScaling(0.01f, 0.01f, 0.01f));
	vector<AS_BONE*> vecMasterBones;

	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/AirRibons/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Circle/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/CircleDetail/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Grass/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Lines/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Rasengan/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000 * ::XMMatrixRotationRollPitchYaw(0.f, ::XMConvertToRadians(-90.f), 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/RasenganBigTornado/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100 * ::XMMatrixRotationRollPitchYaw(::XMConvertToRadians(90.f), 0.f, 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/WaterPrison/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};

		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100 * ::XMMatrixRotationRollPitchYaw(::XMConvertToRadians(-90.f), 0.f, 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Fireball/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};

		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100 * ::XMMatrixRotationRollPitchYaw(0.f, ::XMConvertToRadians(-90.f), 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Lightning/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling10 * ::XMMatrixRotationRollPitchYaw(0.f, ::XMConvertToRadians(-90.f), 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/RasenganSmallTornado/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100 * ::XMMatrixRotationRollPitchYaw(0.f, 0.f, ::XMConvertToRadians(90.f)));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Rings/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/SmokeMesh/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Sphere/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Torus/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Tree/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling1000);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/Wire/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100);
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/PainSkill/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100 * ::XMMatrixRotationRollPitchYaw(::XMConvertToRadians(90.f), 0.f, 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/RasenShuriken/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
	{
		_float4x4 matPreTrasform = {};
		::XMStoreFloat4x4(&matPreTrasform, matPreTransformScaling100 * ::XMMatrixRotationRollPitchYaw(::XMConvertToRadians(90.f), ::XMConvertToRadians(180.f), 0.f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Effects/SharkBomb/", matPreTrasform, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}
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
	
	/*{
		_float4x4 matPreTransform = {};
		::XMStoreFloat4x4(&matPreTransform, ::XMMatrixScaling(0.001f, 0.001f, 0.001f));
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"MapAssets", matPreTransform, ConvertType::MAPOBJECT);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}*/
	
	{
		CConverter* pConverter = CConverter::Create(m_pDevice, m_pDeviceContext, L"Map/AmeVillage/", matPreTransformMapObject, false);
		pConverter->ReadAndExportFile();
		Safe_Release(pConverter);
	}

	////Map
	//{
	//	CUEMapdataParser::MAPPARSER_DESC desc = {};
	//	desc.wstrPath = L"../../Resources/Data/MapData/Village/Village_Floor.json";
	//	CUEMapdataParser* pParser = CUEMapdataParser::Create(desc);
	//	if (!pParser)
	//		return E_FAIL;
	//	if (FAILED(pParser->Read_Mapdata()))
	//		return E_FAIL;
	//	if (FAILED(pParser->Write_Mapdata()))
	//		return E_FAIL;
	//	Safe_Release(pParser);
	//}
	//{
	//	CUEMapdataParser::MAPPARSER_DESC desc = {};
	//	desc.wstrPath = L"../../Resources/Data/MapData/ExamStadium/ExamStadium_Env_Terrain.json";
	//	CUEMapdataParser* pParser = CUEMapdataParser::Create(desc);
	//	if (!pParser)
	//		return E_FAIL;
	//	if (FAILED(pParser->Read_Mapdata()))
	//		return E_FAIL;
	//	if (FAILED(pParser->Write_Mapdata()))
	//		return E_FAIL;
	//	Safe_Release(pParser);
	//}
	//{
	//	CUEMapdataParser::MAPPARSER_DESC desc = {};
	//	desc.wstrPath = L"../../Resources/Data/MapData/Village/Village_Env_Terrain.json";
	//	CUEMapdataParser* pParser = CUEMapdataParser::Create(desc);
	//	if (!pParser)
	//		return E_FAIL;
	//	if (FAILED(pParser->Read_Mapdata()))
	//		return E_FAIL;
	//	if (FAILED(pParser->Write_Mapdata()))
	//		return E_FAIL;
	//	Safe_Release(pParser);
	//}
	
	if (vecMasterBones.size() >= 0)
	{
		for (auto& pElement : vecMasterBones)
			Safe_Delete(pElement);
	}
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
