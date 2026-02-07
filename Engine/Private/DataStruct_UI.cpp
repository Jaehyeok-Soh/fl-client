#include "Engine_pch.h"
#include "DataStruct_UI.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

void to_json(json& j, const TUI_EventBindData& data)
{
	j = json
	{
		{ "Type", TUI_EventBindData::eType },
		{ "strTag", data.strTag },
		{"strOwnerTag", data.strOwnerTag},
		{"strTargetTag", data.strTargetTag},
		{"eEvent", data.eEvent},
		{"eAction", data.eAction},
		{"Params", data.Params}
	};
}
void from_json(const json& j, TUI_EventBindData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("strOwnerTag").get_to(data.strOwnerTag);
	j.at("strTargetTag").get_to(data.strTargetTag);
	j.at("eEvent").get_to(data.eEvent);
	j.at("eAction").get_to(data.eAction);
	j.at("Params").get_to(data.Params);
}
void to_json(json& j, const TUI_GenericUIData& data)
{
	j = json
	{
		{ "Type", TUI_GenericUIData::eType },
		{ "strTag", data.strTag },
		{ "strCanvasName", data.strCanvasName },

		{ "iRectTransformType", data.iRectTransformType },
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },
		{ "strTextureTag", data.strTextureTag },
		{ "iTextureIndex", data.iTextureIndex },
		{ "isVisible", data.isVisible },
	};
}
void from_json(const json& j, TUI_GenericUIData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("strCanvasName").get_to(data.strCanvasName);
	j.at("iRectTransformType").get_to(data.iRectTransformType);
	j.at("fWidth").get_to(data.fWidth);
	j.at("fHeight").get_to(data.fHeight);
	j.at("fPosX").get_to(data.fPosX);
	j.at("fPosY").get_to(data.fPosY);
	j.at("fPosZ").get_to(data.fPosZ);
	j.at("strTextureTag").get_to(data.strTextureTag);
	j.at("iTextureIndex").get_to(data.iTextureIndex);
	j.at("isVisible").get_to(data.isVisible);
}
void to_json(json& j, const TUI_CanvasData& data)
{
	j = json
	{
		{ "Type", TUI_CanvasData::eType },
		{ "strTag", data.strTag },
		{ "iLevelIndex", data.iLevelIndex },
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },
		{ "iEditorSizeX", data.iEditorSizeX },
		{ "iEditorSizeY", data.iEditorSizeY },
	};
}
void from_json(const json& j, TUI_CanvasData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("iLevelIndex").get_to(data.iLevelIndex);
	j.at("fWidth").get_to(data.fWidth);
	j.at("fHeight").get_to(data.fHeight);
	j.at("fPosX").get_to(data.fPosX);
	j.at("fPosY").get_to(data.fPosY);
	j.at("fPosZ").get_to(data.fPosZ);
	j.at("iEditorSizeX").get_to(data.iEditorSizeX);
	j.at("iEditorSizeY").get_to(data.iEditorSizeY);
}

NS_END


NS_BEGIN(Engine)

json CUI_GenericUI_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_GenericUI_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_GenericUIData>();
	return S_OK;
}

json CUI_Canvas_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_Canvas_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_CanvasData>();
	return S_OK;
}

json CUI_EventBindData_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_EventBindData_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_EventBindData>();
	return S_OK;
}

NS_END;