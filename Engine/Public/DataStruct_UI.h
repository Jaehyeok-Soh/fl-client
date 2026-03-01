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
	PROGRESS_COMPONENT = 1 << 1,
	WORLDUI_COMPONENT = 1 << 2
};

enum class EUISubClassType
{
	NONE_OWNER,

	PLAYER_STAT_BEGIN,
	PLAYER_HP,
	PLAYER_ARMOR,
	PLAYER_ENERGY,
	PLAYER_LV,
	PLAYER_STAT_END,

	LOADING_PROGRESS,

	MONSTER_STAT_BEGIN,
	MONSTER_HP,
	MONSTER_ARMOR,
	MONSTER_STAT_END,

	PLAYER_AMMO_PROGRESS,

	BOSS_STAT_BEGIN,
	BOSS_STAT_HP_PROGRESS,
	BOSS_STAT_ARMOR_PROGRESS,
	BOSS_STAT_END,

	END
};

inline std::string UISubClasstypeToString(EUISubClassType eType)
{
	switch (eType)
	{
	case EUISubClassType::NONE_OWNER:				return "NONE_OWNER";

	case EUISubClassType::PLAYER_STAT_BEGIN:		return "PLAYER_STAT_BEGIN";
	case EUISubClassType::PLAYER_HP:				return "PLAYER_HP";
	case EUISubClassType::PLAYER_ARMOR:				return "PLAYER_ARMOR";
	case EUISubClassType::PLAYER_ENERGY:			return "PLAYER_ENERGY";
	case EUISubClassType::PLAYER_LV:				return "PLAYER_LV";
	case EUISubClassType::PLAYER_STAT_END:			return "PLAYER_STAT_END";

	case EUISubClassType::LOADING_PROGRESS:			return "LOADING_PROGRESS";

	case EUISubClassType::MONSTER_STAT_BEGIN:		return "MONSTER_STAT_BEGIN";
	case EUISubClassType::MONSTER_HP:				return "MONSTER_HP";
	case EUISubClassType::MONSTER_ARMOR:			return "MONSTER_ARMOR";
	case EUISubClassType::MONSTER_STAT_END:			return "MONSTER_STAT_END";
	case EUISubClassType::PLAYER_AMMO_PROGRESS:		return "PLAYER_AMMO_PROGRESS";

	case EUISubClassType::BOSS_STAT_BEGIN:			return "BOSS_STAT_BEGIN";
	case EUISubClassType::BOSS_STAT_HP_PROGRESS:	return "BOSS_STAT_HP_PROGRESS";
	case EUISubClassType::BOSS_STAT_ARMOR_PROGRESS:	return "BOSS_STAT_ARMOR_PROGRESS";
	case EUISubClassType::BOSS_STAT_END:			return "BOSS_STAT_END";
			
	case EUISubClassType::END:						return "END";
	default: return "";
	}
}

inline EUISubClassType StringToUISubClassType(const std::string& str)
{
	if (str == "NONE_OWNER")						return EUISubClassType::NONE_OWNER;

	else if (str == "PLAYER_STAT_BEGIN")			return EUISubClassType::PLAYER_STAT_BEGIN;
	else if (str == "PLAYER_HP")					return EUISubClassType::PLAYER_HP;
	else if (str == "PLAYER_ARMOR")					return EUISubClassType::PLAYER_ARMOR;
	else if (str == "PLAYER_ENERGY")				return EUISubClassType::PLAYER_ENERGY;
	else if (str == "PLAYER_LV")					return EUISubClassType::PLAYER_LV;
	else if (str == "PLAYER_STAT_END")				return EUISubClassType::PLAYER_STAT_END;

	else if (str == "LOADING_PROGRESS")				return EUISubClassType::LOADING_PROGRESS;

	else if (str == "MONSTER_STAT_BEGIN")			return EUISubClassType::MONSTER_STAT_BEGIN;
	else if (str == "MONSTER_HP")					return EUISubClassType::MONSTER_HP;
	else if (str == "MONSTER_ARMOR")				return EUISubClassType::MONSTER_ARMOR;
	else if (str == "MONSTER_STAT_END")				return EUISubClassType::MONSTER_STAT_END;

	else if (str == "PLAYER_AMMO_PROGRESS")			return EUISubClassType::PLAYER_AMMO_PROGRESS;

	else if (str == "BOSS_STAT_BEGIN")				return EUISubClassType::BOSS_STAT_BEGIN;
	else if (str == "BOSS_STAT_HP_PROGRESS")		return EUISubClassType::BOSS_STAT_HP_PROGRESS;
	else if (str == "BOSS_STAT_ARMOR_PROGRESS")		return EUISubClassType::BOSS_STAT_ARMOR_PROGRESS;
	else if (str == "BOSS_STAT_END")				return EUISubClassType::BOSS_STAT_END;
	
	else return EUISubClassType::END;
}

NLOHMANN_JSON_SERIALIZE_ENUM(EUISubClassType,
	{
		{EUISubClassType::NONE_OWNER,					"NONE_OWNER"},

		{EUISubClassType::PLAYER_STAT_BEGIN,			"PLAYER_STAT_BEGIN"},
		{EUISubClassType::PLAYER_HP,					"PLAYER_HP"},
		{EUISubClassType::PLAYER_ARMOR,					"PLAYER_ARMOR"},
		{EUISubClassType::PLAYER_ENERGY,				"PLAYER_ENERGY"},
		{EUISubClassType::PLAYER_LV,					"PLAYER_LV"},
		{EUISubClassType::PLAYER_STAT_END,				"PLAYER_STAT_END"},

		{EUISubClassType::LOADING_PROGRESS,				"LOADING_PROGRESS"},

		{EUISubClassType::MONSTER_STAT_BEGIN,			"MONSTER_STAT_BEGIN"},
		{EUISubClassType::MONSTER_HP,					"MONSTER_HP"},
		{ EUISubClassType::MONSTER_ARMOR,				"MONSTER_ARMOR" },
		{ EUISubClassType::MONSTER_STAT_END,			"MONSTER_STAT_END"},

		{ EUISubClassType::PLAYER_AMMO_PROGRESS,		"PLAYER_AMMO_PROGRESS"},

		{ EUISubClassType::BOSS_STAT_BEGIN,				"BOSS_STAT_BEGIN"},
		{ EUISubClassType::BOSS_STAT_HP_PROGRESS,		"BOSS_STAT_HP_PROGRESS"},
		{ EUISubClassType::BOSS_STAT_ARMOR_PROGRESS,	"BOSS_STAT_ARMOR_PROGRESS"},
		{ EUISubClassType::BOSS_STAT_END,				"BOSS_STAT_END"},
	})

