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
	};
}
inline void from_json(const json& j, TUI_GenericUIData& data)
{
	j.at("strTag").get_to(data.strTag);
}
inline void to_json(json& j, const TUI_LayerData& data)
{
	j = json
	{
		{ "Type", TUI_LayerData::eType },
		{ "strTag", data.strTag },
	};
}
inline void from_json(const json& j, TUI_LayerData& data)
{
	j.at("strTag").get_to(data.strTag);
}

inline void to_json(json& j, const TUI_CanvasData& data)
{
	j = json
	{
		{ "Type", TUI_CanvasData::eType },
		{ "strTag", data.strTag },
	};
}

inline void from_json(const json& j, TUI_CanvasData& data)
{
	j.at("strTag").get_to(data.strTag);
}

NS_END


NS_BEGIN(Engine)

json CUI_GenericUI::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_GenericUI::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_GenericUIData>();
	return S_OK;
}

json CUI_Layer::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_Layer::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_LayerData>();
	return S_OK;
}

json CUI_Canvas::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_Canvas::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_CanvasData>();
	return S_OK;
}

NS_END;