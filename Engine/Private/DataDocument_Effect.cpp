#include "Engine_pch.h"
#include "DataDocument_Effect.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

CDataDocument_Effect::CDataDocument_Effect()
{
}

HRESULT CDataDocument_Effect::Initialize()
{
	return S_OK;
}

HRESULT CDataDocument_Effect::Try_Add(const DTO::TEFFECT_ContainerData& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EEffectType::EFFECT_CONTAINER);
	static_cast<CEFFECT_CONTAINER*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

IObjectDataBase* CDataDocument_Effect::Create_ObjectData(DTO::EEffectType eType)
{
	switch (eType)
	{
	case DTO::EEffectType::EFFECT_CONTAINER:
		return CEFFECT_CONTAINER::Create();
	default:
		return nullptr;
	}
}

HRESULT CDataDocument_Effect::Try_Add(IObjectDataBase* pObject)
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
json CDataDocument_Effect::ToJson() const
{
	json j;
	j["Category"] = DTO::ECategory::EFFECT;

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
HRESULT CDataDocument_Effect::FromJson(const json& j)
{
	Clear();

	if (j.contains("Category"))
	{
		const DTO::ECategory eCategory = j.at("Category").get<DTO::ECategory>();
		if (eCategory != DTO::ECategory::EFFECT)
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

		const DTO::EEffectType eType = object.at("Type").get<DTO::EEffectType>();

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

CDataDocument_Effect* CDataDocument_Effect::Create()
{
	CDataDocument_Effect* pInstance = new CDataDocument_Effect();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CDataDocument_Effect::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDataDocument_Effect::Free()
{
	Super::Free();
}