#pragma region 텍스트 서브 클래스
	enum class EUITextSubClassType
{
	NONE_OWNER,
	// 플레이어 스탯 
	PLAYER_STAT_TEXT_BEGIN,

	PLAYER_STAT_TEXT_LV,
	PLAYER_STAT_TEXT_HP,
	PLAYER_STAT_TEXT_ARMOR,
	PLAYER_STAT_TEXT_ENERGY,

	PLAYER_STAT_TEXT_ESKILL_TYPE,
	PLAYER_STAT_TEXT_ESKILL_COOLTIME,
	PLAYER_STAT_TEXT_ESKILL_COST,

	PLAYER_STAT_TEXT_QSKILL_TYPE,
	PLAYER_STAT_TEXT_QSKILL_COOLTIME,
	PLAYER_STAT_TEXT_QSKILL_COST,

	PLAYER_STAT_TEXT_ZSKILL_TYPE,
	PLAYER_STAT_TEXT_ZSKILL_COOLTIME,
	PLAYER_STAT_TEXT_ZSKILL_COST,

	PLAYER_STAT_TEXT_DODGESKILL_COOLTIME,
	PLAYER_STAT_TEXT_DODGESKILL_COUNT,
	PLAYER_STAT_TEXT_MAX_BULLET_COUNT,
	PLAYER_STAT_TEXT_CUR_BULLET_COUNT,

	PLAYER_STAT_TEXT_END,

	// 메뉴창 
	MENU_TEXT_BEGIN,
	MENU_ESC_TEXT,
	MENU_ICON_TEXT,
	MENU_TEXT_END,

	// 로딩창
	LOADING_TEXT_BEGIN,
	LOADING_TEXT_TITLE,
	LOADING_TEXT_CONTENTS,
	LOADING_TEXT_PERCENT,
	LOADING_TEXT_END,
	
	// 몬스터 스탯
	MONSTER_STAT_TEXT_BEGIN,
	MONSTER_STAT_TEXT_LV,
	MONSTER_STAT_TEXT_NICKNAME,
	MONSTER_STAT_TEXT_END,

	// 데미지 폰트
	BATTLE_DAMAGE_TEXT_BEGIN,
	BATTLE_DAMAGE_TEXT_COMMON,
	BATTLE_DAMAGE_TEXT_HIT,
	BATTLE_DAMAGE_TEXT_CRITCAL,
	BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE,
	BATTLE_DAMAGE_TEXT_END,

	// 보스 스탯
	BOSS_STAT_TEXT_BEGIN,
	BOSS_STAT_TEXT_LV,
	BOSS_STAT_TEXT_NICKNAME,
	BOSS_STAT_TEXT_END,

	END
};

