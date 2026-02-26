#pragma once

#include "Engine_Define.h"
#include "EngineConsole.h"
#include <windows.h>


extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;
extern _bool		g_bStart;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;
	enum class ELevelType : unsigned int
	{
		STATIC = 0,
		LOADING,
		LOGO,				/* 현재 임시 Test Level용 추후 Logo Scene으로 바뀔예정  */
		TUTORIAL_VILLAGE,	/* 튜토리얼 처음 진입되는 Level */
		TUTORIAL_BOSS,		/* 튜토리얼 진입 이후 Boss전 가는 Level Type */
		SQUARE,				/* 광장 */


		/* Test Scene은 맨 마지막 */
		TEST,
		END
	};
	inline constexpr size_t g_iLevelType_Count = static_cast<size_t>(ELevelType::END);

	//===================
	// HitType
	//===================
	enum class EHitType : unsigned int
	{
		BLASTED = 0,
		FRONT,
		LEFT,
		RIGHT,
		ELCTRICKSHOCK,
		SPINBLOWOFF,
		SPINBLOWUP,
		ONEHITDOWNLONG,
		ONEHITDOWNSHORT,
		HITDOWNSHORT,
		HITDOWNLONG,
		WATERPRISON,
		EARTHQUAKE,
		UNIVERSALPULL,
		ELECTRICKSHOCK,
		PAIN_SPECIALSKILL,
		END
	};

	enum class EDir : unsigned int
	{
		BACKWARD = 0,
		RIGHT,
		LEFT,
		FRONT,
		END
	};

	enum class EHitDir : unsigned int
	{
		LEFT = 0,
		RIGHT,
		FRONT,
		END
	};

	enum class ESkillEventID : unsigned int
	{
		None = 0,
		AppearBegin,
		ActiveBegin,
		DisappearBegin,
	};

	enum class ESkillCommandType : unsigned int
	{
		EnablePart = 0,
		Rotate,
		PulseScale,
		TweenScale,
		PlayOneShot,
		END
	};

	enum class ERotateAxis : unsigned int
	{
		Yaw = 0,
		InvYaw,
		Roll,
		InvRoll,
		Pitch,
		InvPitch,
		None
	};


	enum class EMapObject_Type
	{
		StaticObject,
		END,
	};


	typedef struct tagColMeshHitInformation
	{
		class CColMesh* pColMesh = { nullptr };
		Vec3 vHitPosition = { 0.f, 0.f, 0.f };
		Vec3 vNormal = { 0.f, 1.f, 0.f };
		_float fDistance = D3D11_FLOAT32_MAX;
		_int iTriangleIndex = -1;
	}COLMESH_HITINFO;

	typedef struct tagWallSpaceInformation
	{
		Vec3 vCurrentUp = { 0.f, 1.f, 0.f };
		Vec3 vCurrentRight = { 1.f, 0.f, 0.f };
		Vec3 vCurrentLook = { 0.f, 0.f, 1.f };

		Vec3 vTargetUp = { 0.f, 1.f, 0.f };
		Vec3 vTargetRight = { 1.f, 0.f, 0.f };
		Vec3 vTargetLook = { 0.f, 0.f, 1.f };

		_float fAttachAlpha = { 0.f };
	}WALLSPACE_INFO;

	typedef struct tagRopeInformation
	{
		_int iTriangleIndex = -1;
		_float fMaxRopeLength = D3D11_FLOAT32_MAX;
		class CColMesh* pColMesh = { nullptr };
		Vec3 vHitPosition = { 0.f, 0.f, 0.f };
		WALLSPACE_INFO wallSpaceInfo = {};		
	}ROPE_INFO;

	typedef struct tagSkillCommand
	{
		ESkillCommandType eType = { ESkillCommandType::END };
		_uint iPartID = { 0 };

		_uint iEnable = 0;
		_uint iAxis = 0;
		_float fRotateSpeed = 1.f;
		_float fA = 0.f;
		_float fB = 0.f;
		_float fDuration = 0.f;
	}SKILL_CMD;

	typedef struct tagSkillPreset
	{
		std::unordered_map<_uint, std::vector<tagSkillCommand>> umapEventToCommands;
	}SKILL_PRESET;



