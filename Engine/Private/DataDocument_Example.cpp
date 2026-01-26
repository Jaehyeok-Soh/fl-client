#include "Engine_pch.h"
#include "DataDocument_Example.h"

CDataDocument_Example::CDataDocument_Example()
{
}

HRESULT CDataDocument_Example::Initialize()
{
	return S_OK;
}

HRESULT CDataDocument_Example::Try_Add(const DTO::TExample_LightData& data)
{
	DTO::IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EMapType::LIGHT);
	static_cast<CExample_LightData*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

HRESULT CDataDocument_Example::Try_Add(const DTO::TExample_StaticModelData& data)
{
	DTO::IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EMapType::STATICMODEL);
	static_cast<CExample_StaticModel*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

DTO::IObjectDataBase* CDataDocument_Example::Create_ObjectData(DTO::EMapType eType)
{
	switch (eType)
	{
	case DTO::EMapType::STATICMODEL:
		return CExample_StaticModel::Create();
	case DTO::EMapType::LIGHT:
		return CExample_LightData::Create();
	default:
		return nullptr;
	}
}

HRESULT CDataDocument_Example::Try_Add(DTO::IObjectDataBase* pObject)
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

/// <summary>
/// <para>MapType의 ToJson</para>
/// 구조체의 Category를 Map으로 강제화
/// </summary>
/// <param name="j"></param>
/// <returns></returns>
json CDataDocument_Example::ToJson() const
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

/// <summary>
/// <para>MapType의 FromJson</para>
/// 구조체의 Category를 검사 후 맞지 않고
/// 저장한 Objects 데이터가 없거나 배열이 아니라면 실패
/// </summary>
/// <param name="j"></param>
/// <returns></returns>
HRESULT CDataDocument_Example::FromJson(const json& j)
{
	Clear();

	if (j.contains("Category"))
	{
		const DTO::ECategory eCategory = j.at("Category").get<DTO::ECategory>();
		if (eCategory != DTO::ECategory::MAP)
			return E_FAIL;
	}
	else
		return E_FAIL;

	if ((j.contains("Objects") == false) || (j["Objects"].is_array() == false))
		return E_FAIL;

	for (const auto& object : j["Objects"])
	{
		if (object.contains("Type") == false)
			return E_FAIL;

		const DTO::EMapType eType = object.at("Type").get<DTO::EMapType>();

		DTO::IObjectDataBase* pObjectDataBase = Create_ObjectData(eType);
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

CDataDocument_Example* CDataDocument_Example::Create()
{
	CDataDocument_Example* pInstance = new CDataDocument_Example();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CDataDocument_Example::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDataDocument_Example::Free()
{
	Super::Free();
}