NLOHMANN_JSON_SERIALIZE_ENUM(EUITextSubClassType,
	{
		{ EUITextSubClassType::NONE_OWNER,							"NONE_OWNER" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_BEGIN,				"PLAYER_STAT_TEXT_BEGIN" },

		{ EUITextSubClassType::PLAYER_STAT_TEXT_LV,					"PLAYER_STAT_TEXT_LV" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_HP,					"PLAYER_STAT_TEXT_HP" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_ARMOR,				"PLAYER_STAT_TEXT_ARMOR" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_ENERGY,				"PLAYER_STAT_TEXT_ENERGY" },

		{ EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_TYPE,		"PLAYER_STAT_TEXT_ESKILL_TYPE" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COOLTIME,	"PLAYER_STAT_TEXT_ESKILL_COOLTIME" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COST,		"PLAYER_STAT_TEXT_ESKILL_COST" },

		{ EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_TYPE,		"PLAYER_STAT_TEXT_QSKILL_TYPE" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COOLTIME,	"PLAYER_STAT_TEXT_QSKILL_COOLTIME" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COST,		"PLAYER_STAT_TEXT_QSKILL_COST" },

		{ EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_TYPE,		"PLAYER_STAT_TEXT_ZSKILL_TYPE" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COOLTIME,	"PLAYER_STAT_TEXT_ZSKILL_COOLTIME" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COST,		"PLAYER_STAT_TEXT_ZSKILL_COST" },

		{ EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COOLTIME,"PLAYER_STAT_TEXT_DODGESKILL_COOLTIME" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COUNT,	"PLAYER_STAT_TEXT_DODGESKILL_COUNT" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_MAX_BULLET_COUNT,	"PLAYER_STAT_TEXT_MAX_BULLET_COUNT" },
		{ EUITextSubClassType::PLAYER_STAT_TEXT_CUR_BULLET_COUNT,	"PLAYER_STAT_TEXT_CUR_BULLET_COUNT" },

		{ EUITextSubClassType::PLAYER_STAT_TEXT_END,				"PLAYER_STAT_TEXT_END" },

		{ EUITextSubClassType::MENU_TEXT_BEGIN,						"MENU_TEXT_BEGIN" },
		{ EUITextSubClassType::MENU_ESC_TEXT,						"MENU_ESC_TEXT" },
		{ EUITextSubClassType::MENU_ICON_TEXT,						"MENU_ICON_TEXT" },
		{ EUITextSubClassType::MENU_TEXT_END,						"MENU_TEXT_END" },

		{ EUITextSubClassType::LOADING_TEXT_BEGIN,					"LOADING_TEXT_BEGIN" },
		{ EUITextSubClassType::LOADING_TEXT_TITLE,					"LOADING_TEXT_TITLE" },
		{ EUITextSubClassType::LOADING_TEXT_CONTENTS,				"LOADING_TEXT_CONTENTS" },
		{ EUITextSubClassType::LOADING_TEXT_PERCENT,				"LOADING_TEXT_PERCENT" },
		{ EUITextSubClassType::LOADING_TEXT_END,					"LOADING_TEXT_END" },

		{ EUITextSubClassType::MONSTER_STAT_TEXT_BEGIN,				"MONSTER_STAT_TEXT_BEGIN" },
		{ EUITextSubClassType::MONSTER_STAT_TEXT_LV,				"MONSTER_STAT_TEXT_LV" },
		{ EUITextSubClassType::MONSTER_STAT_TEXT_NICKNAME,			"MONSTER_STAT_TEXT_NICKNAME" },
		{ EUITextSubClassType::MONSTER_STAT_TEXT_END,				"MONSTER_STAT_TEXT_END" },
		
		{ EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN,		  	"BATTLE_DAMAGE_TEXT_BEGIN" },
		{ EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON,		  	"BATTLE_DAMAGE_TEXT_COMMON" },
		{ EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT,			  	"BATTLE_DAMAGE_TEXT_HIT" },
		{ EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL,		  	"BATTLE_DAMAGE_TEXT_CRITCAL" },
		{ EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE,	"BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE" },
		{ EUITextSubClassType::BATTLE_DAMAGE_TEXT_END,			  	"BATTLE_DAMAGE_TEXT_END" },

		{ EUITextSubClassType::BOSS_STAT_TEXT_BEGIN,			  	"BOSS_STAT_TEXT_BEGIN" },
		{ EUITextSubClassType::BOSS_STAT_TEXT_LV,			  		"BOSS_STAT_TEXT_LV" },
		{ EUITextSubClassType::BOSS_STAT_TEXT_NICKNAME,			  	"BOSS_STAT_TEXT_NICKNAME" },
		{ EUITextSubClassType::BOSS_STAT_TEXT_END,			  		"BOSS_STAT_TEXT_END" },
	  

		{ EUITextSubClassType::END,									"END" },
	})
	inline EUITextSubClassType StringToUITextSubClassType(const std::string& str)
{
	if (str == "NONE_OWNER")									return EUITextSubClassType::NONE_OWNER;
	else if (str == "PLAYER_STAT_TEXT_BEGIN")					return EUITextSubClassType::PLAYER_STAT_TEXT_BEGIN;

	else if (str == "PLAYER_STAT_TEXT_LV")						return EUITextSubClassType::PLAYER_STAT_TEXT_LV;
	else if (str == "PLAYER_STAT_TEXT_HP")						return EUITextSubClassType::PLAYER_STAT_TEXT_HP;
	else if (str == "PLAYER_STAT_TEXT_ARMOR")					return EUITextSubClassType::PLAYER_STAT_TEXT_ARMOR;
	else if (str == "PLAYER_STAT_TEXT_ENERGY")					return EUITextSubClassType::PLAYER_STAT_TEXT_ENERGY;

	else if (str == "PLAYER_STAT_TEXT_ESKILL_TYPE")				return EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_TYPE;
	else if (str == "PLAYER_STAT_TEXT_ESKILL_COOLTIME")			return EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COOLTIME;
	else if (str == "PLAYER_STAT_TEXT_ESKILL_COST")				return EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COST;

	else if (str == "PLAYER_STAT_TEXT_QSKILL_TYPE")				return EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_TYPE;
	else if (str == "PLAYER_STAT_TEXT_QSKILL_COOLTIME")			return EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COOLTIME;
	else if (str == "PLAYER_STAT_TEXT_QSKILL_COST")				return EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COST;

	else if (str == "PLAYER_STAT_TEXT_ZSKILL_TYPE")				return EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_TYPE;
	else if (str == "PLAYER_STAT_TEXT_ZSKILL_COOLTIME")			return EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COOLTIME;
	else if (str == "PLAYER_STAT_TEXT_ZSKILL_COST")				return EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COST;

	else if (str == "PLAYER_STAT_TEXT_DODGESKILL_COOLTIME")		return EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COOLTIME;
	else if (str == "PLAYER_STAT_TEXT_DODGESKILL_COUNT")		return EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COUNT;
	else if (str == "PLAYER_STAT_TEXT_MAX_BULLET_COUNT")		return EUITextSubClassType::PLAYER_STAT_TEXT_MAX_BULLET_COUNT;
	else if (str == "PLAYER_STAT_TEXT_CUR_BULLET_COUNT")		return EUITextSubClassType::PLAYER_STAT_TEXT_CUR_BULLET_COUNT;

	else if (str == "PLAYER_STAT_TEXT_END")						return EUITextSubClassType::PLAYER_STAT_TEXT_END;
	else if (str == "MENU_TEXT_BEGIN")							return EUITextSubClassType::MENU_TEXT_BEGIN;
	else if (str == "MENU_ESC_TEXT")							return EUITextSubClassType::MENU_ESC_TEXT;
	else if (str == "MENU_ICON_TEXT")							return EUITextSubClassType::MENU_ICON_TEXT;
	else if (str == "MENU_TEXT_END")							return EUITextSubClassType::MENU_TEXT_END;

	else if (str == "LOADING_TEXT_BEGIN")						return EUITextSubClassType::LOADING_TEXT_BEGIN;
	else if (str == "LOADING_TEXT_TITLE")						return EUITextSubClassType::LOADING_TEXT_TITLE;
	else if (str == "LOADING_TEXT_CONTENTS")					return EUITextSubClassType::LOADING_TEXT_CONTENTS;
	else if (str == "LOADING_TEXT_PERCENT")						return EUITextSubClassType::LOADING_TEXT_PERCENT;
	else if (str == "LOADING_TEXT_END")							return EUITextSubClassType::LOADING_TEXT_END;

	else if (str == "MONSTER_STAT_TEXT_BEGIN")					return EUITextSubClassType::MONSTER_STAT_TEXT_BEGIN;
	else if (str == "MONSTER_STAT_TEXT_LV")						return EUITextSubClassType::MONSTER_STAT_TEXT_LV;
	else if (str == "MONSTER_STAT_TEXT_NICKNAME")				return EUITextSubClassType::MONSTER_STAT_TEXT_NICKNAME;
	else if (str == "MONSTER_STAT_TEXT_END")					return EUITextSubClassType::MONSTER_STAT_TEXT_END;
	
	else if (str == "BATTLE_DAMAGE_TEXT_BEGIN")					return EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN;
	else if (str == "BATTLE_DAMAGE_TEXT_COMMON")				return EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON;
	else if (str == "BATTLE_DAMAGE_TEXT_HIT")					return EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT;
	else if (str == "BATTLE_DAMAGE_TEXT_CRITCAL")				return EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL;
	else if (str == "BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE")		return EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE;
	else if (str == "BATTLE_DAMAGE_TEXT_END")					return EUITextSubClassType::BATTLE_DAMAGE_TEXT_END;

	else if (str == "BOSS_STAT_TEXT_BEGIN")						return EUITextSubClassType::BOSS_STAT_TEXT_BEGIN;
	else if (str == "BOSS_STAT_TEXT_LV")						return EUITextSubClassType::BOSS_STAT_TEXT_LV;
	else if (str == "BOSS_STAT_TEXT_NICKNAME")					return EUITextSubClassType::BOSS_STAT_TEXT_NICKNAME;
	else if (str == "BOSS_STAT_TEXT_END")						return EUITextSubClassType::BOSS_STAT_TEXT_END;
	
	else if (str == "END")										return EUITextSubClassType::END;

	return EUITextSubClassType::END;
}

