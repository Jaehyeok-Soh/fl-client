#include "Engine_pch.h"
#include "DataDocument_AttackOverlap.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

CDataDocument_AttackOverlap::CDataDocument_AttackOverlap()
{
}

HRESULT CDataDocument_AttackOverlap::Initialize()
{
	return S_OK;
}

json CDataDocument_AttackOverlap::ToJson() const
{
	json j;
	j["Category"] = DTO::ECategory::OVERLAP_SCRIPT;

	json jsonArray = json::array();

	for (const auto& [iType, umapTags] : m_Datas)
		for (const auto& [strTag, object] : umapTags)
			jsonArray.push_back(object->ToJson());

	j["Objects"] = std::move(jsonArray);
	return j;
}

HRESULT CDataDocument_AttackOverlap::FromJson(const json& j)
{
	Clear();

	if (j.contains("Category"))
	{
		const DTO::ECategory eCategory = j.at("Category").get<DTO::ECategory>();
		if (eCategory != DTO::ECategory::OVERLAP_SCRIPT)
			return E_FAIL;
	}
	else
		return E_FAIL;

	if ((j.contains("Objects") == false) || (j["Objects"].is_array() == false))
		return E_FAIL;

	for (const auto& object : j["Objects"])
	{
		IObjectDataBase* pObjectDataBase = Create_ObjectData();
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

HRESULT CDataDocument_AttackOverlap::Try_Add(const DTO::ATTACKOVERLAP_DESC& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData();
	static_cast<CDataStruct_AttackOverlap*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

IObjectDataBase* CDataDocument_AttackOverlap::Create_ObjectData()
{
	return CDataStruct_AttackOverlap::Create();
}

HRESULT CDataDocument_AttackOverlap::Try_Add(IObjectDataBase* pObject)
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
	/* Type */
	const _uint iType = pObject->Get_Type();
	m_Datas[iType].emplace(strTag, pObject);
	return S_OK;
}

CDataDocument_AttackOverlap* CDataDocument_AttackOverlap::Create()
{
	CDataDocument_AttackOverlap* pInstance = new CDataDocument_AttackOverlap();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CDataDocument_AttackOverlap::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDataDocument_AttackOverlap::Free()
{
	Super::Free();
}
