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

	json jsonArray = json::array();
	for (const auto& [iType, umapTags] : m_Datas)
		for (const auto& [strTag, object] : umapTags)
			jsonArray.push_back(object->ToJson());

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

	for (const auto& object : j["Objects"])
	{
		if (object.contains("Type") == false)
			return E_FAIL;

		const DTO::EMapObject_Type eType = object.at("Type").get<DTO::EMapObject_Type>();

		IObjectDataBase* pObjectDataBase = Create_ObjectData(eType);
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

HRESULT CDataDocument_Map::Try_Add(const DTO::STATICMODEL_DATA& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EMapObject_Type::STATICMODEL);
	static_cast<CData_StaticModel*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

HRESULT CDataDocument_Map::Try_Add(const DTO::InstanceModel_Data& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EMapObject_Type::INSTANCEMODEL);
	static_cast<CData_InstanceModel*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}


IObjectDataBase* CDataDocument_Map::Create_ObjectData(DTO::EMapObject_Type eType)
{
	switch (eType)
	{
	case DTO::EMapObject_Type::STATICMODEL:
		return CData_StaticModel::Create();
	case DTO::EMapObject_Type::INSTANCEMODEL:
		return CData_InstanceModel::Create();
	default:
		return nullptr;
	}

	return nullptr;
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