inline std::string UITextSubClassTypeToString(EUITextSubClassType e)
{
	switch (e)
	{
	case EUITextSubClassType::NONE_OWNER:							return "NONE_OWNER";
	case EUITextSubClassType::PLAYER_STAT_TEXT_BEGIN:				return "PLAYER_STAT_TEXT_BEGIN";

	case EUITextSubClassType::PLAYER_STAT_TEXT_LV:					return "PLAYER_STAT_TEXT_LV";
	case EUITextSubClassType::PLAYER_STAT_TEXT_HP:					return "PLAYER_STAT_TEXT_HP";
	case EUITextSubClassType::PLAYER_STAT_TEXT_ARMOR:				return "PLAYER_STAT_TEXT_ARMOR";
	case EUITextSubClassType::PLAYER_STAT_TEXT_ENERGY:				return "PLAYER_STAT_TEXT_ENERGY";

	case EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_TYPE:			return "PLAYER_STAT_TEXT_ESKILL_TYPE";
	case EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COOLTIME:		return "PLAYER_STAT_TEXT_ESKILL_COOLTIME";
	case EUITextSubClassType::PLAYER_STAT_TEXT_ESKILL_COST:			return "PLAYER_STAT_TEXT_ESKILL_COST";

	case EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_TYPE:			return "PLAYER_STAT_TEXT_QSKILL_TYPE";
	case EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COOLTIME:		return "PLAYER_STAT_TEXT_QSKILL_COOLTIME";
	case EUITextSubClassType::PLAYER_STAT_TEXT_QSKILL_COST:			return "PLAYER_STAT_TEXT_QSKILL_COST";

	case EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_TYPE:			return "PLAYER_STAT_TEXT_ZSKILL_TYPE";
	case EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COOLTIME:		return "PLAYER_STAT_TEXT_ZSKILL_COOLTIME";
	case EUITextSubClassType::PLAYER_STAT_TEXT_ZSKILL_COST:			return "PLAYER_STAT_TEXT_ZSKILL_COST";

	case EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COOLTIME:	return "PLAYER_STAT_TEXT_DODGESKILL_COOLTIME";
	case EUITextSubClassType::PLAYER_STAT_TEXT_DODGESKILL_COUNT:	return "PLAYER_STAT_TEXT_DODGESKILL_COUNT";
	case EUITextSubClassType::PLAYER_STAT_TEXT_MAX_BULLET_COUNT:	return "PLAYER_STAT_TEXT_MAX_BULLET_COUNT";
	case EUITextSubClassType::PLAYER_STAT_TEXT_CUR_BULLET_COUNT:	return "PLAYER_STAT_TEXT_CUR_BULLET_COUNT";
	case EUITextSubClassType::PLAYER_STAT_TEXT_END:					return "PLAYER_STAT_TEXT_END";

	case EUITextSubClassType::MENU_TEXT_BEGIN:						return "MENU_TEXT_BEGIN";
	case EUITextSubClassType::MENU_ESC_TEXT:						return "MENU_ESC_TEXT";
	case EUITextSubClassType::MENU_ICON_TEXT:						return "MENU_ICON_TEXT";
	case EUITextSubClassType::MENU_TEXT_END:						return "MENU_TEXT_END";
	
	case EUITextSubClassType::LOADING_TEXT_BEGIN:					return "LOADING_TEXT_BEGIN";
	case EUITextSubClassType::LOADING_TEXT_TITLE:					return "LOADING_TEXT_TITLE";
	case EUITextSubClassType::LOADING_TEXT_CONTENTS:				return "LOADING_TEXT_CONTENTS";
	case EUITextSubClassType::LOADING_TEXT_PERCENT:					return "LOADING_TEXT_PERCENT";
	case EUITextSubClassType::LOADING_TEXT_END:						return "LOADING_TEXT_END";
	
	case EUITextSubClassType::MONSTER_STAT_TEXT_BEGIN:				return "MONSTER_STAT_TEXT_BEGIN";
	case EUITextSubClassType::MONSTER_STAT_TEXT_LV:					return "MONSTER_STAT_TEXT_LV";
	case EUITextSubClassType::MONSTER_STAT_TEXT_NICKNAME:			return "MONSTER_STAT_TEXT_NICKNAME";
	case EUITextSubClassType::MONSTER_STAT_TEXT_END:				return "MONSTER_STAT_TEXT_END";
	
	case EUITextSubClassType::BATTLE_DAMAGE_TEXT_BEGIN:				return "BATTLE_DAMAGE_TEXT_BEGIN";
	case EUITextSubClassType::BATTLE_DAMAGE_TEXT_COMMON:			return "BATTLE_DAMAGE_TEXT_COMMON";
	case EUITextSubClassType::BATTLE_DAMAGE_TEXT_HIT:				return "BATTLE_DAMAGE_TEXT_HIT";
	case EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITCAL:			return "BATTLE_DAMAGE_TEXT_CRITCAL";
	case EUITextSubClassType::BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE:	return "BATTLE_DAMAGE_TEXT_CRITICAL_DAMAGE";
	case EUITextSubClassType::BATTLE_DAMAGE_TEXT_END:				return "BATTLE_DAMAGE_TEXT_END";

	case EUITextSubClassType::BOSS_STAT_TEXT_BEGIN:					return "BOSS_STAT_TEXT_BEGIN";
	case EUITextSubClassType::BOSS_STAT_TEXT_LV:					return "BOSS_STAT_TEXT_LV";
	case EUITextSubClassType::BOSS_STAT_TEXT_NICKNAME:				return "BOSS_STAT_TEXT_NICKNAME";
	case EUITextSubClassType::BOSS_STAT_TEXT_END:					return "BOSS_STAT_TEXT_END";
	
	default:														return "END";
	}
}

