#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

/////////////////-------------------  UI  -------------------/////////////////
enum class EUIType : _uint
{
	CANVAS,
	GENERICUI,
	UI_TEXT,
	TRIGGER,
	BUTTON_TRIGGER,
	DYNAMIC_IMAGE,
	WORLD_UI,
	END
};
inline constexpr _uint g_UITypeCount{ ENUM_TO_UINT(EUIType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EUIType,
	{
		{EUIType::CANVAS, "CANVAS"},
		{EUIType::GENERICUI, "GENERICUI"},
		{EUIType::UI_TEXT, "UI_TEXT"},
		{EUIType::TRIGGER, "TRIGGER"},
		{EUIType::BUTTON_TRIGGER, "BUTTON_TRIGGER"},
		{EUIType::DYNAMIC_IMAGE, "DYNAMIC_IMAGE"},
		{EUIType::WORLD_UI, "WORLD_UI"},
	}
)

enum class EUIClassType
{
	PROGRESS_BAR,
	JUST_IMAGE,
	UI_TEXT,
	TRIGGER,
	BUTTON_TRIGGER,
	DYNAMIC_IMAGE,
	WORLD_UI,
	END
};

inline std::string UIClassTypeToString(EUIClassType eType)
{
	switch (eType)
	{
	case EUIClassType::PROGRESS_BAR: return "PROGRESS_BAR";
	case EUIClassType::JUST_IMAGE: return "JUST_IMAGE";
	case EUIClassType::UI_TEXT: return "UI_TEXT";
	case EUIClassType::TRIGGER: return "TRIGGER";
	case EUIClassType::BUTTON_TRIGGER: return "BUTTON_TRIGGER";
	case EUIClassType::DYNAMIC_IMAGE: return "DYNAMIC_IMAGE";
	case EUIClassType::WORLD_UI: return "WORLD_UI";
	case EUIClassType::END: return "END";
	default: return "";
	}
}

inline EUIClassType StringToUIClassType(const std::string& str)
{
	if (str == "PROGRESS_BAR") return EUIClassType::PROGRESS_BAR;
	else if (str == "JUST_IMAGE") return EUIClassType::JUST_IMAGE;
	else if (str == "UI_TEXT") return EUIClassType::UI_TEXT;
	else if (str == "TRIGGER") return EUIClassType::TRIGGER;
	else if (str == "BUTTON_TRIGGER") return EUIClassType::BUTTON_TRIGGER;
	else if (str == "DYNAMIC_IMAGE") return EUIClassType::DYNAMIC_IMAGE;
	else if (str == "WORLD_UI") return EUIClassType::WORLD_UI;
	else return EUIClassType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUIClassType,
	{
		{EUIClassType::PROGRESS_BAR, "PROGRESS_BAR"},
		{EUIClassType::JUST_IMAGE, "JUST_IMAGE"},
		{EUIClassType::UI_TEXT, "UI_TEXT"},
		{EUIClassType::TRIGGER, "TRIGGER"},
		{EUIClassType::BUTTON_TRIGGER, "BUTTON_TRIGGER"},
		{EUIClassType::DYNAMIC_IMAGE, "DYNAMIC_IMAGE"},
		{EUIClassType::WORLD_UI, "WORLD_UI"},
	})

enum EComponentTypeFlag
{
	BUTTON_COMPONENT = 1 << 0,
	PROGRESS_COMPONENT = 1 << 1
};

enum class EUISubClassType
{
	NONE_OWNER,
	PLAYER_HP,
	PLAYER_ARMOR,
	PLAYER_ENERGY,
	PLAYER_LV,
	END
};

inline std::string UISubClasstypeToString(EUISubClassType eType)
{
	switch (eType)
	{
	case EUISubClassType::NONE_OWNER:		return "NONE_OWNER";
	case EUISubClassType::PLAYER_HP:		return "PLAYER_HP";
	case EUISubClassType::PLAYER_ARMOR:		return "PLAYER_ARMOR";
	case EUISubClassType::PLAYER_ENERGY:	return "PLAYER_ENERGY";
	case EUISubClassType::PLAYER_LV:		return "PLAYER_LV";
	case EUISubClassType::END:				return "END";
	default: return "";
	}
}

inline EUISubClassType StringToUISubClassType(const std::string& str)
{
	if (str == "NONE_OWNER")			return EUISubClassType::NONE_OWNER;
	else if (str == "PLAYER_HP")		return EUISubClassType::PLAYER_HP;
	else if (str == "PLAYER_ARMOR")		return EUISubClassType::PLAYER_ARMOR;
	else if (str == "PLAYER_ENERGY")	return EUISubClassType::PLAYER_ENERGY;
	else if (str == "PLAYER_LV")		return EUISubClassType::PLAYER_LV;
	else return EUISubClassType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUISubClassType,
	{
		{EUISubClassType::NONE_OWNER,		"NONE_OWNER"},
		{EUISubClassType::PLAYER_HP,		"PLAYER_HP"},
		{EUISubClassType::PLAYER_ARMOR,		"PLAYER_ARMOR"},
		{EUISubClassType::PLAYER_ENERGY,	"PLAYER_ENERGY"},
		{EUISubClassType::PLAYER_LV,		"PLAYER_LV"},
	})

	enum class EUIDImageSubClassType
{
	NONE_OWNER,

	// 플레이어 스킬 // PLAYER_E < New Enum < PLAYER_SKILL_END
	PLAYER_SKILL_BEGIN,
	PLAYER_E,
	PLAYER_Q,
	PLAYER_Z,
	PLAYER_GUN,
	PLAYER_DODGE,
	PLAYER_SKILL_END,

	// 호버됐을 때 팝업창 // HOVER_POPUP_BG < New Enum < HOVER_POPUP_END
	HOVER_POPUP_BEGIN,
	HOVER_POPUP_BG,
	HOVER_POPUP_ICON,
	HOVER_POPUP_TEXT,
	HOVER_POPUP_END,

	// 미니맵
	MINIMAP_BEGIN,
	MINIMAP_PLAYER_ICON,
	MINIMAP_CAMERA_SIGHT,
	MINIMAP_BGFRAME,
	MINIMAP_WARNING_FRAME,
	MINIMAP_END,

	// 메뉴창
	MENU_BEGIN,
	MENU_BG,
	MENU_ICON,
	MENU_ICON_BG,
	MENU_END,

	END
};

NLOHMANN_JSON_SERIALIZE_ENUM(EUIDImageSubClassType,
	{
		{ EUIDImageSubClassType::NONE_OWNER,			"NONE_OWNER" },

		{ EUIDImageSubClassType::PLAYER_SKILL_BEGIN,	"PLAYER_SKILL_BEGIN" },
		{ EUIDImageSubClassType::PLAYER_E,				"PLAYER_E" },
		{ EUIDImageSubClassType::PLAYER_Q,				"PLAYER_Q" },
		{ EUIDImageSubClassType::PLAYER_Z,				"PLAYER_Z" },
		{ EUIDImageSubClassType::PLAYER_GUN,			"PLAYER_GUN" },
		{ EUIDImageSubClassType::PLAYER_DODGE,			"PLAYER_DODGE" },
		{ EUIDImageSubClassType::PLAYER_SKILL_END,		"PLAYER_SKILL_END" },

		{ EUIDImageSubClassType::HOVER_POPUP_BEGIN,		"HOVER_POPUP_BEGIN" },
		{ EUIDImageSubClassType::HOVER_POPUP_BG,		"HOVER_POPUP_BG" },
		{ EUIDImageSubClassType::HOVER_POPUP_ICON,		"HOVER_POPUP_ICON" },
		{ EUIDImageSubClassType::HOVER_POPUP_TEXT,		"HOVER_POPUP_TEXT" },
		{ EUIDImageSubClassType::HOVER_POPUP_END,		"HOVER_POPUP_END" },

		{ EUIDImageSubClassType::MINIMAP_BEGIN,			"MINIMAP_BEGIN" },
		{ EUIDImageSubClassType::MINIMAP_PLAYER_ICON,	"MINIMAP_PLAYER_ICON" },
		{ EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT,	"MINIMAP_CAMERA_SIGHT" },
		{ EUIDImageSubClassType::MINIMAP_BGFRAME,		"MINIMAP_BGFRAME" },
		{ EUIDImageSubClassType::MINIMAP_WARNING_FRAME,	"MINIMAP_WARNING_FRAME" },
		{ EUIDImageSubClassType::MINIMAP_END,			"MINIMAP_END" },

		{ EUIDImageSubClassType::MENU_BEGIN,			"MENU_BEGIN" },
		{ EUIDImageSubClassType::MENU_BG,				"MENU_BG" },
		{ EUIDImageSubClassType::MENU_ICON,				"MENU_ICON" },
		{ EUIDImageSubClassType::MENU_ICON_BG,			"MENU_ICON_BG" },
		{ EUIDImageSubClassType::MENU_END,				"MENU_END" },

		{ EUIDImageSubClassType::END,					"END" }
	})


	inline EUIDImageSubClassType StringToUIDImageSubType(const std::string& str)
{
	if (str == "NONE_OWNER")			return EUIDImageSubClassType::NONE_OWNER;

	if (str == "PLAYER_SKILL_BEGIN")	return EUIDImageSubClassType::PLAYER_SKILL_BEGIN;
	if (str == "PLAYER_E")				return EUIDImageSubClassType::PLAYER_E;
	if (str == "PLAYER_Q")				return EUIDImageSubClassType::PLAYER_Q;
	if (str == "PLAYER_Z")				return EUIDImageSubClassType::PLAYER_Z;
	if (str == "PLAYER_GUN")			return EUIDImageSubClassType::PLAYER_GUN;
	if (str == "PLAYER_DODGE")			return EUIDImageSubClassType::PLAYER_DODGE;
	if (str == "PLAYER_SKILL_END")		return EUIDImageSubClassType::PLAYER_SKILL_END;

	if (str == "HOVER_POPUP_BEGIN")		return EUIDImageSubClassType::HOVER_POPUP_BEGIN;
	if (str == "HOVER_POPUP_BG")		return EUIDImageSubClassType::HOVER_POPUP_BG;
	if (str == "HOVER_POPUP_ICON")		return EUIDImageSubClassType::HOVER_POPUP_ICON;
	if (str == "HOVER_POPUP_TEXT")		return EUIDImageSubClassType::HOVER_POPUP_TEXT;
	if (str == "HOVER_POPUP_END")		return EUIDImageSubClassType::HOVER_POPUP_END;

	if (str == "MINIMAP_BEGIN")			return EUIDImageSubClassType::MINIMAP_BEGIN;
	if (str == "MINIMAP_PLAYER_ICON")	return EUIDImageSubClassType::MINIMAP_PLAYER_ICON;
	if (str == "MINIMAP_CAMERA_SIGHT")	return EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT;
	if (str == "MINIMAP_BGFRAME")		return EUIDImageSubClassType::MINIMAP_BGFRAME;
	if (str == "MINIMAP_WARNING_FRAME")	return EUIDImageSubClassType::MINIMAP_WARNING_FRAME;
	if (str == "MINIMAP_END")			return EUIDImageSubClassType::MINIMAP_END;

	if (str == "MENU_BEGIN")			return EUIDImageSubClassType::MENU_BEGIN;
	if (str == "MENU_BG")				return EUIDImageSubClassType::MENU_BG;
	if (str == "MENU_ICON")				return EUIDImageSubClassType::MENU_ICON;
	if (str == "MENU_ICON_BG")			return EUIDImageSubClassType::MENU_ICON_BG;
	if (str == "MENU_END")				return EUIDImageSubClassType::MENU_END;

	if (str == "END")					return EUIDImageSubClassType::END;
	return EUIDImageSubClassType::NONE_OWNER;
}

inline const char* UIDImageSubTypeToString(EUIDImageSubClassType type)
{
	switch (type)
	{
	case EUIDImageSubClassType::NONE_OWNER:			return "NONE_OWNER";

	case EUIDImageSubClassType::PLAYER_SKILL_BEGIN:	return "PLAYER_SKILL_BEGIN";
	case EUIDImageSubClassType::PLAYER_E:			return "PLAYER_E";
	case EUIDImageSubClassType::PLAYER_Q:			return "PLAYER_Q";
	case EUIDImageSubClassType::PLAYER_Z:			return "PLAYER_Z";
	case EUIDImageSubClassType::PLAYER_GUN:			return "PLAYER_GUN";
	case EUIDImageSubClassType::PLAYER_DODGE:		return "PLAYER_DODGE";
	case EUIDImageSubClassType::PLAYER_SKILL_END:	return "PLAYER_SKILL_END";

	case EUIDImageSubClassType::HOVER_POPUP_BEGIN:	return "HOVER_POPUP_BEGIN";
	case EUIDImageSubClassType::HOVER_POPUP_BG:		return "HOVER_POPUP_BG";
	case EUIDImageSubClassType::HOVER_POPUP_ICON:	return "HOVER_POPUP_ICON";
	case EUIDImageSubClassType::HOVER_POPUP_TEXT:	return "HOVER_POPUP_TEXT";
	case EUIDImageSubClassType::HOVER_POPUP_END:	return "HOVER_POPUP_END";

	case EUIDImageSubClassType::MINIMAP_BEGIN:			return "MINIMAP_BEGIN";
	case EUIDImageSubClassType::MINIMAP_PLAYER_ICON:	return "MINIMAP_PLAYER_ICON";
	case EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT:	return "MINIMAP_CAMERA_SIGHT";
	case EUIDImageSubClassType::MINIMAP_BGFRAME:		return "MINIMAP_BGFRAME";
	case EUIDImageSubClassType::MINIMAP_WARNING_FRAME:	return "MINIMAP_WARNING_FRAME";
	case EUIDImageSubClassType::MINIMAP_END:			return "MINIMAP_END";

	case EUIDImageSubClassType::MENU_BEGIN:		return "MENU_BEGIN";
	case EUIDImageSubClassType::MENU_BG:		return "MENU_BG";
	case EUIDImageSubClassType::MENU_ICON:		return "MENU_ICON";
	case EUIDImageSubClassType::MENU_ICON_BG:	return "MENU_ICON_BG";
	case EUIDImageSubClassType::MENU_END:		return "MENU_END";

	case EUIDImageSubClassType::END:			return "END";
	default:									return "NONE_OWNER";
	}
}


enum class EUIWorldUISubClassType
{
	WORLD_UI_NONE,
	MONSTER_HP,
	WORLD_DAMAGE_FONT,
	END
};

NLOHMANN_JSON_SERIALIZE_ENUM(EUIWorldUISubClassType,
	{
		{ EUIWorldUISubClassType::WORLD_UI_NONE,	"WORLD_UI_NONE" },
		{ EUIWorldUISubClassType::MONSTER_HP,		"MONSTER_HP" },
		{ EUIWorldUISubClassType::WORLD_DAMAGE_FONT,"WORLD_DAMAGE_FONT" },
		{ EUIWorldUISubClassType::END,				"END" }
	})

	inline EUIWorldUISubClassType StringToUIWorldUISubType(const std::string& str)
{
	if (str == "WORLD_UI_NONE")       return EUIWorldUISubClassType::WORLD_UI_NONE;
	if (str == "MONSTER_HP")          return EUIWorldUISubClassType::MONSTER_HP;
	if (str == "WORLD_DAMAGE_FONT")   return EUIWorldUISubClassType::WORLD_DAMAGE_FONT;
	if (str == "END")                 return EUIWorldUISubClassType::END;
	return EUIWorldUISubClassType::WORLD_UI_NONE;
}

inline const char* UIWorldUISubTypeToString(EUIWorldUISubClassType type)
{
	switch (type)
	{
	case EUIWorldUISubClassType::WORLD_UI_NONE:      return "WORLD_UI_NONE";
	case EUIWorldUISubClassType::MONSTER_HP:         return "MONSTER_HP";
	case EUIWorldUISubClassType::WORLD_DAMAGE_FONT:  return "WORLD_DAMAGE_FONT";
	case EUIWorldUISubClassType::END:                return "END";
	default:                                         return "WORLD_UI_NONE";
	}
}

/////////////////-------------------  Data Struct  -------------------/////////////////
// 텍스트 데이터
struct TUI_TextData
{
	static constexpr EUIType eType = EUIType::UI_TEXT;
	std::string		strTag;
	std::string		strOwnerName;
	std::string		strFontTag;
	std::string	    strText;
	Vec4			vFontColor;	
	_float			fRotate;
	_float			fScale;
};

/////////////////
// 트리거 데이터
struct TUI_TriggerData
{
	static constexpr EUIType eType = EUIType::TRIGGER;
	std::string		strTag;
	std::string		strOwnerName;
	
	vector<std::string> vecHoverEnterTriggerCanvas;
	vector<std::string> vecHoverEnterTriggerUI;
	vector<std::string> vecHoverExitTriggerCanvas;
	vector<std::string> vecHoverExitTriggerUI;

	vector<std::string> vecPressEnterTriggerCanvas;
	vector<std::string> vecPressEnterTriggerUI;
	vector<std::string> vecPressExitTriggerCanvas;
	vector<std::string> vecPressExitTriggerUI;
};

/////////////////
// 버튼 트리거 데이터
struct TUI_ButtonTriggerData
{
	static constexpr EUIType eType = EUIType::BUTTON_TRIGGER;
	std::string			strTag;
	std::string			strOwnerName;
	_string				strKeyMapping;
	vector<std::string> vecTriggerCanvas;
	vector<std::string> vecTriggerUI;
};

/////////////////
// 다이나믹 이미지 데이터
struct TUI_DImageData
{
	static constexpr EUIType eType = EUIType::DYNAMIC_IMAGE;
	EUIClassType			eClassType;
	std::string				strTag;
	std::string				strOwnerName;
	EUIDImageSubClassType	eDISubClassType;
};

/////////////////
// 월드 UI 데이터
struct TUI_WorldUIData
{
	static constexpr EUIType eType = EUIType::WORLD_UI;
	EUIClassType			eClassType;
	std::string				strTag;
	std::string				strOwnerName;
	EUIWorldUISubClassType	eWorldUISubClass;
};

/////////////////
// UI
struct TUI_GenericUIData
{
	static constexpr EUIType eType = EUIType::GENERICUI;
	EUIClassType	eClassType;
	std::string		strTag;
	std::string		strCanvasName;
	uint32_t		iRectTransformType;
	_float			fWidth;
	_float			fHeight;
	_float			fPosX;
	_float			fPosY;
	_float			fPosZ;
	_string			strTextureTag;
	_bool			isVisible;
	uint32_t		iComponentFlag;
	EUISubClassType	eSubClassType;
	_bool			isUseColorTint;
	Vec4			vColorTint;
	Vec4			vGradiantColorTint;
	int32_t			iShaderPass;
	int32_t			iFillDir;
	_float			fDelay;
	int32_t			iFlip;
	_float			fAlphaRatio;
	_string			strNoiseTextureTag;
	_string			strAlphaMaskTextureTag;
};

struct TUI_CanvasData
{
	static constexpr EUIType eType = EUIType::CANVAS;
	std::string strTag;
	uint32_t	iLevelIndex;
	_float		fWidth;
	_float		fHeight;
	_float		fPosX;
	_float		fPosY;
	_float		fPosZ;
	uint32_t	iEditorSizeX ;
	uint32_t	iEditorSizeY;
};

/////////////////-------------------  to_json, from_json  -------------------/////////////////
void to_json(json& j, const TUI_DImageData& data);
void from_json(const json& j, TUI_DImageData& data);

void to_json(json& j, const TUI_ButtonTriggerData& data);
void from_json(const json& j, TUI_ButtonTriggerData& data);

void to_json(json& j, const TUI_TriggerData& data);
void from_json(const json& j, TUI_TriggerData& data);

void to_json(json& j, const TUI_TextData& data);
void from_json(const json& j, TUI_TextData& data);

void to_json(json& j, const TUI_GenericUIData& data);
void from_json(const json& j, TUI_GenericUIData& data);

void to_json(json& j, const TUI_CanvasData& data);
void from_json(const json& j, TUI_CanvasData& data);
NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)
// 다이나믹 이미지 클래스 
class ENGINE_DLL CUI_DImage_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_DImage_DTO() = default;
	virtual ~CUI_DImage_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::DYNAMIC_IMAGE); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_DImageData& Get_Data() const { return m_Data; }
	DTO::TUI_DImageData& Get_Data() { return m_Data; }
