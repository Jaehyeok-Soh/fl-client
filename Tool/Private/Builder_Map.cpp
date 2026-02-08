#include "pch.h"
#include "Builder_Map.h"
#include "MapToolManager.h"
#include "MapObject.h"
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
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::STATICMODEL));
		for (const auto& pObjectData : vecList)
		{
			pObjectData->Get_Type();
			const auto* pStaticModelData = static_cast<const Engine::CData_StaticModel*>(pObjectData);

		}
	}
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