NLOHMANN_JSON_SERIALIZE_ENUM(EFontPivotType, {
	{ EFontPivotType::CENTER, "CENTER" },
	{ EFontPivotType::LEFT,   "LEFT"   },
	{ EFontPivotType::RIGHT,  "RIGHT"  },
	{ EFontPivotType::UP,     "UP"     },
	{ EFontPivotType::DOWN,   "DOWN"   },
	{ EFontPivotType::END,    "END"    },
	})

	inline EFontPivotType StringToFontPivotType(const std::string& str)
{
	if (str == "CENTER") return EFontPivotType::CENTER;
	if (str == "LEFT")   return EFontPivotType::LEFT;
	if (str == "RIGHT")  return EFontPivotType::RIGHT;
	if (str == "UP")     return EFontPivotType::UP;
	if (str == "DOWN")   return EFontPivotType::DOWN;
	if (str == "END")    return EFontPivotType::END;
	return EFontPivotType::END;
}

inline const char* FontPivotTypeToString(const EFontPivotType eType)
{
	switch (eType)
	{
	case EFontPivotType::CENTER: return "CENTER";
	case EFontPivotType::LEFT:   return "LEFT";
	case EFontPivotType::RIGHT:  return "RIGHT";
	case EFontPivotType::UP:     return "UP";
	case EFontPivotType::DOWN:   return "DOWN";
	case EFontPivotType::END:    return "END";
	default:                     return "END";
	}
}

NLOHMANN_JSON_SERIALIZE_ENUM(EFontShaderType,
	{
		{ EFontShaderType::NORMAL,				"NORMAL" },
		{ EFontShaderType::OUTLINE,				"OUTLINE" },
		{ EFontShaderType::NOISE,				"NOISE" },
		{ EFontShaderType::NOISE_KOR,			"NOISE_KOR" },
		{ EFontShaderType::OUTLINE_NOISE,		"OUTLINE_NOISE" },
		{ EFontShaderType::OUTLINE_NOISE_KOR,	"OUTLINE_NOISE_KOR" },
		{ EFontShaderType::END,					"END" }
	})

	inline EFontShaderType StringToFontShaderType(const std::string_view str)
{
	if (str == "NORMAL")				return EFontShaderType::NORMAL;
	if (str == "OUTLINE")				return EFontShaderType::OUTLINE;
	if (str == "NOISE")					return EFontShaderType::NOISE;
	if (str == "NOISE_KOR")				return EFontShaderType::NOISE_KOR;
	if (str == "OUTLINE_NOISE")			return EFontShaderType::OUTLINE_NOISE;
	if (str == "OUTLINE_NOISE_KOR")		return EFontShaderType::OUTLINE_NOISE_KOR;
	if (str == "END")					return EFontShaderType::END;
	return EFontShaderType::END;
}

inline const char* FontShaderTypeToString(const EFontShaderType eType)
{
	switch (eType)
	{
	case EFontShaderType::NORMAL:				return "NORMAL";
	case EFontShaderType::OUTLINE:				return "OUTLINE";
	case EFontShaderType::NOISE:				return "NOISE";
	case EFontShaderType::NOISE_KOR:			return "NOISE_KOR";
	case EFontShaderType::OUTLINE_NOISE:		return "OUTLINE_NOISE";
	case EFontShaderType::OUTLINE_NOISE_KOR:	return "OUTLINE_NOISE_KOR";
	case EFontShaderType::END:					return "END";
	default:									return "END";
	}
}
#pragma endregion

#pragma region 다이나믹 이미지 서브 클래스

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
	MENU_ICON_OUTLINE,
	MENU_END,

	//로딩화면
	LOADING_BEGIN,
	LOADING_BG,
	LOADING_BG_TOP,
	LOADING_BG_BOTTOM,
	LOADING_END,

	// 전투 UI
	BATTLE_UI_BEGIN,
	BATTLE_AIMDOT_COMMON,
	BATTLE_AIMDOT_CROSSHAIR_TOP,
	BATTLE_AIMDOT_CROSSHAIR_RIGHT,
	BATTLE_AIMDOT_CROSSHAIR_BOTTOM,
	BATTLE_AIMDOT_CROSSHAIR_LEFT,
	BATTLE_AIM_HIT,
	BATTLE_AIM_LOCK,
	BATTLE_UI_END,

	// 몬스터 네임플레이트
	MONSTER_NAMEPLATE_BG,

	// 유틸 
	LEVEL_CHAGE_1,
	LEVEL_CHAGE_2,
	LEVEL_CHAGE_3,
	LEVEL_CHAGE_4,
	LEVEL_CHAGE_5,

	// 보스 스탯
	BOSS_STAT_BEGIN,
	BOSS_STAT_BG,
	BOSS_STAT_END,

	// 콤보 
	BATTLE_COMBO,

	END
};

