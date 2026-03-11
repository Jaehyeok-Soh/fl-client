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
		{ "iParams0", data.iParams0 },
	};
}

void from_json(const json& j, TUI_DImageData& data)
{
	data.strTag				= "";
	data.strOwnerName		= "";
	data.eDISubClassType	= std::decay_t<decltype(data.eDISubClassType)>{};
	data.iParams0			= 0;

	data.strTag				= j.value("strTag", data.strTag);
	data.strOwnerName		= j.value("strOwnerName", data.strOwnerName);
	data.eDISubClassType	= j.value("eDISubClassType", j.value("eSubClassType", data.eDISubClassType));
	data.iParams0			= j.value("iParams0", data.iParams0);
}

void to_json(json& j, const TUI_TextData& data)
{
	j = json
	{
		{"Type",				TUI_TextData::eType },
		{"strTag",				data.strTag},
		{"strOwnerName",		data.strOwnerName},
		{"eTextSubClassType",	data.eTextSubClassType},
		{"eShaderType",			data.eShaderType},
		{"strFontTag",			data.strFontTag},
		{"strText",				data.strText},
		{"vFontColor", {
			{ "x",	data.vFontColor.x },
			{ "y",	data.vFontColor.y },
			{ "z",	data.vFontColor.z },
			{ "w",	data.vFontColor.w }}},
		{"ePivot",				data.ePivot},
		{"fRotate",				data.fRotate},
		{"fScale",				data.fScale},
		{"iParam0",				data.iParam0},
	};
}

void from_json(const json& j, TUI_TextData& data)
{
	data.strTag			= "";
	data.strOwnerName	= "";
	data.eTextSubClassType = EUITextSubClassType::NONE_OWNER;
	data.eShaderType	= EFontShaderType::NORMAL;
	data.strText		= "No Text";
	data.strFontTag		= "SemiBold";
	data.vFontColor.x	= 1.f;
	data.vFontColor.y	= 1.f;
	data.vFontColor.z	= 1.f;
	data.vFontColor.w	= 1.f;
	data.ePivot			= EFontPivotType::CENTER;
	data.fRotate		= 0.f;
	data.fScale			= 1.f;
	data.iParam0		= 0;

	data.strTag			= j.value("strTag", data.strTag);
	data.strOwnerName	= j.value("strOwnerName", data.strOwnerName);
	data.eTextSubClassType = j.value("eTextSubClassType", data.eTextSubClassType);
	data.eShaderType	= j.value("eShaderType", data.eShaderType);
	data.strText		= j.value("strText", data.strText);
	data.strFontTag		= j.value("strFontTag", data.strFontTag);
	const json jc		= j.value("vFontColor", json::object());
	data.vFontColor.x	= jc.value("x", data.vFontColor.x);
	data.vFontColor.y	= jc.value("y", data.vFontColor.y);
	data.vFontColor.z	= jc.value("z", data.vFontColor.z);
	data.vFontColor.w	= jc.value("w", data.vFontColor.w);
	data.ePivot			= j.value("ePivot", data.ePivot);
	data.fRotate		= j.value("fRotate", data.fRotate);
	data.fScale			= j.value("fScale", data.fScale);
	data.iParam0		= j.value("iParam0", data.iParam0);
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
		{ "fScale", data.fScale	},
		{ "fRotate", data.fRotate},
		{ "strTextureTag", data.strTextureTag },
		{ "isVisible", data.isVisible },
		{ "isInteract", data.isInteract },
		{ "isActivate", data.isActivate },
		{ "iComponentFlag", data.iComponentFlag },
		{ "eSubClassType", data.eSubClassType },
		{ "isUseColorTint", data.isUseColorTint },
		{ "vColorTint", {{ "x", data.vColorTint.x },{ "y", data.vColorTint.y },{ "z", data.vColorTint.z },{ "w", data.vColorTint.w }}},
		{ "vGradiantColorTint", {{ "x", data.vGradiantColorTint.x },{ "y", data.vGradiantColorTint.y },{ "z", data.vGradiantColorTint.z },{ "w", data.vGradiantColorTint.w }}},
		{ "iShaderPass", data.iShaderPass },
		{ "iFillDir", data.iFillDir },
		{ "fDelay", data.fDelay },
		{ "iFlip", data.iFlip },
		{ "fAlphaRatio", data.fAlphaRatio },
		{ "strNoiseTextureTag", data.strNoiseTextureTag },
		{ "strAlphaMaskTextureTag", data.strAlphaMaskTextureTag },
		{ "strGlowTextureTag", data.strGlowTextureTag },
	};
}

