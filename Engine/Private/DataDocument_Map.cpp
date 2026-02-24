#include "Engine_pch.h"
#include "DataDocument_Map.h"



CDataDocument_Map::CDataDocument_Map()
{
}

CDataDocument_Map::~CDataDocument_Map()
{
}

HRESULT CDataDocument_Map::Initialize()
{
	return S_OK;
}

json CDataDocument_Map::ToJson() const
{
	json j;
	j["Category"] = DTO::ECategory::MAP;

	IObjectDataBase* pSceneData{nullptr};

	json jsonArray = json::array();
	for (const auto& [iType, umapTags] : m_Datas)
		for (const auto& [strTag, object] : umapTags)
		{
			if (ENUM_TO_UINT(DTO::EMapObject_Type::SCENEDATA) != object->Get_Type())
				jsonArray.push_back(object->ToJson());
			else
				pSceneData = object;
		}

	if(pSceneData)
		j["Level Data"] = pSceneData->ToJson();


	j["Objects"] = std::move(jsonArray);
	return j;
}

HRESULT CDataDocument_Map::FromJson(const json& j)
{
	Clear();

	if (!j.contains("Category")) return E_FAIL;

	if (j["Category"].get<DTO::ECategory>() != DTO::ECategory::MAP) return E_FAIL;

	if (!j.contains("Objects")) return E_FAIL;
	if (!j["Objects"].is_array()) return E_FAIL;



	if (j.contains("Level Data"))
	{
		CData_LevelData* pSceneData = CData_LevelData::Create();

		if (FAILED(pSceneData->FromJson(j["Level Data"])))
		{
			Safe_Release(pSceneData);
			return E_FAIL;
		}

		if (FAILED(Try_Add(pSceneData->Get_Data())))
			return E_FAIL;

	}


	for (const auto& object : j["Objects"])
	{
		//if (object.contains("Client Make Path") == false)
		//	return E_FAIL;

		//const DTO::EMapObject_Type eType = object.at("Type").get<DTO::EMapObject_Type>();

		IObjectDataBase* pObjectDataBase = CData_MapObject::Create();
		if (pObjectDataBase == nullptr)
			return E_FAIL;

		if (FAILED(pObjectDataBase->FromJson(object)))
		{
			Safe_Release(pObjectDataBase);
			return E_FAIL;
		}

		if (FAILED(Try_Add(pObjectDataBase)))
		{
			Safe_Release(pObjectDataBase);
			return E_FAIL;
		}
	}


	return S_OK;
}


HRESULT CDataDocument_Map::Try_Add(const DTO::TMap_MapObjectData& data)
{
	CData_MapObject* pObjectBase = CData_MapObject::Create();
	pObjectBase->Get_Data() = data;
	return Try_Add(pObjectBase);
}

HRESULT CDataDocument_Map::Try_Add(const DTO::TLevelData& data)
{
	CData_LevelData* pObjectBase = CData_LevelData::Create();
	pObjectBase->Get_Data() = data;
	return Try_Add(pObjectBase);
}




HRESULT CDataDocument_Map::Try_Add(IObjectDataBase* pObject)
{
	if (pObject == nullptr)
		return E_FAIL;

	const string& strTag = pObject->Get_Tag();
	if (strTag.empty() == true)
	{
		Safe_Release(pObject);
		return E_FAIL;
	}

	if (m_AllTags.find(strTag) != m_AllTags.end())
	{
		Safe_Release(pObject);
		return E_FAIL;
	}

	m_AllTags.insert(strTag);
	const _uint iType = pObject->Get_Type();
	m_Datas[iType].emplace(strTag, pObject);
	return S_OK;
}

CDataDocument_Map* CDataDocument_Map::Create()
{
	CDataDocument_Map* pMap = new CDataDocument_Map;

	if (FAILED(pMap->Initialize()))
	{
		Safe_Release(pMap);
		MSG_BOX(" Data Document Map is failed to Create ");
		return nullptr;
	}

	return pMap;
}



void CDataDocument_Map::Free()
{
	Super::Free();

	return;
}