NLOHMANN_JSON_SERIALIZE_ENUM(EUIDImageSubClassType,
	{
	{ EUIDImageSubClassType::NONE_OWNER,						"NONE_OWNER" },

	{ EUIDImageSubClassType::PLAYER_SKILL_BEGIN,				"PLAYER_SKILL_BEGIN" },
	{ EUIDImageSubClassType::PLAYER_E,							"PLAYER_E" },
	{ EUIDImageSubClassType::PLAYER_Q,							"PLAYER_Q" },
	{ EUIDImageSubClassType::PLAYER_Z,							"PLAYER_Z" },
	{ EUIDImageSubClassType::PLAYER_GUN,						"PLAYER_GUN" },
	{ EUIDImageSubClassType::PLAYER_DODGE,						"PLAYER_DODGE" },
	{ EUIDImageSubClassType::PLAYER_SKILL_END,					"PLAYER_SKILL_END" },

	{ EUIDImageSubClassType::HOVER_POPUP_BEGIN,					"HOVER_POPUP_BEGIN" },
	{ EUIDImageSubClassType::HOVER_POPUP_BG,					"HOVER_POPUP_BG" },
	{ EUIDImageSubClassType::HOVER_POPUP_ICON,					"HOVER_POPUP_ICON" },
	{ EUIDImageSubClassType::HOVER_POPUP_TEXT,					"HOVER_POPUP_TEXT" },
	{ EUIDImageSubClassType::HOVER_POPUP_END,					"HOVER_POPUP_END" },

	{ EUIDImageSubClassType::MINIMAP_BEGIN,						"MINIMAP_BEGIN" },
	{ EUIDImageSubClassType::MINIMAP_PLAYER_ICON,				"MINIMAP_PLAYER_ICON" },
	{ EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT,				"MINIMAP_CAMERA_SIGHT" },
	{ EUIDImageSubClassType::MINIMAP_BGFRAME,					"MINIMAP_BGFRAME" },
	{ EUIDImageSubClassType::MINIMAP_WARNING_FRAME,				"MINIMAP_WARNING_FRAME" },
	{ EUIDImageSubClassType::MINIMAP_END,						"MINIMAP_END" },

	{ EUIDImageSubClassType::MENU_BEGIN,						"MENU_BEGIN" },
	{ EUIDImageSubClassType::MENU_BG,							"MENU_BG" },
	{ EUIDImageSubClassType::MENU_ICON,							"MENU_ICON" },
	{ EUIDImageSubClassType::MENU_ICON_BG,						"MENU_ICON_BG" },
	{ EUIDImageSubClassType::MENU_ICON_OUTLINE,					"MENU_ICON_OUTLINE" },
	{ EUIDImageSubClassType::MENU_END,							"MENU_END" },

	{ EUIDImageSubClassType::LOADING_BEGIN,						"LOADING_BEGIN" },
	{ EUIDImageSubClassType::LOADING_BG,						"LOADING_BG" },
	{ EUIDImageSubClassType::LOADING_BG_TOP,					"LOADING_BG_TOP" },
	{ EUIDImageSubClassType::LOADING_BG_BOTTOM,					"LOADING_BG_BOTTOM" },
	{ EUIDImageSubClassType::LOADING_END,						"LOADING_END" },

	{ EUIDImageSubClassType::BATTLE_UI_BEGIN,					"BATTLE_UI_BEGIN" },
	{ EUIDImageSubClassType::BATTLE_AIMDOT_COMMON,				"BATTLE_AIMDOT_COMMON" },
	{ EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP,		"BATTLE_AIMDOT_CROSSHAIR_TOP" },
	{ EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT,		"BATTLE_AIMDOT_CROSSHAIR_RIGHT" },
	{ EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM,	"BATTLE_AIMDOT_CROSSHAIR_BOTTOM" },
	{ EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT,		"BATTLE_AIMDOT_CROSSHAIR_LEFT" },
	{ EUIDImageSubClassType::BATTLE_AIM_HIT,					"BATTLE_AIM_HIT" },
	{ EUIDImageSubClassType::BATTLE_AIM_LOCK,					"BATTLE_AIM_LOCK" },
	{ EUIDImageSubClassType::BATTLE_UI_END,						"BATTLE_UI_END" },
		
	{ EUIDImageSubClassType::MONSTER_NAMEPLATE_BG,				"MONSTER_NAMEPLATE_BG" },

	{ EUIDImageSubClassType::LEVEL_CHAGE_1,						"LEVEL_CHAGE_1" },
	{ EUIDImageSubClassType::LEVEL_CHAGE_2,						"LEVEL_CHAGE_2" },
	{ EUIDImageSubClassType::LEVEL_CHAGE_3,						"LEVEL_CHAGE_3" },
	{ EUIDImageSubClassType::LEVEL_CHAGE_4,						"LEVEL_CHAGE_4" },
	{ EUIDImageSubClassType::LEVEL_CHAGE_5,						"LEVEL_CHAGE_5" },

	{ EUIDImageSubClassType::BOSS_STAT_BEGIN,					"BOSS_STAT_BEGIN" },
	{ EUIDImageSubClassType::BOSS_STAT_BG,						"BOSS_STAT_BG" },
	{ EUIDImageSubClassType::BOSS_STAT_END,						"BOSS_STAT_END" },

	{ EUIDImageSubClassType::END,								"END" }
	})

	inline EUIDImageSubClassType StringToUIDImageSubType(const std::string& str)
{
	if (str == "NONE_OWNER")							return EUIDImageSubClassType::NONE_OWNER;

	if (str == "PLAYER_SKILL_BEGIN")					return EUIDImageSubClassType::PLAYER_SKILL_BEGIN;
	if (str == "PLAYER_E")								return EUIDImageSubClassType::PLAYER_E;
	if (str == "PLAYER_Q")								return EUIDImageSubClassType::PLAYER_Q;
	if (str == "PLAYER_Z")								return EUIDImageSubClassType::PLAYER_Z;
	if (str == "PLAYER_GUN")							return EUIDImageSubClassType::PLAYER_GUN;
	if (str == "PLAYER_DODGE")							return EUIDImageSubClassType::PLAYER_DODGE;
	if (str == "PLAYER_SKILL_END")						return EUIDImageSubClassType::PLAYER_SKILL_END;

	if (str == "HOVER_POPUP_BEGIN")						return EUIDImageSubClassType::HOVER_POPUP_BEGIN;
	if (str == "HOVER_POPUP_BG")						return EUIDImageSubClassType::HOVER_POPUP_BG;
	if (str == "HOVER_POPUP_ICON")						return EUIDImageSubClassType::HOVER_POPUP_ICON;
	if (str == "HOVER_POPUP_TEXT")						return EUIDImageSubClassType::HOVER_POPUP_TEXT;
	if (str == "HOVER_POPUP_END")						return EUIDImageSubClassType::HOVER_POPUP_END;

	if (str == "MINIMAP_BEGIN")							return EUIDImageSubClassType::MINIMAP_BEGIN;
	if (str == "MINIMAP_PLAYER_ICON")					return EUIDImageSubClassType::MINIMAP_PLAYER_ICON;
	if (str == "MINIMAP_CAMERA_SIGHT")					return EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT;
	if (str == "MINIMAP_BGFRAME")						return EUIDImageSubClassType::MINIMAP_BGFRAME;
	if (str == "MINIMAP_WARNING_FRAME")					return EUIDImageSubClassType::MINIMAP_WARNING_FRAME;
	if (str == "MINIMAP_END")							return EUIDImageSubClassType::MINIMAP_END;

	if (str == "MENU_BEGIN")							return EUIDImageSubClassType::MENU_BEGIN;
	if (str == "MENU_BG")								return EUIDImageSubClassType::MENU_BG;
	if (str == "MENU_ICON")								return EUIDImageSubClassType::MENU_ICON;
	if (str == "MENU_ICON_BG")							return EUIDImageSubClassType::MENU_ICON_BG;
	if (str == "MENU_ICON_OUTLINE")						return EUIDImageSubClassType::MENU_ICON_OUTLINE;
	if (str == "MENU_END")								return EUIDImageSubClassType::MENU_END;

	if (str == "LOADING_BEGIN")							return EUIDImageSubClassType::LOADING_BEGIN;
	if (str == "LOADING_BG")							return EUIDImageSubClassType::LOADING_BG;
	if (str == "LOADING_BG_TOP")						return EUIDImageSubClassType::LOADING_BG_TOP;
	if (str == "LOADING_BG_BOTTOM")						return EUIDImageSubClassType::LOADING_BG_BOTTOM;
	if (str == "LOADING_END")							return EUIDImageSubClassType::LOADING_END;

	if (str == "BATTLE_UI_BEGIN")						return EUIDImageSubClassType::BATTLE_UI_BEGIN;
	if (str == "BATTLE_AIMDOT_COMMON")					return EUIDImageSubClassType::BATTLE_AIMDOT_COMMON;
	if (str == "BATTLE_AIMDOT_CROSSHAIR_TOP")			return EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP;
	if (str == "BATTLE_AIMDOT_CROSSHAIR_RIGHT")			return EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT;
	if (str == "BATTLE_AIMDOT_CROSSHAIR_BOTTOM")		return EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM;
	if (str == "BATTLE_AIMDOT_CROSSHAIR_LEFT")			return EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT;
	if (str == "BATTLE_AIM_HIT")						return EUIDImageSubClassType::BATTLE_AIM_HIT;
	if (str == "BATTLE_AIM_LOCK")						return EUIDImageSubClassType::BATTLE_AIM_LOCK;
	if (str == "BATTLE_UI_END")							return EUIDImageSubClassType::BATTLE_UI_END;
	
	if (str == "MONSTER_NAMEPLATE_BG")					return EUIDImageSubClassType::MONSTER_NAMEPLATE_BG;

	if (str == "LEVEL_CHAGE_1")							return EUIDImageSubClassType::LEVEL_CHAGE_1;
	if (str == "LEVEL_CHAGE_2")							return EUIDImageSubClassType::LEVEL_CHAGE_2;
	if (str == "LEVEL_CHAGE_3")							return EUIDImageSubClassType::LEVEL_CHAGE_3;
	if (str == "LEVEL_CHAGE_4")							return EUIDImageSubClassType::LEVEL_CHAGE_4;
	if (str == "LEVEL_CHAGE_5")							return EUIDImageSubClassType::LEVEL_CHAGE_5;

	if (str == "BOSS_STAT_BEGIN")						return EUIDImageSubClassType::BOSS_STAT_BEGIN;
	if (str == "BOSS_STAT_BG")							return EUIDImageSubClassType::BOSS_STAT_BG;
	if (str == "BOSS_STAT_END")							return EUIDImageSubClassType::BOSS_STAT_END;

	if (str == "END")									return EUIDImageSubClassType::END;
	return EUIDImageSubClassType::NONE_OWNER;
}

