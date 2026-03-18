#pragma once
#include "Engine_Define.h"
#include "EngineConsole.h"
#include <windows.h>

NS_BEGIN(Engine)
class CGameObject;
NS_END

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;
extern _bool		g_bStart;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1600;
	static const unsigned int g_iWinSizeY = 900;

	enum class ELevelType : unsigned int
	{
		STATIC = 0,
		LOADING,
		LOGO,				/* 현재 임시 Test Level용 추후 Logo Scene으로 바뀔예정  */
		TUTORIAL_VILLAGE,	/* 튜토리얼 처음 진입되는 Level */
		TUTORIAL_BOSS,		/* 튜토리얼 진입 이후 Boss전 가는 Level Type */
		SQUARE,				/* 광장 */
		TAVERN,				/* 술집 */
		KUANGKENG,			/* 갱도 */
		LIANHUO,			/* 지옥불 교도소장 보스 */


		TEST,				/* Test Scene은 맨 마지막 */
		END
	};

	inline constexpr size_t g_iLevelType_Count = static_cast<size_t>(ELevelType::END);
#ifdef _DEBUG
	static const char* LevelTypeToString(int lv)
	{
		static const char* kNames[] =
		{
			"STATIC",
			"LOADING",
			"LOGO",
			"TUTORIAL_VILLAGE",
			"TUTORIAL_BOSS",
			"TAVERN",
			"SQUARE",
			"KUANGKENG",
			"LIANHUO",
			"TEST",
		};

		if (lv < 0 || lv >= (_int)ELevelType::END)
			return "Invalid";

		return kNames[lv];
	}
