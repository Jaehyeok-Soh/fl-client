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
	const auto& doc = static_cast<const CDataDocument_Map&>(document);
	// For. StaticModel
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::MAPOBJECT));
		for (const auto& pObjectData : vecList)
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
	tDesc.eClientLevelType					= static_cast<EClientLevelType>(tData.eClientLevelType);
	tDesc.eClientMakePath					= static_cast<EClientMakePath>(tData.eClientMakePath);
	tDesc.eMapObjectDrawType				= static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);

	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.isLoaded = true;
	
	
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
