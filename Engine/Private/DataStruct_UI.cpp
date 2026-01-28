#include "Engine_pch.h"
#include "DataStruct_UI.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

NLOHMANN_JSON_SERIALIZE_ENUM(EUIType,
	{
		{EUIType::CANVAS, "CANVAS"},
		{EUIType::LAYER, "LAYER"},
		{EUIType::GENERICUI, "GENERICUI"}
	}
)	

inline void to_json(json& j, const TUI_GenericUIData& data)
{
	j = json
	{
		{ "Type", TUI_GenericUIData::eType },
		{ "strTag", data.strTag },
		{ "strUIName", data.strName },
		{ "strLayerTag", data.strLayerTag },

		{ "iRectTransformType", data.iRectTransformType },
		{ "iUIType", data.iUIType },
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },
	};
}
inline void from_json(const json& j, TUI_GenericUIData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("strName").get_to(data.strName);
	j.at("strLayerTag").get_to(data.strLayerTag);

	j.at("iRectTransformType").get_to(data.iRectTransformType);
	j.at("iUIType").get_to(data.iUIType);
	j.at("fWidth").get_to(data.fWidth);
	j.at("fHeight").get_to(data.fHeight);
	j.at("fPosX").get_to(data.fPosX);
	j.at("fPosY").get_to(data.fPosY);
	j.at("fPosZ").get_to(data.fPosZ);
}
inline void to_json(json& j, const TUI_LayerData& data)
{
	j = json
	{
		{ "Type", TUI_LayerData::eType },
		{ "strTag", data.strTag },
		{ "strLayerTag", data.strLayerTag },
	};
}
inline void from_json(const json& j, TUI_LayerData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("strLayerTag").get_to(data.strLayerTag);
}

inline void to_json(json& j, const TUI_CanvasData& data)
{
	j = json
	{
		{ "Type", TUI_CanvasData::eType },
		{ "strTag", data.strTag },
		{ "strName", data.strName},		
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },
	};
}

inline void from_json(const json& j, TUI_CanvasData& data)
{
	j.at("strTag").get_to(data.strTag);
	j.at("strName").get_to(data.strName);
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