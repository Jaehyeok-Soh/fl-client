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
		{ "eClassType",data.eClassType},
		{ "strTag", data.strTag },
		{ "strCanvasName", data.strCanvasName },
		{ "iRectTransformType", data.iRectTransformType },
		{ "fWidth", data.fWidth },
		{ "fHeight", data.fHeight },
		{ "fPosX", data.fPosX },
		{ "fPosY", data.fPosY },
		{ "fPosZ", data.fPosZ },
		{ "strTextureTag", data.strTextureTag },
		{ "isVisible", data.isVisible },
		{ "iComponentFlag", data.iComponentFlag },
		{ "eOwnerType", data.eOwnerType },
		{ "isUseColorTint", data.isUseColorTint },
		{ "vColorTint", {{ "x", data.vColorTint.x },{ "y", data.vColorTint.y },{ "z", data.vColorTint.z },{ "w", data.vColorTint.w }}},
		{ "iShaderPass", data.iShaderPass },
		{ "iFillDir", data.iFillDir },
		{ "fDelay", data.fDelay },
	};
}
void from_json(const json& j, TUI_GenericUIData& data)
{
	j.at("eClassType").get_to(data.eClassType);
	j.at("strTag").get_to(data.strTag);
	j.at("strCanvasName").get_to(data.strCanvasName);
	j.at("iRectTransformType").get_to(data.iRectTransformType);
	j.at("fWidth").get_to(data.fWidth);
	j.at("fHeight").get_to(data.fHeight);
	j.at("fPosX").get_to(data.fPosX);
	j.at("fPosY").get_to(data.fPosY);
	j.at("fPosZ").get_to(data.fPosZ);
	j.at("strTextureTag").get_to(data.strTextureTag);
	j.at("isVisible").get_to(data.isVisible);
	j.at("iComponentFlag").get_to(data.iComponentFlag);
	j.at("eOwnerType").get_to(data.eOwnerType);
	j.at("isUseColorTint").get_to(data.isUseColorTint);
	const auto& jt = j.at("vColorTint");
	jt.at("x").get_to(data.vColorTint.x);
	jt.at("y").get_to(data.vColorTint.y);
	jt.at("z").get_to(data.vColorTint.z);
	jt.at("w").get_to(data.vColorTint.w);
	j.at("iShaderPass").get_to(data.iShaderPass);
	j.at("iFillDir").get_to(data.iFillDir);
	j.at("fDelay").get_to(data.fDelay);
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


NS_END;