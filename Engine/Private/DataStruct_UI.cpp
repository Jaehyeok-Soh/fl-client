#include "Engine_pch.h"
#include "DataStruct_UI.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)



void to_json(json& j, const TUI_GenericUIData& data)
{
	j = json
	{
		{ "Type", TUI_GenericUIData::eType },
		{ "strTag", data.strTag },
		{ "iLayerIndex", data.iLayerIndex },
		{ "iRectTransformType", data.iRectTransformType },
		{ "iUIType", data.iUIType },
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },

		{ "strTextureTag", data.strTextureTag },
		{ "iTextureIndex", data.iTextureIndex },
	};
}
void from_json(const json& j, TUI_GenericUIData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("iLayerIndex").get_to(data.iLayerIndex);
	j.at("iRectTransformType").get_to(data.iRectTransformType);
	j.at("iUIType").get_to(data.iUIType);
	j.at("fWidth").get_to(data.fWidth);
	j.at("fHeight").get_to(data.fHeight);
	j.at("fPosX").get_to(data.fPosX);
	j.at("fPosY").get_to(data.fPosY);
	j.at("fPosZ").get_to(data.fPosZ);
	j.at("strTextureTag").get_to(data.strTextureTag);
	j.at("iTextureIndex").get_to(data.iTextureIndex);
}
void to_json(json& j, const TUI_LayerData& data)
{
	j = json
	{
		{ "Type", TUI_LayerData::eType },
		{ "strTag", data.strTag },
	};
}
void from_json(const json& j, TUI_LayerData& data)
{
	j.at("strTag").get_to(data.strTag);
}
void to_json(json& j, const TUI_CanvasData& data)
{
	j = json
	{
		{ "Type", TUI_CanvasData::eType },
		{ "strTag", data.strTag },
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },
	};
}

void from_json(const json& j, TUI_CanvasData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("fWidth").get_to(data.fWidth);
	j.at("fHeight").get_to(data.fHeight);
	j.at("fPosX").get_to(data.fPosX);
	j.at("fPosY").get_to(data.fPosY);
	j.at("fPosZ").get_to(data.fPosZ);
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

json CUI_Layer_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_Layer_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_LayerData>();
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

NS_END;