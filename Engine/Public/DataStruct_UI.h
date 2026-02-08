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

enum class EUIClassType
{
	PLAYER_HP,
	MONSTER_HP,
	END
};

inline std::string UIClassTypeToString(EUIClassType eType)
{
	switch (eType)
	{
	case EUIClassType::PLAYER_HP: return "PLAYER_HP";
	case EUIClassType::MONSTER_HP: return "MONSTER_HP";
	case EUIClassType::END: return "END";
	default: return "";
	}
}

inline EUIClassType StringToUIClassType(const std::string& str)
{
	if (str == "PLAYER_HP") return EUIClassType::PLAYER_HP;
	else if (str == "MONSTER_HP") return EUIClassType::MONSTER_HP;
	else return EUIClassType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUIClassType,
	{
		{EUIClassType::PLAYER_HP, "PLAYER_HP"},
		{EUIClassType::MONSTER_HP, "MONSTER_HP"},
	})

enum EComponentTypeFlag
{
	BUTTON_COMPONENT = 1 << 0,
	PROGRESS_COMPONENT = 1 << 1
};

enum class EUIOwnerType
{
	NONE_OWNER,
	PLAYER,
	MONSTER,
	BOSS,
	END
};

inline std::string UIOwnertypeToString(EUIOwnerType eType)
{
	switch (eType)
	{
	case EUIOwnerType::NONE_OWNER: return "NONE_OWNER";
	case EUIOwnerType::PLAYER: return "PLAYER";
	case EUIOwnerType::MONSTER: return "MONSTER";
	case EUIOwnerType::BOSS: return "BOSS";
	case EUIOwnerType::END: return "END";
	default: return "";
	}
}

inline EUIOwnerType StringToUIOwnertype(const std::string& str)
{
	if (str == "NONE_OWNER") return EUIOwnerType::NONE_OWNER;
	else if (str == "PLAYER") return EUIOwnerType::PLAYER;
	else if (str == "MONSTER") return EUIOwnerType::MONSTER;
	else if (str == "BOSS") return EUIOwnerType::BOSS;
	else return EUIOwnerType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUIOwnerType,
	{
		{EUIOwnerType::NONE_OWNER, "NONE_OWNER"},
		{EUIOwnerType::PLAYER, "PLAYER"},
		{EUIOwnerType::MONSTER, "MONSTER"},
		{EUIOwnerType::BOSS, "BOSS"},
	})

/////////////////-------------------  Data Struct  -------------------/////////////////

struct TProgress_ComponentData
{

};

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

	uint32_t iComponentFlag;
	EUIOwnerType eOwnerType;

	_bool isUseColorTint;
	Vec4 vColorTint;
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