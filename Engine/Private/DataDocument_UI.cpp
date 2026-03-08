#include "Engine_pch.h"
#include "DataDocument_UI.h"
#include "DataStruct_UI.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

CDataDocument_UI::CDataDocument_UI()
{
}

HRESULT CDataDocument_UI::Initialize()
{
	return S_OK;
}

HRESULT CDataDocument_UI::Try_Add(const DTO::TUI_CanvasData& data)
{
	/* ObjectDataBase를 생성, Data를 함수 인자로 채워줌 */
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EUIType::CANVAS);
	static_cast<CUI_Canvas_DTO*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

HRESULT CDataDocument_UI::Try_Add(const DTO::TUI_GenericUIData& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EUIType::GENERICUI);
	static_cast<CUI_GenericUI_DTO*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

HRESULT CDataDocument_UI::Try_Add(const DTO::TUI_TextData& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EUIType::UI_TEXT);
	static_cast<CUI_Text_DTO*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

HRESULT CDataDocument_UI::Try_Add(const DTO::TUI_DImageData& data)
{
	IObjectDataBase* pObjectBase = Create_ObjectData(DTO::EUIType::DYNAMIC_IMAGE);
	static_cast<CUI_DImage_DTO*>(pObjectBase)->Get_Data() = data;
	return Try_Add(pObjectBase);
}

IObjectDataBase* CDataDocument_UI::Create_ObjectData(DTO::EUIType eType)
{
	switch (eType)
	{
	case DTO::EUIType::CANVAS:
		return CUI_Canvas_DTO::Create();

	case DTO::EUIType::GENERICUI:
		return CUI_GenericUI_DTO::Create();

	case DTO::EUIType::UI_TEXT:
		return CUI_Text_DTO::Create();

	case DTO::EUIType::DYNAMIC_IMAGE:
		return CUI_DImage_DTO::Create();
	}
	return nullptr;
}

HRESULT CDataDocument_UI::Try_Add(IObjectDataBase* pObject)
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

/// <summary>
/// <para>UIType의 ToJson</para>
/// 구조체의 Category를 Map으로 강제화
/// </summary>
/// <param name="j"></param>
/// <returns></returns>
json CDataDocument_UI::ToJson() const
{
	json j;
	j["Category"] = DTO::ECategory::UI;

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
HRESULT CDataDocument_UI::FromJson(const json& j)
{
	Clear();

	if (j.contains("Category"))
	{
		const DTO::ECategory eCategory = j.at("Category").get<DTO::ECategory>();
		if (eCategory != DTO::ECategory::UI)
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

		const DTO::EUIType eType = object.at("Type").get<DTO::EUIType>();

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


CDataDocument_UI* CDataDocument_UI::Create()
{
	CDataDocument_UI* pInstance = new CDataDocument_UI();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CDataDocument_UI::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDataDocument_UI::Free()
{
	Super::Free();
}