inline const char* UIDImageSubTypeToString(EUIDImageSubClassType type)
{
	switch (type)
	{
	case EUIDImageSubClassType::NONE_OWNER:							return "NONE_OWNER";

	case EUIDImageSubClassType::PLAYER_SKILL_BEGIN:					return "PLAYER_SKILL_BEGIN";
	case EUIDImageSubClassType::PLAYER_E:							return "PLAYER_E";
	case EUIDImageSubClassType::PLAYER_Q:							return "PLAYER_Q";
	case EUIDImageSubClassType::PLAYER_Z:							return "PLAYER_Z";
	case EUIDImageSubClassType::PLAYER_GUN:							return "PLAYER_GUN";
	case EUIDImageSubClassType::PLAYER_DODGE:						return "PLAYER_DODGE";
	case EUIDImageSubClassType::PLAYER_SKILL_END:					return "PLAYER_SKILL_END";

	case EUIDImageSubClassType::HOVER_POPUP_BEGIN:					return "HOVER_POPUP_BEGIN";
	case EUIDImageSubClassType::HOVER_POPUP_BG:						return "HOVER_POPUP_BG";
	case EUIDImageSubClassType::HOVER_POPUP_ICON:					return "HOVER_POPUP_ICON";
	case EUIDImageSubClassType::HOVER_POPUP_TEXT:					return "HOVER_POPUP_TEXT";
	case EUIDImageSubClassType::HOVER_POPUP_END:					return "HOVER_POPUP_END";

	case EUIDImageSubClassType::MINIMAP_BEGIN:						return "MINIMAP_BEGIN";
	case EUIDImageSubClassType::MINIMAP_PLAYER_ICON:				return "MINIMAP_PLAYER_ICON";
	case EUIDImageSubClassType::MINIMAP_CAMERA_SIGHT:				return "MINIMAP_CAMERA_SIGHT";
	case EUIDImageSubClassType::MINIMAP_BGFRAME:					return "MINIMAP_BGFRAME";
	case EUIDImageSubClassType::MINIMAP_WARNING_FRAME:				return "MINIMAP_WARNING_FRAME";
	case EUIDImageSubClassType::MINIMAP_END:						return "MINIMAP_END";

	case EUIDImageSubClassType::MENU_BEGIN:							return "MENU_BEGIN";
	case EUIDImageSubClassType::MENU_BG:							return "MENU_BG";
	case EUIDImageSubClassType::MENU_ICON:							return "MENU_ICON";
	case EUIDImageSubClassType::MENU_ICON_BG:						return "MENU_ICON_BG";
	case EUIDImageSubClassType::MENU_ICON_OUTLINE:					return "MENU_ICON_OUTLINE";
	case EUIDImageSubClassType::MENU_END:							return "MENU_END";
	
	case EUIDImageSubClassType::LOADING_BEGIN:						return "LOADING_BEGIN";
	case EUIDImageSubClassType::LOADING_BG:							return "LOADING_BG";
	case EUIDImageSubClassType::LOADING_BG_TOP:						return "LOADING_BG_TOP";
	case EUIDImageSubClassType::LOADING_BG_BOTTOM:					return "LOADING_BG_BOTTOM";
	case EUIDImageSubClassType::LOADING_END:						return "LOADING_END";

	case EUIDImageSubClassType::BATTLE_UI_BEGIN:					return "BATTLE_UI_BEGIN";
	case EUIDImageSubClassType::BATTLE_AIMDOT_COMMON:				return "BATTLE_AIMDOT_COMMON";
	case EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_TOP:		return "BATTLE_AIMDOT_CROSSHAIR_TOP";
	case EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_RIGHT:		return "BATTLE_AIMDOT_CROSSHAIR_RIGHT";
	case EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_BOTTOM:		return "BATTLE_AIMDOT_CROSSHAIR_BOTTOM";
	case EUIDImageSubClassType::BATTLE_AIMDOT_CROSSHAIR_LEFT:		return "BATTLE_AIMDOT_CROSSHAIR_LEFT";
	case EUIDImageSubClassType::BATTLE_AIM_HIT:						return "BATTLE_AIM_HIT";
	case EUIDImageSubClassType::BATTLE_AIM_LOCK:					return "BATTLE_AIM_LOCK";
	case EUIDImageSubClassType::BATTLE_UI_END:						return "BATTLE_UI_END";
	
	case EUIDImageSubClassType::MONSTER_NAMEPLATE_BG:				return "MONSTER_NAMEPLATE_BG";

	case EUIDImageSubClassType::LEVEL_CHAGE_1:						return "LEVEL_CHAGE_1";
	case EUIDImageSubClassType::LEVEL_CHAGE_2:						return "LEVEL_CHAGE_2";
	case EUIDImageSubClassType::LEVEL_CHAGE_3:						return "LEVEL_CHAGE_3";
	case EUIDImageSubClassType::LEVEL_CHAGE_4:						return "LEVEL_CHAGE_4";
	case EUIDImageSubClassType::LEVEL_CHAGE_5:						return "LEVEL_CHAGE_5";

	case EUIDImageSubClassType::BOSS_STAT_BEGIN:					return "BOSS_STAT_BEGIN";
	case EUIDImageSubClassType::BOSS_STAT_BG:						return "BOSS_STAT_BG";
	case EUIDImageSubClassType::BOSS_STAT_END:						return "BOSS_STAT_END";

	case EUIDImageSubClassType::END:								return "END";
	default:														return "NONE_OWNER";
	}
}
#pragma endregion

