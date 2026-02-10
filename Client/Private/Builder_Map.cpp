#include "pch.h"
#include "Builder_Map.h"
#include "GameInstance.h"
#include "StaticObject.h"
#include "Bounds.h"
#include "DataDocument_Map.h"

CBuilder_Map::CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: CBuilderBase(pDevice, pDeviceContext, iLevelID), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT	CBuilder_Map::Initialize()
{

	return S_OK;
}


HRESULT CBuilder_Map::Build(const CDataDocumentBase& document)
{
	const auto& doc = static_cast<const CDataDocument_Map&>(document);
	// For. StaticObject
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::MAPOBJECT));
		for (const auto& pObjectData : vecList)
		{
			const auto* pStaticObjectData = static_cast<const Engine::CData_MapObject*>(pObjectData);
	
			DTO::TMap_MapObjectData tData = pStaticObjectData->Get_Data();

			DTO::EClientMakePath eClientMakePath = static_cast<DTO::EClientMakePath>(tData.eClientMakePath);

			switch (eClientMakePath)
			{
			case DTO::EClientMakePath::StaticObject:	Create_StaticObject(tData); break;
			default:									return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CBuilder_Map::Create_StaticObject(const DTO::TMap_MapObjectData& tData)
{
	CStaticObject::STATICOBJECT_DESC tStaticObjectDesc{};

	tStaticObjectDesc.iLevelIndex		 = tData.eClientLevelType;
	tStaticObjectDesc.isUELoaded		 = tData.isUELoaded;
	tStaticObjectDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tStaticObjectDesc.wstrModelPath		 = Engine_Utils::ToWString(tData.strModelPath);

	/* 여기서 Desc가 따로 필요한 객체라면 알잘딱 static_cast로 부여받기 */


	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tStaticObjectDesc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject( ENUM_TO_UINT(ELevelType::STATIC),L"Prototype_GameObject_StaticObject", tStaticObjectDesc.iLevelIndex , g_wszStaticObjectLayer,&tStaticObjectDesc);

	return S_OK;
}


CBuilder_Map* CBuilder_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Map* pBuilder = new CBuilder_Map(pDevice, pDeviceContext, iLevelID);

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
	Safe_Release(m_pGameInstance);


	Super::Free();
}