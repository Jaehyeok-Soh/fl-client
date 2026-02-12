#include "Engine_pch.h"
#include "DataStruct_UI.h"
#pragma push_macro("new")
#undef new
#include "json.hpp"
using json = nlohmann::json;
#pragma pop_macro("new")

NS_BEGIN(DTO)

void to_json(json& j, const TUI_DImageData& data)
{
	j = json
	{
		{ "Type", TUI_DImageData::eType },
		{ "strTag", data.strTag },
		{ "strOwnerName", data.strOwnerName },
		{ "eDISubClassType", data.eDISubClassType },
	};
}

void from_json(const json& j, TUI_DImageData& data)
{
	data.strTag          = j.value("strTag", data.strTag);
	data.strOwnerName    = j.value("strOwnerName", data.strOwnerName);
	data.eDISubClassType = j.value("eDISubClassType", j.value("eSubClassType", data.eDISubClassType));
}

void to_json(json& j, const TUI_ButtonTriggerData& data)
{
	j = json
	{
		{"Type", TUI_ButtonTriggerData::eType},
		{"strTag", data.strTag},
		{"strOwnerName", data.strOwnerName},
		{"strKeyMapping", data.strKeyMapping},
		{"vecTriggerCanvas", data.vecTriggerCanvas},
		{"vecTriggerUI", data.vecTriggerUI},
	};
}

void from_json(const json& j, TUI_ButtonTriggerData& data)
{
	data.strTag           = j.value("strTag", data.strTag);
	data.strOwnerName     = j.value("strOwnerName", data.strOwnerName);
	data.strKeyMapping    = j.value("strKeyMapping", data.strKeyMapping);
	data.vecTriggerCanvas = j.value("vecTriggerCanvas", data.vecTriggerCanvas);
	data.vecTriggerUI     = j.value("vecTriggerUI", data.vecTriggerUI);
}

void to_json(json& j, const TUI_TriggerData& data)
{
	j = json
	{
		{"Type", TUI_TriggerData::eType},
		{"strTag", data.strTag},
		{"strOwnerName", data.strOwnerName},

		{"vecHoverEnterTriggerCanvas",  data.vecHoverEnterTriggerCanvas},
		{"vecHoverEnterTriggerUI",      data.vecHoverEnterTriggerUI},
		{"vecHoverExitTriggerCanvas",	data.vecHoverExitTriggerCanvas},
		{"vecHoverExitTriggerUI",		data.vecHoverExitTriggerUI},

		{"vecPressEnterTriggerCanvas",	data.vecPressEnterTriggerCanvas},
		{"vecPressEnterTriggerUI",		data.vecPressEnterTriggerUI},
		{"vecPressExitTriggerCanvas",	data.vecPressExitTriggerCanvas},
		{"vecPressExitTriggerUI",		data.vecPressExitTriggerUI},
	};
}

void from_json(const json& j, TUI_TriggerData& data)
{
	data.strTag = j.value("strTag", data.strTag);
	data.strOwnerName = j.value("strOwnerName", data.strOwnerName);

	data.vecHoverEnterTriggerCanvas = j.value("vecHoverEnterTriggerCanvas", data.vecHoverEnterTriggerCanvas);
	data.vecHoverEnterTriggerUI = j.value("vecHoverEnterTriggerUI", data.vecHoverEnterTriggerUI);
	data.vecHoverExitTriggerCanvas = j.value("vecHoverExitTriggerCanvas", data.vecHoverExitTriggerCanvas);
	data.vecHoverExitTriggerUI = j.value("vecHoverExitTriggerUI", data.vecHoverExitTriggerUI);

	data.vecPressEnterTriggerCanvas = j.value("vecPressEnterTriggerCanvas", data.vecPressEnterTriggerCanvas);
	data.vecPressEnterTriggerUI = j.value("vecPressEnterTriggerUI", data.vecPressEnterTriggerUI);
	data.vecPressExitTriggerCanvas = j.value("vecPressExitTriggerCanvas", data.vecPressExitTriggerCanvas);
	data.vecPressExitTriggerUI = j.value("vecPressExitTriggerUI", data.vecPressExitTriggerUI);
}

void to_json(json& j, const TUI_TextData& data)
{
	j = json
	{
		{"Type", TUI_TextData::eType },
		{"strTag", data.strTag},
		{"strOwnerName", data.strOwnerName},
		{"strText", data.strText},
		{"vFontColor", {{ "x", data.vFontColor.x },{ "y", data.vFontColor.y },{ "z", data.vFontColor.z },{ "w", data.vFontColor.w }}},
		{"fRotate", data.fRotate},
		{"vPivot", {{ "x", data.vPivot.x },{ "y", data.vPivot.y }}},
		{"fScale", data.fScale},
	};
}

