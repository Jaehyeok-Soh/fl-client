#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

/////////////////-------------------  UI  -------------------/////////////////
enum class EUIType : _uint
{
	CANVAS,
	GENERICUI,
	EVENT,
	VALUE,
	END
};
inline constexpr _uint g_UITypeCount{ ENUM_TO_UINT(EUIType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EUIType,
	{
		{EUIType::CANVAS, "CANVAS"},
		{EUIType::GENERICUI, "GENERICUI"},
		{EUIType::EVENT, "EVENT"}
	}
)

enum class EUIEvent : uint32_t
{
	NONE = 0,
	HOVER_ENTER,
	HOVERING,
	HOVER_EXIT,
	PRESS_ENTER,
	PRESSING,
	PRESS_EXIT,
	INVOKED,
	END
};

enum EUIEvent_Flag : uint32_t
{
	NONE = 0u,
	HOVER_ENTER = 1u << 1,
	HOVERING	= 1u << 2,
	HOVER_EXIT	= 1u << 3,
	PRESS_ENTER = 1u << 4,
	PRESSING	= 1u << 5,
	PRESS_EXIT	= 1u << 6,
	INVOKED		= 1u << 7,
	END			= 1u << 8
};

inline DTO::EUIEvent EventFlagToEvent(DTO::EUIEvent_Flag eFlag)
{
	switch (eFlag)
	{
	case DTO::EUIEvent_Flag::NONE:			return DTO::EUIEvent::NONE;
	case DTO::EUIEvent_Flag::HOVER_ENTER:	return DTO::EUIEvent::HOVER_ENTER;
	case DTO::EUIEvent_Flag::HOVERING:		return DTO::EUIEvent::HOVERING;
	case DTO::EUIEvent_Flag::HOVER_EXIT:	return DTO::EUIEvent::HOVER_EXIT;
	case DTO::EUIEvent_Flag::PRESS_ENTER:	return DTO::EUIEvent::PRESS_ENTER;
	case DTO::EUIEvent_Flag::PRESSING:		return DTO::EUIEvent::PRESSING;
	case DTO::EUIEvent_Flag::PRESS_EXIT:	return DTO::EUIEvent::PRESS_EXIT;
	case DTO::EUIEvent_Flag::INVOKED:		return DTO::EUIEvent::INVOKED;
	default:								return DTO::EUIEvent::NONE;
	}
}

inline DTO::EUIEvent_Flag EventToEventFlag(DTO::EUIEvent eEvent)
{
	switch (eEvent)
	{
	case DTO::EUIEvent::NONE:			return DTO::EUIEvent_Flag::NONE;
	case DTO::EUIEvent::HOVER_ENTER:	return DTO::EUIEvent_Flag::HOVER_ENTER;
	case DTO::EUIEvent::HOVERING:		return DTO::EUIEvent_Flag::HOVERING;
	case DTO::EUIEvent::HOVER_EXIT:		return DTO::EUIEvent_Flag::HOVER_EXIT;
	case DTO::EUIEvent::PRESS_ENTER:	return DTO::EUIEvent_Flag::PRESS_ENTER;
	case DTO::EUIEvent::PRESSING:		return DTO::EUIEvent_Flag::PRESSING;
	case DTO::EUIEvent::PRESS_EXIT:		return DTO::EUIEvent_Flag::PRESS_EXIT;
	case DTO::EUIEvent::INVOKED:		return DTO::EUIEvent_Flag::INVOKED;
	default:							return DTO::EUIEvent_Flag::NONE;
	}
}


NLOHMANN_JSON_SERIALIZE_ENUM(EUIEvent,
	{
		{EUIEvent::NONE, "NONE"},
		{EUIEvent::HOVER_ENTER, "HOVER_ENTER"},
		{EUIEvent::HOVERING, "HOVERING"},
		{EUIEvent::HOVER_EXIT, "HOVER_EXIT"},
		{EUIEvent::PRESS_ENTER, "PRESS_ENTER"},
		{EUIEvent::PRESSING, "PRESSING"},
		{EUIEvent::PRESS_EXIT, "PRESS_EXIT"},
		{EUIEvent::INVOKED, "INVOKED"},
	})

inline std::string UIEventToString(DTO::EUIEvent eType)
{
	switch (eType)
	{
	case DTO::EUIEvent::NONE: return "NONE";
	case DTO::EUIEvent::HOVER_ENTER: return "HOVER_ENTER";
	case DTO::EUIEvent::HOVERING: return "HOVERING";
	case DTO::EUIEvent::HOVER_EXIT: return "HOVER_EXIT";
	case DTO::EUIEvent::PRESS_ENTER: return "PRESS_ENTER";
	case DTO::EUIEvent::PRESSING: return "PRESSING";
	case DTO::EUIEvent::PRESS_EXIT: return "PRESS_EXIT";
	case DTO::EUIEvent::INVOKED: return "INVOKED";
	default: return "";
	}
}

inline DTO::EUIEvent StringToUIEvent(const std::string& str)
{
	if (str == "NONE") return DTO::EUIEvent::NONE;
	else if (str == "HOVER_ENTER") return DTO::EUIEvent::HOVER_ENTER;
	else if (str == "HOVERING") return DTO::EUIEvent::HOVERING;
	else if (str == "HOVER_EXIT") return DTO::EUIEvent::HOVER_EXIT;
	else if (str == "PRESS_ENTER") return DTO::EUIEvent::PRESS_ENTER;
	else if (str == "PRESSING") return DTO::EUIEvent::PRESSING;
	else if (str == "PRESS_EXIT") return DTO::EUIEvent::PRESS_EXIT;
	else if (str == "INVOKED") return DTO::EUIEvent::INVOKED;
	else return DTO::EUIEvent::END;
}

enum class EUIClassType
{
	PLAYER_HP,
	END
};

inline std::string UIClassTypeToString(EUIClassType eType)
{
	switch (eType)
	{
	case EUIClassType::PLAYER_HP: return "PLAYER_HP";
	case EUIClassType::END: return "END";
	default: return "";
	}
}

inline EUIClassType StringToUIClassType(const std::string& str)
{
	if (str == "PLAYER_HP") return EUIClassType::PLAYER_HP;
	else return EUIClassType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUIClassType,
	{
		{EUIClassType::PLAYER_HP, "PLAYER_HP"},
	})


/////////////////-------------------  Data Struct  -------------------/////////////////

struct TUI_GenericUIData
{
	static constexpr EUIType eType = EUIType::GENERICUI;
	EUIClassType eClassType;

	std::string strTag;
	std::string strCanvasName;

	uint32_t iRectTransformType;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;
	_string strTextureTag;
	uint32_t iTextureIndex;
	_bool isVisible;
};

struct TUI_CanvasData
{
	static constexpr EUIType eType = EUIType::CANVAS;
	std::string strTag;

	uint32_t iLevelIndex;
	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;

	uint32_t iEditorSizeX ;
	uint32_t iEditorSizeY;
};

/////////////////-------------------  to_json, from_json  -------------------/////////////////
void to_json(json& j, const TUI_GenericUIData& data);
void from_json(const json& j, TUI_GenericUIData& data);
void to_json(json& j, const TUI_CanvasData& data);
void from_json(const json& j, TUI_CanvasData& data);
NS_END
/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

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