private:
	DTO::TUI_DImageData m_Data;
public:
	static CUI_DImage_DTO* Create() { return new CUI_DImage_DTO(); }
	virtual void Free() override { Super::Free(); }
};

// 버튼 트리거 클래스 
class ENGINE_DLL CUI_ButtonTrigger_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_ButtonTrigger_DTO() = default;
	virtual ~CUI_ButtonTrigger_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::BUTTON_TRIGGER); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_ButtonTriggerData& Get_Data() const { return m_Data; }
	DTO::TUI_ButtonTriggerData& Get_Data() { return m_Data; }
private:
	DTO::TUI_ButtonTriggerData m_Data;
public:
	static CUI_ButtonTrigger_DTO* Create() { return new CUI_ButtonTrigger_DTO(); }
	virtual void Free() override { Super::Free(); }
};

// 트리거 클래스
class ENGINE_DLL CUI_Trigger_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Trigger_DTO() = default;
	virtual ~CUI_Trigger_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::TRIGGER); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_TriggerData& Get_Data() const { return m_Data; }
	DTO::TUI_TriggerData& Get_Data() { return m_Data; }
private:
	DTO::TUI_TriggerData m_Data;
public:
	static CUI_Trigger_DTO* Create() { return new CUI_Trigger_DTO(); }
	virtual void Free() override { Super::Free(); }
};