void from_json(const json& j, TUI_TextData& data)
{
	data.strTag = j.value("strTag", data.strTag);
	data.strOwnerName = j.value("strOwnerName", data.strOwnerName);
	data.strText = j.value("strText", data.strText);

	const json jc = j.value("vFontColor", json::object());
	data.vFontColor.x = jc.value("x", data.vFontColor.x);
	data.vFontColor.y = jc.value("y", data.vFontColor.y);
	data.vFontColor.z = jc.value("z", data.vFontColor.z);
	data.vFontColor.w = jc.value("w", data.vFontColor.w);

	data.fRotate = j.value("fRotate", data.fRotate);

	const json jp = j.value("vPivot", json::object());
	data.vPivot.x = jp.value("x", data.vPivot.x);
	data.vPivot.y = jp.value("y", data.vPivot.y);

	data.fScale = j.value("fScale", data.fScale);
}

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
		{ "eSubClassType", data.eSubClassType },
		{ "isUseColorTint", data.isUseColorTint },
		{ "vColorTint", {{ "x", data.vColorTint.x },{ "y", data.vColorTint.y },{ "z", data.vColorTint.z },{ "w", data.vColorTint.w }}},
		{ "iShaderPass", data.iShaderPass },
		{ "iFillDir", data.iFillDir },
		{ "fDelay", data.fDelay },
		{ "iFlip", data.iFlip },
		{ "fAlphaRatio", data.fAlphaRatio },
	};
}

void from_json(const json& j, TUI_GenericUIData& data)
{
	data.eClassType = j.value("eClassType", data.eClassType);
	data.strTag = j.value("strTag", data.strTag);
	data.strCanvasName = j.value("strCanvasName", data.strCanvasName);
	data.iRectTransformType = j.value("iRectTransformType", data.iRectTransformType);
	data.fWidth = j.value("fWidth", data.fWidth);
	data.fHeight = j.value("fHeight", data.fHeight);
	data.fPosX = j.value("fPosX", data.fPosX);
	data.fPosY = j.value("fPosY", data.fPosY);
	data.fPosZ = j.value("fPosZ", data.fPosZ);
	data.strTextureTag = j.value("strTextureTag", data.strTextureTag);
	data.isVisible = j.value("isVisible", data.isVisible);
	data.iComponentFlag = j.value("iComponentFlag", data.iComponentFlag);
	data.eSubClassType = j.value("eSubClassType", data.eSubClassType);
	data.isUseColorTint = j.value("isUseColorTint", data.isUseColorTint);

	const json jt = j.value("vColorTint", json::object());
	data.vColorTint.x = jt.value("x", data.vColorTint.x);
	data.vColorTint.y = jt.value("y", data.vColorTint.y);
	data.vColorTint.z = jt.value("z", data.vColorTint.z);
	data.vColorTint.w = jt.value("w", data.vColorTint.w);

	data.iShaderPass = j.value("iShaderPass", data.iShaderPass);
	data.iFillDir = j.value("iFillDir", data.iFillDir);
	data.fDelay = j.value("fDelay", data.fDelay);
	data.iFlip = j.value("iFlip", data.iFlip);
	data.fAlphaRatio = j.value("fAlphaRatio", data.fAlphaRatio);
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
	data.strTag = j.value("strTag", data.strTag);
	data.iLevelIndex = j.value("iLevelIndex", data.iLevelIndex);
	data.fWidth = j.value("fWidth", data.fWidth);
	data.fHeight = j.value("fHeight", data.fHeight);
	data.fPosX = j.value("fPosX", data.fPosX);
	data.fPosY = j.value("fPosY", data.fPosY);
	data.fPosZ = j.value("fPosZ", data.fPosZ);
	data.iEditorSizeX = j.value("iEditorSizeX", data.iEditorSizeX);
	data.iEditorSizeY = j.value("iEditorSizeY", data.iEditorSizeY);
}

NS_END


NS_BEGIN(Engine)

json CUI_DImage_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_DImage_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_DImageData>();
	return S_OK;
}

json CUI_ButtonTrigger_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_ButtonTrigger_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_ButtonTriggerData>();
	return S_OK;
}

json CUI_Trigger_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_Trigger_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_TriggerData>();
	return S_OK;
}

json CUI_Text_DTO::ToJson() const
{
	return json(m_Data);
}

HRESULT CUI_Text_DTO::FromJson(const json& j)
{
	m_Data = j.get<DTO::TUI_TextData>();
	return S_OK;
}

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