#pragma region MapObject






	static ELevelType StringToClientleveltype(const _string& str)
	{
		if (::strcmp(str.c_str(), "STATIC") == 0)
			return ELevelType::STATIC;
		else if (::strcmp(str.c_str(), "LOADING") == 0)
			return ELevelType::LOADING;
		else if (::strcmp(str.c_str(), "LOGO") == 0)
			return ELevelType::LOGO;
		else if (::strcmp(str.c_str(), "TUTORIAL_VILLAGE") == 0)
			return ELevelType::TUTORIAL_VILLAGE;
		else if (::strcmp(str.c_str(), "TUTORIAL_BOSS") == 0)
			return ELevelType::TUTORIAL_BOSS;
		else if (::strcmp(str.c_str(), "SQUARE") == 0)
			return ELevelType::SQUARE;
		else if (::strcmp(str.c_str(), "TEST") == 0)
			return ELevelType::TEST;
		else
			return ELevelType::END;
	}



	/* Tool과 1ㄷ1 대응 */
	static _uint Get_IndexByMaterialSlotName(const wstring& wstrSlotName)
	{

		if (wstrSlotName == L"PM_Diffuse")
			return aiTextureType_DIFFUSE - 1;
		else if (wstrSlotName == L"PM_Normals")\
			return aiTextureType_NORMALS - 1;
		else if (wstrSlotName == L"PM_SpecularMasks")
			return aiTextureType_SPECULAR - 1;
		else if (wstrSlotName == L"PM_Emissive")
			return aiTextureType_EMISSIVE - 1;
		else if (wstrSlotName == L"PM_Ambient")
			return aiTextureType_AMBIENT - 1;


		return 0;
	}
	

	enum class EMapObject_DrawType
	{
		Collider,
		Default,
		Instance,
		END,
	};

	enum class EMapObjectShaderPass
	{
		StaticObject,
		LandScape,
		Bush,
		Grass,
		Moss,
		Tree,
		Vine,
		Rock,
		Water,
		RGBMapping,
		SHADOW_BAKE,
		DEBUG,
		END,
	};


#pragma	endregion

