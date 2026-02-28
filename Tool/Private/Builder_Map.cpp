#include "pch.h"
#include "Builder_Map.h"
#include "MapToolManager.h"
#include "MapObject.h"
#include "DataStruct_Map.h"
#include "GameInstance.h"

CBuilder_Map::CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: CBuilderBase(pDevice,pDeviceContext,iLevelID) , m_pGameInstance(CGameInstance::GetInstance()) , m_pMapToolManager(CMapToolManager::GetInstance())
{
}

HRESULT	CBuilder_Map::Initialize()
{
	return S_OK;
}


HRESULT CBuilder_Map::Build(const CDataDocumentBase& document)
{
	m_eClientLevelType = EClientLevelType::STATIC;

	const auto& doc = static_cast<const CDataDocument_Map&>(document);

	/* Level Data 먼지 필수 */
	{
		const vector<Engine::IObjectDataBase*> vecSceneData = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::SCENEDATA));
		for (const auto& pObjectData : vecSceneData)
		{
			pObjectData->Get_Type();
			const auto* pMapObjectData = static_cast<const Engine::CData_LevelData*>(pObjectData);

			if (FAILED(Apply_ScenceData(pMapObjectData->Get_Data())))
				return E_FAIL;
		}
	}

	{
		const vector<Engine::IObjectDataBase*> vecMapObjectList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::MAPOBJECT));
		for (const auto& pObjectData : vecMapObjectList)
		{
			pObjectData->Get_Type();
			const auto* pMapObjectData = static_cast<const Engine::CData_MapObject*>(pObjectData);
			
			if (FAILED(Create_MapObject(pMapObjectData->Get_Data())))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CBuilder_Map::Create_MapObject(const DTO::TMap_MapObjectData& tData)
{

	CMapObject::MAPOBJECT_DESC tDesc{};
	tDesc.eClientLevelType					= m_eClientLevelType;
	tDesc.eClientMakePath					= static_cast<EClientMakePath>(tData.eClientMakePath);
	tDesc.eMapObjectDrawType				= static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);

	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.isLoaded = true;
	
	tDesc.iSectionNumber					= tData.iSectionNum;
	tDesc.tUsingModelInfo.wstrName			= path(tData.strModelPath).filename().stem();
	tDesc.tUsingModelInfo.wstrPath			= Engine_Utils::ToWString(tData.strModelPath);
	tDesc.vecClientMakePathDesc				= tData.vecClientMakePathDesc;
	tDesc.wstrLayerTag						= g_wszMapObjectLayer;

	for (auto& DTO_SRTDATA : tData.vecSRTs)
	{
		SRT_DATA tSRT{};
		tSRT.Update_SRT(DTO_SRTDATA.vScale , DTO_SRTDATA.vQuat , DTO_SRTDATA.vPosition);
		tSRT.vScale_Isolated = DTO_SRTDATA.vScale_Isolated;
		tDesc.vecSRTs.push_back(tSRT);
	}

	/* 복사생성해서 넣어준다 */
	m_pMapToolManager->Make_MapObject( &tDesc , false );

	return S_OK;
}

HRESULT CBuilder_Map::Apply_ScenceData(const DTO::TLevelData& tData)
{
	m_eClientLevelType = StringToClientleveltype(tData.strLevelTypeName);


	/* None 이면 사용하지 않는 다는 뜻 */
	m_pMapToolManager->Apply_LevelData(&tData);

	return S_OK;
}


CBuilder_Map* CBuilder_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Map* pBuilder = new CBuilder_Map(pDevice,pDeviceContext,iLevelID);

	if (FAILED(pBuilder->Initialize()))
	{
		Safe_Release(pBuilder);
		MSG_BOX(" Builder Map is Failed To Create ");
		return nullptr;
	}

	return pBuilder;
}

void CBuilder_Map::Free()
{
	Super::Free();
}