void from_json(const json& j, TUI_GenericUIData& data)
{
	data.eClassType				= EUIClassType::JUST_IMAGE;
	data.strTag					= "";
	data.strCanvasName			= "";
	data.iRectTransformType		= 0;
	data.fWidth					= 0.f;
	data.fHeight				= 0.f;
	data.fPosX					= 0.f;
	data.fPosY					= 0.f;
	data.fPosZ					= 0.f;
	data.fScale					= 1.f;
	data.fRotate				= 0.f;
	data.strTextureTag			= "";
	data.isVisible				= true;
	data.isInteract				= true;
	data.isActivate				= true;
	data.iComponentFlag			= 0;
	data.eSubClassType			= decltype(data.eSubClassType){};
	data.isUseColorTint			= false;
	data.vColorTint.x			= 1.f;
	data.vColorTint.y			= 1.f;
	data.vColorTint.z			= 1.f;
	data.vColorTint.w			= 1.f;
	data.vGradiantColorTint.x	= 1.f;
	data.vGradiantColorTint.y	= 1.f;
	data.vGradiantColorTint.z	= 1.f;
	data.vGradiantColorTint.w	= 1.f;
	data.iShaderPass			= 0;
	data.iFillDir				= 0;
	data.fDelay					= 0.f;
	data.iFlip					= 0;
	data.fAlphaRatio			= 1.f;
	data.strNoiseTextureTag		= "";
	data.strAlphaMaskTextureTag = "";
	data.strGlowTextureTag		= "";


	data.eClassType				= j.value("eClassType", data.eClassType);
	data.strTag					= j.value("strTag", data.strTag);
	data.strCanvasName			= j.value("strCanvasName", data.strCanvasName);
	data.iRectTransformType		= j.value("iRectTransformType", data.iRectTransformType);
	data.fWidth					= j.value("fWidth", data.fWidth);
	data.fHeight				= j.value("fHeight", data.fHeight);
	data.fPosX					= j.value("fPosX", data.fPosX);
	data.fPosY					= j.value("fPosY", data.fPosY);
	data.fPosZ					= j.value("fPosZ", data.fPosZ);
	data.fScale					= j.value("fScale", data.fScale);
	data.fRotate				= j.value("fRotate", data.fRotate);
	data.strTextureTag			= j.value("strTextureTag", data.strTextureTag);
	data.isVisible				= j.value("isVisible", data.isVisible);
	data.isInteract				= j.value("isInteract", data.isInteract);
	data.isActivate				= j.value("isActivate", data.isActivate);
	data.iComponentFlag			= j.value("iComponentFlag", data.iComponentFlag);
	data.eSubClassType			= j.value("eSubClassType", data.eSubClassType);
	data.isUseColorTint			= j.value("isUseColorTint", data.isUseColorTint);
	const json jt				= j.value("vColorTint", json::object());
	data.vColorTint.x			= jt.value("x", data.vColorTint.x);
	data.vColorTint.y			= jt.value("y", data.vColorTint.y);
	data.vColorTint.z			= jt.value("z", data.vColorTint.z);
	data.vColorTint.w			= jt.value("w", data.vColorTint.w);

	const json jgt				= j.value("vGradiantColorTint", json::object());
	data.vGradiantColorTint.x	= jgt.value("x", data.vGradiantColorTint.x);
	data.vGradiantColorTint.y	= jgt.value("y", data.vGradiantColorTint.y);
	data.vGradiantColorTint.z	= jgt.value("z", data.vGradiantColorTint.z);
	data.vGradiantColorTint.w	= jgt.value("w", data.vGradiantColorTint.w);
	data.iShaderPass			= j.value("iShaderPass", data.iShaderPass);
	data.iFillDir				= j.value("iFillDir", data.iFillDir);
	data.fDelay					= j.value("fDelay", data.fDelay);
	data.iFlip					= j.value("iFlip", data.iFlip);
	data.fAlphaRatio			= j.value("fAlphaRatio", data.fAlphaRatio);
	data.strNoiseTextureTag		= j.value("strNoiseTextureTag", data.strNoiseTextureTag);
	data.strAlphaMaskTextureTag = j.value("strAlphaMaskTextureTag", data.strAlphaMaskTextureTag);
	data.strGlowTextureTag		= j.value("strGlowTextureTag", data.strGlowTextureTag);
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
		{ "iPrefabType", data.iPrefabType },
		{ "iNumPrefabs", data.iNumPrefabs },
	};
}

void from_json(const json& j, TUI_CanvasData& data)
{
	data.iPrefabType = 0;
	data.iNumPrefabs = 0;

	data.strTag			= j.value("strTag", data.strTag);
	data.iLevelIndex	= j.value("iLevelIndex", data.iLevelIndex);
	data.fWidth			= j.value("fWidth", data.fWidth);
	data.fHeight		= j.value("fHeight", data.fHeight);
	data.fPosX			= j.value("fPosX", data.fPosX);
	data.fPosY			= j.value("fPosY", data.fPosY);
	data.fPosZ			= j.value("fPosZ", data.fPosZ);
	data.iEditorSizeX	= j.value("iEditorSizeX", data.iEditorSizeX);
	data.iEditorSizeY	= j.value("iEditorSizeY", data.iEditorSizeY);
	data.iPrefabType	= j.value("iPrefabType", data.iPrefabType);
	data.iNumPrefabs	= j.value("iNumPrefabs", data.iNumPrefabs);
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