// 텍스트 클래스
class ENGINE_DLL CUI_Text_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Text_DTO() = default;
	virtual ~CUI_Text_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::UI_TEXT); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_TextData& Get_Data() const { return m_Data; }
	DTO::TUI_TextData& Get_Data() { return m_Data; }
private:
	DTO::TUI_TextData m_Data;
public:
	static CUI_Text_DTO* Create() { return new CUI_Text_DTO(); }
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CUI_GenericUI_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_GenericUI_DTO() = default;
	virtual ~CUI_GenericUI_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::GENERICUI); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_GenericUIData& Get_Data() const { return m_Data; }
	DTO::TUI_GenericUIData& Get_Data() { return m_Data; }
private:
	DTO::TUI_GenericUIData m_Data;
public:
	static CUI_GenericUI_DTO* Create() { return new CUI_GenericUI_DTO(); }
	virtual void Free() override { Super::Free(); }
};

class ENGINE_DLL CUI_Canvas_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Canvas_DTO() = default;
	virtual ~CUI_Canvas_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::CANVAS); }
	const std::string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_CanvasData& Get_Data() const { return m_Data; }
	DTO::TUI_CanvasData& Get_Data() { return m_Data; }
private:
	DTO::TUI_CanvasData m_Data;
public:
	static CUI_Canvas_DTO* Create() { return new CUI_Canvas_DTO(); }
	virtual void Free() override { Super::Free(); }
};


NS_END