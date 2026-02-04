#pragma once
#include "ObjectDataBase.h"

NS_BEGIN(DTO)

/////////////////-------------------  UI  -------------------/////////////////
enum class EUIType : _uint
{
	CANVAS,
	LAYER,
	GENERICUI,
	EVENT,
	END
};
inline constexpr _uint g_UITypeCount{ ENUM_TO_UINT(EUIType::END) };

NLOHMANN_JSON_SERIALIZE_ENUM(EUIType,
	{
		{EUIType::CANVAS, "CANVAS"},
		{EUIType::LAYER, "LAYER"},
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
	END			= 1u << 7
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
	else return DTO::EUIEvent::END;
}

enum class EUIAction
{
	/* (isVisible / bool) */
	SET_VISIBLE,
	/* (index / uint) */
	SET_TEXTURE_INDEX,
	END
};

inline EUIAction StringToUIFunctype(const _string& str)
{
	if (str == "SET_VISIBLE")return EUIAction::SET_VISIBLE;
	else if (str == "SET_TEXTURE_INDEX")return EUIAction::SET_TEXTURE_INDEX;
	else return EUIAction::END;
}

inline _string UIFunctypeToString(EUIAction eType)
{
	switch (eType)
	{
	case DTO::EUIAction::SET_VISIBLE: return "SET_VISIBLE";
	case DTO::EUIAction::SET_TEXTURE_INDEX: return "SET_TEXTURE_INDEX";
	default: return "";
	}
}

/////////////////-------------------  Data Struct  -------------------/////////////////

struct TUI_EventBindData
{
	static constexpr EUIType eType = EUIType::EVENT	;
	std::string strTag;
	std::string strOwnerTag;
	EUIEvent eEvent = EUIEvent::NONE;
	std::string strActionKey;
	json Params;
};

struct TUI_GenericUIData
{
	static constexpr EUIType eType = EUIType::GENERICUI;
	std::string strTag;
	std::string strCanvasName;
	std::string strLayerName;

	uint32_t iRectTransformType;

	_float fWidth;
	_float fHeight;
	_float fPosX;
	_float fPosY;
	_float fPosZ;
	_string strTextureTag;
	uint32_t iTextureIndex;
};

struct TUI_LayerData
{
	static constexpr EUIType eType = EUIType::LAYER;
	std::string strTag;
	std::string strCanvasName;
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
void to_json(json& j, const TUI_EventBindData& data);
void from_json(const json& j, TUI_EventBindData& data);
void to_json(json& j, const TUI_GenericUIData& data);
void from_json(const json& j, TUI_GenericUIData& data);
void to_json(json& j, const TUI_LayerData& data);
void from_json(const json& j, TUI_LayerData& data);
void to_json(json& j, const TUI_CanvasData& data);
void from_json(const json& j, TUI_CanvasData& data);
NS_END

/////////////////-------------------  Wrapping Class  -------------------/////////////////

NS_BEGIN(Engine)

class ENGINE_DLL CUI_EventBindData_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_EventBindData_DTO() = default;
	virtual ~CUI_EventBindData_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::EVENT); }
	const _string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_EventBindData& Get_Data() const { return m_Data; }
	DTO::TUI_EventBindData& Get_Data() { return m_Data; }
private:
	DTO::TUI_EventBindData m_Data;
public:
	static CUI_EventBindData_DTO* Create() { return new CUI_EventBindData_DTO(); }
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

class ENGINE_DLL CUI_Layer_DTO final : public IObjectDataBase
{
	using Super = IObjectDataBase;
private:
	CUI_Layer_DTO() = default;
	virtual ~CUI_Layer_DTO() = default;
public:
	_uint Get_Type() const override { return ENUM_TO_UINT(DTO::EUIType::LAYER); }
	const std::string& Get_Tag() const override { return m_Data.strTag; }

	json ToJson() const override;
	HRESULT FromJson(const json& j) override;

	const DTO::TUI_LayerData& Get_Data() const { return m_Data; }
	DTO::TUI_LayerData& Get_Data() { return m_Data; }
private:
	DTO::TUI_LayerData m_Data;
public:
	static CUI_Layer_DTO* Create() { return new CUI_Layer_DTO(); }
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