#pragma region UI

	enum class ERectTransform {
		LT = 0, CT, RT, LC, C, RC, LB, CB, RB, END
	};

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
		NONE		= 0u,
		HOVER_ENTER = 1u << 1,
		HOVERING	= 1u << 2,
		HOVER_EXIT	= 1u << 3,
		PRESS_ENTER = 1u << 4,
		PRESSING	= 1u << 5,
		PRESS_EXIT	= 1u << 6,
		INVOKED		= 1u << 7,
		END			= 1u << 8
	};

	inline EUIEvent EventFlagToEvent(EUIEvent_Flag eFlag)
	{
		switch (eFlag)
		{
		case EUIEvent_Flag::NONE:			return EUIEvent::NONE;
		case EUIEvent_Flag::HOVER_ENTER:	return EUIEvent::HOVER_ENTER;
		case EUIEvent_Flag::HOVERING:		return EUIEvent::HOVERING;
		case EUIEvent_Flag::HOVER_EXIT:		return EUIEvent::HOVER_EXIT;
		case EUIEvent_Flag::PRESS_ENTER:	return EUIEvent::PRESS_ENTER;
		case EUIEvent_Flag::PRESSING:		return EUIEvent::PRESSING;
		case EUIEvent_Flag::PRESS_EXIT:		return EUIEvent::PRESS_EXIT;
		case EUIEvent_Flag::INVOKED:		return EUIEvent::INVOKED;
		default:							return EUIEvent::NONE;
		}
	}

	inline EUIEvent_Flag EventToEventFlag(EUIEvent eEvent)
	{
		switch (eEvent)
		{
		case EUIEvent::NONE:			return EUIEvent_Flag::NONE;
		case EUIEvent::HOVER_ENTER:		return EUIEvent_Flag::HOVER_ENTER;
		case EUIEvent::HOVERING:		return EUIEvent_Flag::HOVERING;
		case EUIEvent::HOVER_EXIT:		return EUIEvent_Flag::HOVER_EXIT;
		case EUIEvent::PRESS_ENTER:		return EUIEvent_Flag::PRESS_ENTER;
		case EUIEvent::PRESSING:		return EUIEvent_Flag::PRESSING;
		case EUIEvent::PRESS_EXIT:		return EUIEvent_Flag::PRESS_EXIT;
		case EUIEvent::INVOKED:		return EUIEvent_Flag::INVOKED;
		default:							return EUIEvent_Flag::NONE;
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

		inline std::string UIEventToString(EUIEvent eType)
	{
		switch (eType)
		{
		case EUIEvent::NONE: return "NONE";
		case EUIEvent::HOVER_ENTER: return "HOVER_ENTER";
		case EUIEvent::HOVERING: return "HOVERING";
		case EUIEvent::HOVER_EXIT: return "HOVER_EXIT";
		case EUIEvent::PRESS_ENTER: return "PRESS_ENTER";
		case EUIEvent::PRESSING: return "PRESSING";
		case EUIEvent::PRESS_EXIT: return "PRESS_EXIT";
		case EUIEvent::INVOKED: return "INVOKED";
		default: return "";
		}
	}

	inline EUIEvent StringToUIEvent(const std::string& str)
	{
		if (str == "NONE") return EUIEvent::NONE;
		else if (str == "HOVER_ENTER") return EUIEvent::HOVER_ENTER;
		else if (str == "HOVERING") return EUIEvent::HOVERING;
		else if (str == "HOVER_EXIT") return EUIEvent::HOVER_EXIT;
		else if (str == "PRESS_ENTER") return EUIEvent::PRESS_ENTER;
		else if (str == "PRESSING") return EUIEvent::PRESSING;
		else if (str == "PRESS_EXIT") return EUIEvent::PRESS_EXIT;
		else if (str == "INVOKED") return EUIEvent::INVOKED;
		else return EUIEvent::END;
	}

	enum class ETriggerEventType { HOVER_ENTER, HOVER_EXIT, PRESS_ENTER, PRESS_EXIT, END };
	enum class EUIFlip
	{
		NONE = 0,
		FILP_X,
		FLIP_Y,
		FLIP_XY,
		END
	};
	enum class EUIPrefabType
	{
		NOT_PREFAB,
		MONSTER_NAMEPLATE,
		END
	};

	inline _wstring UIPrefabtypeToWstring(EUIPrefabType eType)
	{
		switch (eType)
		{
		case Client::EUIPrefabType::NOT_PREFAB:return L"NOT_PREFAB";
		case Client::EUIPrefabType::MONSTER_NAMEPLATE:return L"MONSTER_NAMEPLATE";
		case Client::EUIPrefabType::END:
		default:
			break;
		}
		return L"NOT_PREFAB";
	}

#pragma endregion

#pragma region SKILL
	inline _wstring SKILL_TYPE_ToWstring(const SKILL_TYPE eType)
	{
		switch (eType)
		{
		case SKILL_TYPE::DAMAGE:		return L"데미지";
		case SKILL_TYPE::BUFF:			return L"버프";
		case SKILL_TYPE::SUMMON:		return L"소환";
		case SKILL_TYPE::CURE:			return L"치유";
		case SKILL_TYPE::DEFENSE:		return L"디펜스";
		default:						return L"UNKNOWN";
		}
	}

#pragma endregion



	inline constexpr wchar_t g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer" };
	inline constexpr wchar_t g_wszColMeshLayer[]{ L"ColMesh_Layer" };
	inline constexpr wchar_t g_wszStaticObjectLayer[]{ L"StaticObject_Layer" };
	inline constexpr wchar_t g_wszInstanceModelLayer[]{ L"InstanceModel_Layer" };
	inline constexpr wchar_t g_wszBossLayer[]{ L"Boss_Layer" };
	inline constexpr wchar_t g_wszPlayerLayer[]{ L"Player_Layer" };
	inline constexpr wchar_t g_wszDynamicCameraLayer[]{ L"DynamicCamera_Layer" };
	inline constexpr wchar_t g_wszUILayer[]{ L"UI_Layer" };
	inline constexpr wchar_t g_wszSkillLayer[]{ L"Skill_Layer" };
	inline constexpr wchar_t g_wszEffectLayer[]{ L"Effect_Layer" };
}

using namespace Client;