#endif

	enum class EGroggyState : unsigned int
	{
		None = 0,
		Middle = 1 << 0,
		Final = 1 << 1
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


	enum class EMonster_Type
	{
		/* Mosnter */
		Dog,
		Shooter,
		Boomer,

		/* Boss */
		Xibi,
		END
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

	struct CB_EnvData
	{
		Vec3        vWindDirection{ 1.f,0.f, 1.f }; //바람이 부는 방향
		_float      fWindPower{ 1.f }; //바람이 부는 새기
	};

	struct CB_WaterData
	{
		_uint  g_WaterTexBindingFlags{ 0 };								// Texture가 바인딩되었는지 안되어있는지 Flag값
		float  g_fWaterDT{ 0.f };										// 움직이는 UV좌표를 위한 DT값
		Vec2   g_vWaterSpeed1{ 1.f,1.f };								// 물 일렁임관련? Speed 값
		Vec2   g_vWaterSpeed2{ 1.f,1.f };								// 물 일렁임관련? Speed 값
		Vec2   g_vWaterDistortionSpeed{ 1.f, 1.f };                     // 

		Vec2    g_vWaterUVPower{ 1.f, 1.f };
		Vec2    g_vWaterDistortionUVPower{ 1.f, 1.f };                  // Noise Texture UV Tiling Power
		float   g_fDistortionPower{ 1.f };								// Noise가 섞이는 비율? 세기

		float   g_fSparklePower;                                        // 4 Byte (윤슬 눈뽕 강도!)
		Vec2    g_vSparkleUVPower;                                      // 8 Byte (윤슬 자글자글함 크기 조절!)
	};

	struct CB_GrassData
	{
		_float	g_fGrassDT			{0.f};
		_float	g_fGrassMaxHeight	{1.f};		//Model Min Max 중 Max의 Y값		
		_float  g_fGrassSwaySpeed	{1.f};		//이 잔디가 Sway = 흔들리는 Speed		Tool에서 지정
		_float  g_fGrassWaveSize	{1.f};		//이 잔디가 Power = 흔들리는 힘		Tool에서 지정
	};

	struct CB_PlantData
	{
		_float	g_DiffuseColorPower{1.f};
		Vec3	g_Dummy{};
	};


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
		else if (::strcmp(str.c_str(), "TAVERN") == 0)
			return ELevelType::TAVERN;
		else if (::strcmp(str.c_str(), "KUANGKENG") == 0)
			return ELevelType::KUANGKENG;
		else if (::strcmp(str.c_str(), "TEST") == 0)
			return ELevelType::TEST;
		else if (::strcmp(str.c_str(), "LIANHUO") == 0)
			return ELevelType::LIANHUO;
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
		SkyBox,
		END,
	};


#pragma	endregion

#pragma region UI

	enum class ERectTransform {
		LT = 0, CT, RT, LC, C, RC, LB, CB, RB, END
	};

	enum class EUIInteract : uint32_t {
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

	enum EUIInteract_Flag : uint32_t {
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

	inline EUIInteract UIInteractFlagToUIInteract(EUIInteract_Flag eFlag)
	{
		switch (eFlag)
		{
		case EUIInteract_Flag::NONE:			return EUIInteract::NONE;
		case EUIInteract_Flag::HOVER_ENTER:	return EUIInteract::HOVER_ENTER;
		case EUIInteract_Flag::HOVERING:		return EUIInteract::HOVERING;
		case EUIInteract_Flag::HOVER_EXIT:		return EUIInteract::HOVER_EXIT;
		case EUIInteract_Flag::PRESS_ENTER:	return EUIInteract::PRESS_ENTER;
		case EUIInteract_Flag::PRESSING:		return EUIInteract::PRESSING;
		case EUIInteract_Flag::PRESS_EXIT:		return EUIInteract::PRESS_EXIT;
		case EUIInteract_Flag::INVOKED:		return EUIInteract::INVOKED;
		default:							return EUIInteract::NONE;
		}
	}

	inline EUIInteract_Flag UIInteractToUIInteractFlag(EUIInteract eEvent)
	{
		switch (eEvent)
		{
		case EUIInteract::NONE:			return EUIInteract_Flag::NONE;
		case EUIInteract::HOVER_ENTER:		return EUIInteract_Flag::HOVER_ENTER;
		case EUIInteract::HOVERING:		return EUIInteract_Flag::HOVERING;
		case EUIInteract::HOVER_EXIT:		return EUIInteract_Flag::HOVER_EXIT;
		case EUIInteract::PRESS_ENTER:		return EUIInteract_Flag::PRESS_ENTER;
		case EUIInteract::PRESSING:		return EUIInteract_Flag::PRESSING;
		case EUIInteract::PRESS_EXIT:		return EUIInteract_Flag::PRESS_EXIT;
		case EUIInteract::INVOKED:			return EUIInteract_Flag::INVOKED;
		default:						return EUIInteract_Flag::NONE;
		}
	}

	NLOHMANN_JSON_SERIALIZE_ENUM(EUIInteract,
		{
			{EUIInteract::NONE,			"NONE"},
			{EUIInteract::HOVER_ENTER,	"HOVER_ENTER"},
			{EUIInteract::HOVERING,		"HOVERING"},
			{EUIInteract::HOVER_EXIT,	"HOVER_EXIT"},
			{EUIInteract::PRESS_ENTER,	"PRESS_ENTER"},
			{EUIInteract::PRESSING,		"PRESSING"},
			{EUIInteract::PRESS_EXIT,	"PRESS_EXIT"},
			{EUIInteract::INVOKED,		"INVOKED"},
		})

		inline std::string UIInteractToString(EUIInteract eType)
	{
		switch (eType)
		{
		case EUIInteract::NONE:			return "NONE";
		case EUIInteract::HOVER_ENTER:	return "HOVER_ENTER";
		case EUIInteract::HOVERING:		return "HOVERING";
		case EUIInteract::HOVER_EXIT:	return "HOVER_EXIT";
		case EUIInteract::PRESS_ENTER:	return "PRESS_ENTER";
		case EUIInteract::PRESSING:		return "PRESSING";
		case EUIInteract::PRESS_EXIT:	return "PRESS_EXIT";
		case EUIInteract::INVOKED:		return "INVOKED";
		default: return "";
		}
	}

	inline EUIInteract StringToUIInteract(const std::string& str)
	{
		if (str == "NONE")				return EUIInteract::NONE;
		else if (str == "HOVER_ENTER")	return EUIInteract::HOVER_ENTER;
		else if (str == "HOVERING")		return EUIInteract::HOVERING;
		else if (str == "HOVER_EXIT")	return EUIInteract::HOVER_EXIT;
		else if (str == "PRESS_ENTER")	return EUIInteract::PRESS_ENTER;
		else if (str == "PRESSING")		return EUIInteract::PRESSING;
		else if (str == "PRESS_EXIT")	return EUIInteract::PRESS_EXIT;
		else if (str == "INVOKED")		return EUIInteract::INVOKED;
		else return EUIInteract::END;
	}

	enum class ETriggerEventType { 
		HOVER_ENTER, 
		HOVER_EXIT, 
		PRESS_ENTER, 
		PRESS_EXIT, 
		END 
	};

	enum class EUIFlip {
		NONE = 0,
		FILP_X,
		FLIP_Y,
		FLIP_XY,
		END
	};

	enum class EUITextureSlot : uint32_t { 
		DEFAULT, 
		NOISE,
		ALPHA_MASK, 
		GLOW, 
		END 
	};


	// 지금 어떤 퀘스트인지
	enum class EUITutorialQuestTypeID
	{
		// 연옥도 탈출
		QUEST1_1,		// 계속 전진해서 산 아래의 마을로 가기
		QUEST1_2,		// 필토이드를 피해 집으로 돌아가 무기 찾기
		QUEST1_3,		// 공격해 오는 필토이드 퇴치(0/10)
		QUEST1_4,		// 부두에서 배 찾기
		QUEST1_5,		// 해안을 따라 해변 조사

		//세월에 묻힌 땅
		QUEST2_1,		// 총기를 테스트하고 필토이드 무리 처치(0/10)
		QUEST2_2,		// 산 위의 유적으로 가기

		// 모레의 메아리
		QUEST3_1,		// 병사를 처치하고 벗어나기(0/40)
		QUEST3_2,		// 시빌라의 공격 막아내기

		END
	};

	enum class EUITutorialPannelTypeID
	{
		TUTORIAL_PANNEL_1,	// 쉴드 / HP
		TUTORIAL_PANNEL_2,	// 원거리 무기
		TUTORIAL_PANNEL_3,	// 캐릭터 매커니즘: 아스크의 징벌
		TUTORIAL_PANNEL_4,	// 캐릭터 종결 스킬: 어둠의 불꽃
		END
	};


	enum class EUITutorialPopUpTypeID
	{
		TUTORIAL_POPUP_1,	// [Space] 를 눌러 점프
		TUTORIAL_POPUP_2,	// [LCtrl] 를 눌러 슬라이드로 구멍 지나가기
		TUTORIAL_POPUP_3,	// [LCtrl] 를 길게 눌러 앉기/슬라이드 상태 진입
		TUTORIAL_POPUP_3_1,	// [Space] 를 다시 눌러 스파이럴 점프로 절벽 넘기
		TUTORIAL_POPUP_4,	// 마우스를 움직여 시야를 올린 후 대각선으로 뛰어오르기
		TUTORIAL_POPUP_4_1,	// [LCtrl] 를 길게 누른 상태에서 [Space] 를 눌러 스파이럴 점프로 절벽을 넘거나 원거리 이동할 수 있습니다.
		TUTORIAL_POPUP_5,	// 벽에 다가가서 [Space] 를 연속으로 눌러 벽 오르기
		TUTORIAL_POPUP_6,	// [V] 버튼을 눌러 가이드 포인트를 확인할 수 있습니다.
		TUTORIAL_POPUP_7,	// [Mouse L] 을 눌러 근접 공격
		TUTORIAL_POPUP_8,	// [LShift] 버튼을 눌러 적의 공격 회피
		TUTORIAL_POPUP_9,	// [Mouse R] 를 눌러 원거리 공격
		TUTORIAL_POPUP_10,	// [R] 버튼을 눌러 탄환 장전
		TUTORIAL_POPUP_11,	// [E] 버튼을 눌러 스킬 시전
		TUTORIAL_POPUP_12,	// 보스를 조준하고 [Mouse Wheel] 마우스 휠 버튼을 눌러 시점 고정, 다시 눌러서 고정 해제
		TUTORIAL_POPUP_13,	// [Q] 버튼을 눌러 종결 스킬 시전
		END,
	};

	// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
	static const char* g_szTutorialUIEvent[(int)EUITutorialPopUpTypeID::END] = {
		"TUTORIAL_POPUP_1",
		"TUTORIAL_POPUP_2",
		"TUTORIAL_POPUP_3",
		"TUTORIAL_POPUP_3_1",
		"TUTORIAL_POPUP_4",
		"TUTORIAL_POPUP_4_1",
		"TUTORIAL_POPUP_5",
		"TUTORIAL_POPUP_6",
		"TUTORIAL_POPUP_7",
		"TUTORIAL_POPUP_8",
		"TUTORIAL_POPUP_9",
		"TUTORIAL_POPUP_10",
		"TUTORIAL_POPUP_11",
		"TUTORIAL_POPUP_12",
		"TUTORIAL_POPUP_13"
	};

	inline std::string UITutorialPopUpTypeID_ToString(EUITutorialPopUpTypeID eType)
	{
		// 인덱스 초과 방지 안전장치
		if (eType >= EUITutorialPopUpTypeID::TUTORIAL_POPUP_1 && eType < EUITutorialPopUpTypeID::END)
			return g_szTutorialUIEvent[(int)eType];

		return "Unknown";
	}





	enum class EUIPrefabType {
		NOT_PREFAB,
		MONSTER_NAMEPLATE,
		DAMAGE_FONTS_COMMON,
		DAMAGE_FONTS_CRITICAL,
		DAMAGE_FONTS_HIT,
		BOSS_NAMEPLATE,
		MINIMAP_MONSTER_ICON,
		TUTORIAL_PANNEL,

		TUTORIAL_POPUP_1,
		TUTORIAL_POPUP_2,
		TUTORIAL_POPUP_3,
		TUTORIAL_POPUP_3_1,
		TUTORIAL_POPUP_4,
		TUTORIAL_POPUP_4_1,
		TUTORIAL_POPUP_5,
		TUTORIAL_POPUP_6,
		TUTORIAL_POPUP_7,
		TUTORIAL_POPUP_8,
		TUTORIAL_POPUP_9,
		TUTORIAL_POPUP_10,
		TUTORIAL_POPUP_11,
		TUTORIAL_POPUP_12,
		TUTORIAL_POPUP_13,

		TUTORIAL_PANNEL_1,
		TUTORIAL_PANNEL_2,
		TUTORIAL_PANNEL_3,
		TUTORIAL_PANNEL_4,

		END
	};


	inline EUITutorialPopUpTypeID UITutorialPopUpTypeID_ToEnum(const std::string& strType)
	{
		for (int i = 0; i < (int)EUITutorialPopUpTypeID::END; ++i)
		{
			if (strType == g_szTutorialUIEvent[i])
				return (EUITutorialPopUpTypeID)i;
		}
		return EUITutorialPopUpTypeID::END;
	}



	inline _wstring UIPrefabtypeToWstring(EUIPrefabType eType)
	{
		switch (eType)
		{
		case Client::EUIPrefabType::NOT_PREFAB:				return L"NOT_PREFAB";
		case Client::EUIPrefabType::MONSTER_NAMEPLATE:		return L"MONSTER_NAMEPLATE";
		case Client::EUIPrefabType::DAMAGE_FONTS_COMMON:	return L"DAMAGE_FONTS_COMMON";
		case Client::EUIPrefabType::DAMAGE_FONTS_CRITICAL:	return L"DAMAGE_FONTS_CRITICAL";
		case Client::EUIPrefabType::DAMAGE_FONTS_HIT:		return L"DAMAGE_FONTS_HIT";
		case Client::EUIPrefabType::MINIMAP_MONSTER_ICON:	return L"DAMAGE_FONTS_HIT";
		case Client::EUIPrefabType::TUTORIAL_PANNEL:		return L"TUTORIAL_PANNEL";

		case Client::EUIPrefabType::TUTORIAL_POPUP_1:		return L"TUTORIAL_POPUP_1";
		case Client::EUIPrefabType::TUTORIAL_POPUP_2:		return L"TUTORIAL_POPUP_2";
		case Client::EUIPrefabType::TUTORIAL_POPUP_3:		return L"TUTORIAL_POPUP_3";
		case Client::EUIPrefabType::TUTORIAL_POPUP_3_1:		return L"TUTORIAL_POPUP_3_1";
		case Client::EUIPrefabType::TUTORIAL_POPUP_4:		return L"TUTORIAL_POPUP_4";
		case Client::EUIPrefabType::TUTORIAL_POPUP_4_1:		return L"TUTORIAL_POPUP_4_1";
		case Client::EUIPrefabType::TUTORIAL_POPUP_5:		return L"TUTORIAL_POPUP_5";
		case Client::EUIPrefabType::TUTORIAL_POPUP_6:		return L"TUTORIAL_POPUP_6";
		case Client::EUIPrefabType::TUTORIAL_POPUP_7:		return L"TUTORIAL_POPUP_7";
		case Client::EUIPrefabType::TUTORIAL_POPUP_8:		return L"TUTORIAL_POPUP_8";
		case Client::EUIPrefabType::TUTORIAL_POPUP_9:		return L"TUTORIAL_POPUP_9";
		case Client::EUIPrefabType::TUTORIAL_POPUP_10:		return L"TUTORIAL_POPUP_10";
		case Client::EUIPrefabType::TUTORIAL_POPUP_11:		return L"TUTORIAL_POPUP_11";
		case Client::EUIPrefabType::TUTORIAL_POPUP_12:		return L"TUTORIAL_POPUP_12";
		case Client::EUIPrefabType::TUTORIAL_POPUP_13:		return L"TUTORIAL_POPUP_13";

		case Client::EUIPrefabType::TUTORIAL_PANNEL_1:		return L"TUTORIAL_PANNEL_1";
		case Client::EUIPrefabType::TUTORIAL_PANNEL_2:		return L"TUTORIAL_PANNEL_2";
		case Client::EUIPrefabType::TUTORIAL_PANNEL_3:		return L"TUTORIAL_PANNEL_3";
		case Client::EUIPrefabType::TUTORIAL_PANNEL_4:		return L"TUTORIAL_PANNEL_4";

		case Client::EUIPrefabType::END:
		default:
			break;
		}
		return L"NOT_PREFAB";
	}




	typedef struct tagUINamePlatePrefabData
	{
		CGameObject* pTarget = { nullptr };
		Vec3 vOffset = {};
	} UI_NAMEPLATE_PREFAB_DATA;

	typedef struct tagUIDamageFontPrefabData
	{
		CGameObject* pTarget = { nullptr };
		Vec4 vFontColor = {};
		Vec3 vHitPos = {};
		_uint iDamage = {};
		Vec3 vRandOffset = {};
	} UI_DAMAGEFONT_PREFAB_DATA;

	typedef struct tagUIBossNamePlatePrefabData
	{
		CGameObject* pTarget = { nullptr };
	} UI_BOSS_NAMEPLATE_PREFAB_DATA;

	typedef struct tagUIMinimapMonsterIconPrefabData
	{
		CGameObject* pTarget = { nullptr };
	} UI_MINIMAP_MONSTER_ICON_PREFAB_DATA;

	typedef struct tagUITutorialPannelPrefabData
	{
		EUITutorialPannelTypeID eTutorialTypeID = {};

	} UI_TUTORIAL_PANNEL_PREFAB_DATA;

	typedef struct tagUITutorialPopUpPrefabData
	{
		EUITutorialPopUpTypeID eTutorialTypeID = { EUITutorialPopUpTypeID::END };

	} UI_TUTORIAL_POPUP_PREFAB_DATA;


	typedef std::variant<
		UI_NAMEPLATE_PREFAB_DATA,
		UI_DAMAGEFONT_PREFAB_DATA,
		UI_BOSS_NAMEPLATE_PREFAB_DATA,
		UI_MINIMAP_MONSTER_ICON_PREFAB_DATA,
		UI_TUTORIAL_PANNEL_PREFAB_DATA,
		UI_TUTORIAL_POPUP_PREFAB_DATA
	> UI_PREFAB_VARIANT;

	typedef struct tagUIPrefabData
	{
		UI_PREFAB_VARIANT Data;
		class CCanvas* pCanvas = {nullptr};
	} UI_PREFAB_DATA;



	enum class ECombotype {
		C, 
		B, 
		A, 
		S, 
		END
	};




#pragma endregion



#pragma region Player

	// COLLIDED_DESC -> EXTRA_ATTACK_DESC ->iDamageFlag 에 대한 정보
	enum class EPlayerAttackFlag : Flags {
		NONE = 0,

		// player 종류
		MOON = 0x000001,
		BERO = 0x000002,
		PIGGY = 0x000004,

		NORMAL = 0x000008,
		CRITICAL = 0x000010,
		SKILLE = 0x000020,
		SKILLQ = 0x000040,
		GUN = 0x000080
	};

#pragma	endregion


#pragma region BroadCast Enum Mapping

	enum class EGlobal_Broadcast_Type
	{
		NONE,
		TUTORIAL_BOSS_CONTATCT,
		TUTORIAL_BOSS_CONTATCT_END,

		CINEMATIC_START,
		CINEMATIC_END,
		XIBILA_BOSS_ACTION_ON,
		XIBILA_BOSS_ACTION_OFF,

		XIBILA_BOSS_UI_ON,
		XIBILA_BOSS_UI_OFF,


		/* 보스 연출위해 임시방편 추후에 사라질 예정 */
		XIBI_CHANGE_STATE_BOSS_DIRECTION,
		XIBI_CHANGE_STATE_BOSS_IDLE,

		END,
	};

	// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
	static const char* g_szGlobalBroadCastType[(int)EGlobal_Broadcast_Type::END] = {
		"NONE",
		"TUTORIAL_BOSS_CONTATCT",
		"TUTORIAL_BOSS_CONTATCT_END",
		"CINEMATIC_START",
		"CINEMATIC_END",
		"XIBILA_BOSS_ACTION_ON",
		"XIBILA_BOSS_ACTION_OFF",
		"XIBILA_BOSS_UI_ON",
		"XIBILA_BOSS_UI_OFF",

		"XIBI_CHANGE_STATE_BOSS_DIRECTION",
		"XIBI_CHANGE_STATE_BOSS_IDLE",

	};
	inline string Global_Broadcast_Type_ToString(EGlobal_Broadcast_Type eType)
	{
		// 인덱스 초과 방지 안전장치
		if (eType >= EGlobal_Broadcast_Type::NONE && eType < EGlobal_Broadcast_Type::END)
			return g_szGlobalBroadCastType[(int)eType];

		return "Unknown";
	}
	inline EGlobal_Broadcast_Type Global_Broadcast_Type_ToEnum(const string& strType)
	{
		for (int i = 0; i < (int)EGlobal_Broadcast_Type::END; ++i)
		{
			if (strType == g_szGlobalBroadCastType[i])
				return (EGlobal_Broadcast_Type)i;
		}
		return EGlobal_Broadcast_Type::NONE;
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


#pragma region Tag 모음

	inline constexpr _tchar g_wszCameraCinematicData_JsonPath[]{ L"../../Resources/Data/CameraCinematicData/CameraCinematicData.json" };
	inline constexpr _tchar g_wszCameraCinematicSequnceEventManifest_JsonPath[]{ L"../../Resources/Data/CameraCinematicData/CCS_EventManifest.json" };

#pragma region State Tag

	/* Monster State Tag */
	inline constexpr wchar_t g_wszMonster_Dog_State_Tag[]{ L"Monster_Dog" };
	inline constexpr wchar_t g_wszMonster_Boomer_State_Tag[]{ L"Monster_Boomer" };
	inline constexpr wchar_t g_wszBoss_Xibi_State_Tag[]{ L"Boss_Xibi" };

	inline constexpr wchar_t g_wszNPC_Pan_State_Tag[]{ L"NPC_Pan" };

#pragma endregion 

#pragma region Model Protototype Tag

	/* Monster Model Tag */
	inline constexpr wchar_t g_wszMonster_Dog_Model_Prototype_Tag[]				{ L"Prototype_Component_Model_Monster_Dog"};
	inline constexpr wchar_t g_wszMonster_Boomer_Model_Prototype_Tag[]			{ L"Prototype_Component_Model_Monster_Boomer" };
	inline constexpr wchar_t g_wszBoss_Xibi_Model_Prototype_Tag[]				{ L"Prototype_Component_Model_Xibi" };
	
	inline constexpr wchar_t g_wszNPC_Pan_Model_Prototype_Tag[]				{ L"Prototype_Component_Model_NPC_Pan" };

#pragma endregion

#pragma region Prototype Tag


#pragma region Monster Attack Overlap

	/* Monster Attack OverLap */
	inline constexpr wchar_t g_wszMonster_Dog_AttackOverlap_Prototype_Tag[]		{ L"Prototype_Component_AttackOverlap_Monster_Dog" };
	inline constexpr wchar_t g_wszMonster_Boomer_AttackOverlap_Prototype_Tag[]	{ L"Prototype_Component_AttackOverlap_Monster_Boomer" };
	inline constexpr wchar_t g_wszBoss_Xibi_AttackOverlap_Prototype_Tag[]		{ L"Prototype_Component_AttackOverlap_Xibi" };
#pragma endregion


#pragma region Bounding 관련

	/* Bounding Box */
	inline constexpr wchar_t g_wszCollider_AABB_Prototype_Tag[]{ L"Prototype_Component_Collider_AABB" };
	inline constexpr wchar_t g_wszCollider_Sphere_PrototypeTag[]{ L"Prototype_Component_Collider_Sphere" };
	inline constexpr wchar_t g_wszCollider_OBB_Prototype_Tag[]{ L"Prototype_Component_Collider_OBB" };

#pragma endregion

#pragma region SkillObject 관련
	inline constexpr wchar_t g_wszColliderModule_Prototype_Tag[]				{ L"Prototype_GameObject_ColliderModule" };
	/* Xibi */
	inline constexpr wchar_t g_wszXibiProjectile_Prototype_Tag[]				{ L"Prototype_GameObject_Xibi_CircleProjectile" };
	inline constexpr wchar_t g_wszXibiLoopThunder_Prototype_Tag[]				{ L"Prototype_GameObject_Xibi_LoopThunder" };
	inline constexpr wchar_t g_wszXibiOneshotThunder_Prototype_Tag[]			{ L"Prototype_GameObject_Xibi_OneshotThunder" };
	inline constexpr wchar_t g_wszPool_XibiLoopThunder[]						{ L"Pool_Xibi_LoopThunder" };
	inline constexpr wchar_t g_wszPool_XibiCircleProjectile[]					{ L"Pool_Xibi_CircleProjectile" };
	inline constexpr wchar_t g_wszPool_XibiOneshotThunder[]						{ L"Pool_Xibi_OneshotThunder" };

	inline constexpr wchar_t g_wszMoonSkillE__Prototype_Tag[]								{ L"Prototype_GameObject_Moon_SkillE" };
	inline constexpr wchar_t g_wszPool_MoonSkillE[]											{ L"Pool_Moon_SkillE" };
	//inline constexpr wchar_t g_wszMoonSkillQSheild_Prototype_Tag[]							{ L"Prototype_GameObject_Moon_SkillQSheild" };
	//inline constexpr wchar_t g_wszPool_MoonSkillQSheild[]									{ L"Pool_Moon_SkillQSheild" };
	inline constexpr wchar_t g_wszMoonSkillQAttack_Prototype_Tag[]							{ L"Prototype_GameObject_Moon_SkillQAttack" };	// obj 프로토타입 태그 값
	inline constexpr wchar_t g_wszPool_MoonSkillQAttack[]									{ L"Pool_Moon_SkillQAttack" };					// pool에서 obj 꺼내올 태그 값

#pragma endregion

#pragma region SkillObjectSpawner 관련
	inline constexpr wchar_t g_wszSpawner_XibiOneshotSingleThunder[]			{ L"Prototype_Spawner_XibiOneshotSingleThunder" };
	inline constexpr wchar_t g_wszSpawner_XibiOneshotRandomThunder[]			{ L"Prototype_Spawner_XibiOneshotRandomThunder" };
	inline constexpr wchar_t g_wszSpawner_Xibi360CircleProjectile[]				{ L"Prototype_Spawner_Xibi360CircleProjectile" };
	inline constexpr wchar_t g_wszSpawner_Xibi360ThunderProjectile[]			{ L"Prototype_Spawner_Xibi360ThunderProjectile" };
	inline constexpr wchar_t g_wszSpawner_Xibi3wayLoopThunder[]					{ L"Prototype_Spawner_Xibi3wayLoopThunder" };
	inline constexpr wchar_t g_wszSpawner_XibiGate[]							{ L"Prototype_Spawner_XibiGate" };

	inline constexpr wchar_t g_wszSpawner_MoonSkillE[]							{ L"Prototype_Spawner_PlayerMoon_SkillE" };
	inline constexpr wchar_t g_wszSpawner_MoonSkillQ_Sheild[]							{ L"Prototype_Spawner_PlayerMoon_SkillQ_Sheild" };
	inline constexpr wchar_t g_wszSpawner_MoonSkillQ_Attack[]							{ L"Prototype_Spawner_PlayerMoon_SkillQ_Attack" };
#pragma endregion

#pragma region MapObject 관련
	inline constexpr wchar_t g_wszStaticObject_Prototype_Tag[]					{ L"Prototype_GameObject_StaticObject" };
	inline constexpr wchar_t g_wszLandScape_Prototype_Tag[]						{ L"Prototype_GameObject_LandScape" };
	inline constexpr wchar_t g_wszBush_Prototype_Tag[]							{ L"Prototype_GameObject_Bush" };
	inline constexpr wchar_t g_wszGrass_Prototype_Tag[]							{ L"Prototype_GameObject_Grass" };
	inline constexpr wchar_t g_wszMoss_Prototype_Tag[]							{ L"Prototype_GameObject_Moss" };
	inline constexpr wchar_t g_wszTree_Prototype_Tag[]							{ L"Prototype_GameObject_Tree" };
	inline constexpr wchar_t g_wszVine_Prototype_Tag[]							{ L"Prototype_GameObject_Vine" };
	inline constexpr wchar_t g_wszRock_Prototype_Tag[]							{ L"Prototype_GameObject_Rock" };
	inline constexpr wchar_t g_wszWater_Prototype_Tag[]							{ L"Prototype_GameObject_Water" };

	inline constexpr wchar_t g_wszEnvObject_Prototype_Tag[]						{ L"Prototype_GameObject_EnvObject" };



	inline constexpr wchar_t g_wszInvisibleWall_Prototype_Tag[]					{ L"Prototype_GameObject_InvisibleWall" };
#pragma endregion

#pragma region Trigger Box 관련
	inline constexpr wchar_t g_wszTriggerBox_ChangeLevel_Prototype_Tag[]{ L"Prototype_GameObject_TriggerBox_ChangeLevel" };
	inline constexpr wchar_t g_wszTriggerBox_MonsterSapwner_Prototype_Tag[]{ L"Prototype_GameObject_TriggerBox_MonsterSpawner" };
	inline constexpr wchar_t g_wszTriggerBox_PhysicsColliderBox_PrototypeTag[]{ L"Prototype_Component_Physics_Collider_TriggerBox" };
	inline constexpr wchar_t g_wszTriggerBox_GlobalEvent_BroadCaster_PrototypeTag[]{ L"Prototype_GameObject_TriggerBox_GlobalEvent_BroadCaster" };
	inline constexpr wchar_t g_wszTriggerBox_TutorialUIEvent_PrototypeTag[]{ L"Prototype_GameObject_TriggerBox_TutorialUIEvent" };
#pragma endregion

#pragma region Monster 관련	
	/* Monster Prototype Name 모음 */
	inline constexpr wchar_t g_wszMonster_Dog_Prototype_Tag[]{ L"Prototype_GameObject_Monster_Dog" };
	inline constexpr wchar_t g_wszMonster_Boomer_Prototype_Tag[]{ L"Prototype_GameObject_Monster_Boomer" };
	inline constexpr wchar_t g_wszMonster_Shooter_Prototype_Tag[]{ L"Prototype_GameObject_Monster_Shooter" };
	inline constexpr wchar_t g_wszBoss_Xibi_Prototype_Tag[]{ L"Prototype_GameObject_Boss_Xibi" };

	/* Monster Body Prototype Name 모음 */
	inline constexpr wchar_t g_wszMonster_Dog_Body_Prototype_Tag[]{ L"Prototype_GameObject_Monster_Dog_Body" };
	inline constexpr wchar_t g_wszMonster_Boomer_Body_Prototype_Tag[]{ L"Prototype_GameObject_Monster_Boomer_Body" };
	inline constexpr wchar_t g_wszBoss_Shooter_Body_Prototype_Tag[]{ L"Prototype_GameObject_Monster_Shooter_Body" };
	inline constexpr wchar_t g_wszBoss_Xibi_Body_Prototype_Tag[]{ L"Prototype_GameObject_Boss_Xibi_Body" };

	inline constexpr wchar_t g_wszPool_Monster_Dog[]{ L"Pool_Monster_Dog" };
	inline constexpr wchar_t g_wszPool_Monster_Boomer[]{ L"Pool_Monster_Boomer" };
	inline constexpr wchar_t g_wszPool_Monster_Shooter[]{ L"Pool_Monster_Shooter" };
#pragma endregion

#pragma region Npc 관련
	inline constexpr wchar_t g_wszNPC_Pan_Prototype_Tag[]{ L"Prototype_GameObject_NPC_Pan" };

	inline constexpr wchar_t g_wszNPC_Pan_Body_Prototype_Tag[]{ L"Prototype_GameObject_NPC_Pan_Body" };
#pragma endregion

#pragma region Part Objects

	inline constexpr wchar_t g_wszPartObj_Effect_Prototype_Tag[]{ L"Prototype_GameObject_Part_Effect" }; // static
	inline constexpr wchar_t g_wszPartObj_Socket_Prototype_Tag[]{ L"Prototype_GameObject_Part_Socket" }; // static
	inline constexpr wchar_t g_wszPartObj_Bone_Prototype_Tag[]{ L"Prototype_GameObject_Part_Socket" }; // static

#pragma endregion

#pragma region 기타
	inline constexpr wchar_t g_wszBattleField_Prototype_Tag[]					{ L"Prototype_GameObject_BattleField"};
	inline constexpr wchar_t g_wszSkyBox_Prototype_Tag[]						{ L"Prototype_GameObject_SkyBox" };
	inline constexpr wchar_t g_wszPointLight_Prototype_Tag[]					{ L"Prototype_GameObject_PointLight"};
#pragma endregion

#pragma endregion

#pragma region Layer Tag
	inline constexpr wchar_t g_wszSkillObjectLayer[]							{ L"SkillObject_Layer" };
	inline constexpr wchar_t g_wszPlayerLayer[]									{ L"Player_Layer" };
	inline constexpr wchar_t g_wszMonstereLayer[]								{ L"Monster_Layer" };
	inline constexpr wchar_t g_wszBossLayer[]									{ L"Boss_Layer" };
	inline constexpr wchar_t g_wszColMeshLayer[]								{ L"ColMesh_Layer" };
	inline constexpr wchar_t g_wszStaticObjectLayer[]							{ L"StaticObject_Layer" };
	inline constexpr wchar_t g_wszInstanceModelLayer[]							{ L"InstanceModel_Layer" };
	inline constexpr wchar_t g_wszDynamicCameraLayer[]							{ L"DynamicCamera_Layer" };
	inline constexpr wchar_t g_wszUILayer[]										{ L"UI_Layer" };
	inline constexpr wchar_t g_wszSkillLayer[]									{ L"Skill_Layer" };
	inline constexpr wchar_t g_wszEffectLayer[]									{ L"Effect_Layer" };
	inline constexpr wchar_t g_wszTriggerBoxLayer[]								{ L"TriggerBox_Layer" };
	inline constexpr wchar_t g_wszBattleFieldLayer[]							{ L"BattleField_Layer" };
	inline constexpr wchar_t g_wszInvisibleWallLayer[]							{ L"InvisibleWall_Layer" };
	inline constexpr wchar_t g_wszSkyBoxLayer[]									{ L"SkyBox_Layer" };
	inline constexpr wchar_t g_wszPointLightLayer[]								{ L"PointLight_Layer" };
	inline constexpr wchar_t g_wszNPCeLayer[]									{ L"NPC_Layer" };
#pragma endregion

#pragma region Dialogue
	//typedef struct EDialogueChoice
	//{
	//	enum Enum
	//	{
	//		NONE,
	//		CONFIRM,
	//		CANCEL,
	//		BACK,
	//		END
	//	};
	//}DIALOGUE_CHOICE;

	typedef struct tagDialogueChoice
	{
		wstring wstrText = {};
		_int iTransitionId = { -1 };
	}DIALOGUE_CHOICE;

	typedef struct tagDialogueNode
	{
		_int iNodeId = { -1 };
		_int iPrevId = { -1 };
		_int iNextId = { -1 };

		//OBJECT_ENUM_TAG::Enum eSpeakerTag = OBJECT_ENUM_TAG::NPC_PAN;
		wstring wstrSpeakerName = {};
		wstring wstrContentText = {};

		vector<_int> vecTriggerEvent;
		vector<DIALOGUE_CHOICE> vecChoices;

		tagDialogueNode& AddTrigger(_int eventId)
		{
			vecTriggerEvent.push_back(eventId);
			return *this;
		}

		tagDialogueNode& AddChoice(const wstring& text, _int transitionId)
		{
			vecChoices.push_back({ text, transitionId });
			return *this;
		}

		vector<DIALOGUE_CHOICE>* GetChoices()
		{
			return &vecChoices;
		}

	}DIALOGUE_NODE;
#pragma endregion



}

#pragma endregion


using namespace Client;