#pragma region 트리거 서브 클래스

enum class EUITriggerSubClassType
{
	NONE_OWNER,

	MENU_TAB_TRIGGER,
	MENU_TAB_EXIT_TRIGGER,

	END
};
NLOHMANN_JSON_SERIALIZE_ENUM(EUITriggerSubClassType,
	{
		{ EUITriggerSubClassType::NONE_OWNER,        "NONE_OWNER" },
		{ EUITriggerSubClassType::MENU_TAB_TRIGGER,  "MENU_TAB_TRIGGER" },
		{ EUITriggerSubClassType::MENU_TAB_EXIT_TRIGGER,  "MENU_TAB_EXIT_TRIGGER" },
	})
	inline EUITriggerSubClassType StringToUITriggerSubClassType(const std::string& str)
{
	if (str == "NONE_OWNER")        return EUITriggerSubClassType::NONE_OWNER;
	if (str == "MENU_TAB_TRIGGER")  return EUITriggerSubClassType::MENU_TAB_TRIGGER;
	if (str == "MENU_TAB_EXIT_TRIGGER")  return EUITriggerSubClassType::MENU_TAB_EXIT_TRIGGER;
	return EUITriggerSubClassType::END;
}
inline std::string UITriggerSubClassTypeToString(EUITriggerSubClassType e)
{
	switch (e)
	{
	case EUITriggerSubClassType::NONE_OWNER:       return "NONE_OWNER";
	case EUITriggerSubClassType::MENU_TAB_TRIGGER: return "MENU_TAB_TRIGGER";
	case EUITriggerSubClassType::MENU_TAB_EXIT_TRIGGER: return "MENU_TAB_EXIT_TRIGGER";
	default:                                      return "END";
	}
}

#pragma endregion

#pragma region 월드 유아이 서브 클래스

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

#pragma endregion

#pragma region 유아이 데이터
/////////////////-------------------  Data Struct  -------------------/////////////////
// 텍스트 데이터
struct TUI_TextData
{
	static constexpr EUIType eType = EUIType::UI_TEXT;
	std::string		strTag;
	std::string		strOwnerName;
	EUITextSubClassType		eTextSubClassType;
	EFontShaderType eShaderType;
	std::string		strFontTag;
	std::string	    strText;
	Vec4			vFontColor;	
	EFontPivotType	ePivot;
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
	EUITriggerSubClassType	eTriggerSubClassType;
	
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
	_float			fScale;
	_float			fRotate;
	_string			strTextureTag;
	_bool			isVisible;
	_bool			isInteract;
	_bool			isActivate;
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
	uint32_t	iEditorSizeX;
	uint32_t	iEditorSizeY;

	uint32_t	iPrefabType;
	uint32_t	iNumPrefabs;
};

#pragma endregion

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

#pragma region Wrapping Class
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
#pragma endregion