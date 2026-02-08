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
	PROGRESS_BAR,
	JUST_IMAGE,
	END
};

inline std::string UIClassTypeToString(EUIClassType eType)
{
	switch (eType)
	{
	case EUIClassType::PROGRESS_BAR: return "PROGRESS_BAR";
	case EUIClassType::JUST_IMAGE: return "JUST_IMAGE";
	case EUIClassType::END: return "END";
	default: return "";
	}
}

inline EUIClassType StringToUIClassType(const std::string& str)
{
	if (str == "PROGRESS_BAR") return EUIClassType::PROGRESS_BAR;
	else if (str == "JUST_IMAGE") return EUIClassType::JUST_IMAGE;
	else return EUIClassType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUIClassType,
	{
		{EUIClassType::PROGRESS_BAR, "PROGRESS_BAR"},
		{EUIClassType::JUST_IMAGE, "JUST_IMAGE"},
	})

enum EComponentTypeFlag
{
	BUTTON_COMPONENT = 1 << 0,
	PROGRESS_COMPONENT = 1 << 1
};

enum class EUIOwnerType
{
	NONE_OWNER,
	PLAYER_HP,
	PLAYER_ARMOR,
	PLAYER_ENERGY,
	END
};

inline std::string UIOwnertypeToString(EUIOwnerType eType)
{
	switch (eType)
	{
	case EUIOwnerType::NONE_OWNER: return "NONE_OWNER";
	case EUIOwnerType::PLAYER_HP: return "PLAYER_HP";
	case EUIOwnerType::PLAYER_ARMOR: return "PLAYER_ARMOR";
	case EUIOwnerType::PLAYER_ENERGY: return "PLAYER_ENERGY";
	case EUIOwnerType::END: return "END";
	default: return "";
	}
}

inline EUIOwnerType StringToUIOwnertype(const std::string& str)
{
	if (str == "NONE_OWNER") return EUIOwnerType::NONE_OWNER;
	else if (str == "PLAYER_HP") return EUIOwnerType::PLAYER_HP;
	else if (str == "PLAYER_ARMOR") return EUIOwnerType::PLAYER_ARMOR;
	else if (str == "PLAYER_ENERGY") return EUIOwnerType::PLAYER_ENERGY;
	else return EUIOwnerType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUIOwnerType,
	{
		{EUIOwnerType::NONE_OWNER, "NONE_OWNER"},
		{EUIOwnerType::PLAYER_HP, "PLAYER_HP"},
		{EUIOwnerType::PLAYER_ARMOR, "PLAYER_ARMOR"},
		{EUIOwnerType::PLAYER_ENERGY, "PLAYER_ENERGY"},
	})

/////////////////-------------------  Data Struct  -------------------/////////////////
struct TUI_ActionBindData
{
	std::string strOwnerName;
};

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
	EUIOwnerType	eOwnerType;
	_bool			isUseColorTint;
	Vec4			vColorTint;
	int32_t			iShaderPass;
	int32_t			iFillDir;
	_float			fDelay;
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