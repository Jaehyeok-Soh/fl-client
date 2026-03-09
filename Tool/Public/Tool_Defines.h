#pragma once
#include "Engine_Define.h"
#include "Tool_EventDefines.h"
#include <windows.h>

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

#define IMGUI_DEFINE_MATH_OPERATORS
#pragma push_macro("new")
#undef new
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_stdlib.h"

#include <cstdint>
#include <cmath>
#include <shobjidl.h>


#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "imfilebrowser.h"
#include "GraphEditor.h"
#pragma pop_macro("new")

#define MAX_ROW 129
#define MAX_COL 129

#define	TO_RAD  (XM_PI / 180.f)
#define	To_DEGREE ( 180.f / XM_PI )


namespace Tool
{
	extern unsigned int g_iWinSizeX;
	extern unsigned int g_iWinSizeY;
	extern float g_fAspectio;

	typedef struct FolderInfo
	{
		std::wstring	wstrFloderFullPath{};
		std::wstring	wstrFloderName{};
		unsigned int	iCountFloders{ 0 };
		unsigned int	iCountFiles{ 0 };
	}FLODER_INFO;
	/* 파일 관련 */
	typedef struct FileInfo
	{
		/* 전체 경로 */
		std::wstring wstrFileFullPath{};
		std::wstring wstrFileNameEXT{};
		std::wstring wstrFileName{};
		std::wstring wstrFileEXT{};

	public:
		FileInfo()
			: wstrFileFullPath{}, wstrFileEXT{ }, wstrFileName{}
		{

		}
		FileInfo(const std::wstring& wstrPath)
		{
			std::filesystem::path psFile(wstrPath);

			wstrFileFullPath = psFile.c_str();
			wstrFileNameEXT = psFile.filename().wstring();
			wstrFileName = psFile.filename().stem().wstring();
			wstrFileEXT = psFile.extension().wstring();
			return;
		}
		FileInfo(const std::filesystem::path& Path)
			: wstrFileFullPath(Path.wstring()), wstrFileName(Path.filename().stem().wstring()), wstrFileEXT(Path.extension().wstring())
			, wstrFileNameEXT(Path.filename().wstring())
		{
		}
	}FILE_INFO;

	enum class ELevelType : unsigned int
	{
		STATIC = 0,
		LOADING,
		LOGO,
		MAP,
		ANIMATION,
		EFFECT,
		CAMERA,
		UI,
		ATTACK_PRESET,
		ASSET_CONVERT,
		END
	};
	inline constexpr size_t g_iLevelType_Count = static_cast<size_t>(ELevelType::END);


#pragma region MapToolObjectBatchMode
	enum class EMapToolObjectBatchMode
	{
		Single,     
		Brush,
		END
	};

	static string MapToolObjectBatchMode_ToString(EMapToolObjectBatchMode eType)
	{

		switch (eType)
		{
		case Tool::EMapToolObjectBatchMode::Single:	return "Single";
		case Tool::EMapToolObjectBatchMode::Brush:	return "Brush";
		default:									return "Unknown";
		}

		return "Unknown";
	}

	static EMapToolObjectBatchMode MapObjectBatchMode_ToEnum(string strType)
	{
		if (strType == "Single") return EMapToolObjectBatchMode::Single;
		if (strType == "Brush")	 return EMapToolObjectBatchMode::Brush;

		return EMapToolObjectBatchMode::END;
	}

#pragma endregion

#pragma region EMapTool Batch Mode Picking Type

	enum class EMapTool_EmplaceType
	{
		Free,
		Picking,
		END,
	};

	static string EMapTool_EmplaceType_ToString(EMapTool_EmplaceType eType)
	{
		switch (eType)
		{
		case Tool::EMapTool_EmplaceType::Free:				return "Free";
		case Tool::EMapTool_EmplaceType::Picking:			return "Picking";
		default:											return "Unknown";
		}

		return "Unknown";
	}

	static EMapTool_EmplaceType	MapTool_EmplaceType_ToEnum(const string& strType)
	{
		if (strType == "Free")		return EMapTool_EmplaceType::Free;
		if (strType == "Picking")	return EMapTool_EmplaceType::Picking;
		return EMapTool_EmplaceType::END;
	}


#pragma endregion


	enum class EToolObjectType : unsigned int
	{
		MAPOBJECT,
		MESHEFFECT,
		UI,
		ANIMATION,
		END
	};

	enum class EStaticModel_Type
	{
		DEFUALT,
		INSTANCE,
		END,
	};


	static std::string StaticModelType_ToString(EStaticModel_Type eType)
	{
		switch (eType)
		{
		case Tool::EStaticModel_Type::DEFUALT:	return "DEFUALT";
		case Tool::EStaticModel_Type::INSTANCE:	return "INSTANCE";
		default		:							return "NONE";
		}

		return "NONE";
	}

	static EStaticModel_Type StaticModelType_ToEnum(std::string streType)
	{
		if (streType == "DEFUALT") return EStaticModel_Type::DEFUALT;
		if (streType == "INSTANCE") return EStaticModel_Type::INSTANCE;
		return EStaticModel_Type::END;
	}

	static std::string TypeToString(EToolObjectType eType)
	{
		switch (eType)
		{
		case Tool::EToolObjectType::MAPOBJECT:	return "MAPOBJECT";
		case Tool::EToolObjectType::MESHEFFECT: return "MESHEFFECT";
		case Tool::EToolObjectType::UI:			return "UI";
		case Tool::EToolObjectType::ANIMATION:	return "ANIMATION";
		default:								return "NONE";
		}

		return "NONE";
	}

	static EToolObjectType StringToType(const std::string& strType)
	{
		if (::strcmp(strType.c_str(), "MAPOBJECT") == 0)  return EToolObjectType::MAPOBJECT;
		else if (::strcmp(strType.c_str(), "MESHEFFECT") == 0) return EToolObjectType::MESHEFFECT;
		if (::strcmp(strType.c_str(), "UI") == 0) return EToolObjectType::UI;

			return EToolObjectType::END;
	}


	enum class EClientLevelType : unsigned int
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

	inline constexpr size_t	g_iClientLevelType_Count = static_cast<size_t>(EClientLevelType::END);

	static std::string ClientleveltypeToString(EClientLevelType eType)
	{
		switch (eType)
		{
		case Tool::EClientLevelType::STATIC:			return "STATIC";
		case Tool::EClientLevelType::LOADING:			return "LOADING";
		case Tool::EClientLevelType::LOGO:				return "LOGO";
		case Tool::EClientLevelType::TUTORIAL_VILLAGE:	return "TUTORIAL_VILLAGE";
		case Tool::EClientLevelType::TUTORIAL_BOSS:		return "TUTORIAL_BOSS";
		case Tool::EClientLevelType::SQUARE:			return "SQUARE";
		case Tool::EClientLevelType::TAVERN:			return "TAVERN";
		case Tool::EClientLevelType::KUANGKENG:			return "KUANGKENG";
		case Tool::EClientLevelType::LIANHUO:			return "LIANHUO";
		case Tool::EClientLevelType::TEST:				return "TEST";
		default:										return "NONE";
		}
		return "NONE";
	}

	static EClientLevelType StringToClientleveltype(const _string& str)
	{
		if (::strcmp(str.c_str(), "STATIC") == 0)
			return EClientLevelType::STATIC;
		else if (::strcmp(str.c_str(), "LOADING") == 0)
			return EClientLevelType::LOADING;
		else if (::strcmp(str.c_str(), "LOGO") == 0)
			return EClientLevelType::LOGO;
		else if (::strcmp(str.c_str(), "TUTORIAL_VILLAGE") == 0)
			return EClientLevelType::TUTORIAL_VILLAGE;
		else if (::strcmp(str.c_str(), "TUTORIAL_BOSS") == 0)
			return EClientLevelType::TUTORIAL_BOSS;
		else if (::strcmp(str.c_str(), "SQUARE") == 0)
			return EClientLevelType::SQUARE;
		else if (::strcmp(str.c_str(), "TAVERN") == 0)
			return EClientLevelType::TAVERN;
		else if (::strcmp(str.c_str(), "KUANGKENG") == 0)
			return EClientLevelType::KUANGKENG;
		else if (::strcmp(str.c_str(), "LIANHUO") == 0)
			return EClientLevelType::LIANHUO;
		else if (::strcmp(str.c_str(), "TEST") == 0)
			return EClientLevelType::TEST;
		else
			return EClientLevelType::END;
	}


	inline constexpr wchar_t g_wszCollider_AABB_Prototype_Tag[]		{ L"Prototype_Component_Collider_AABB" };
	inline constexpr wchar_t g_wszCollider_Sphere_Prototype_Tag[]	{ L"Prototype_Component_Collider_Sphere" };
	inline constexpr wchar_t g_wszCollider_OBB_Prototype_Tag[]		{ L"Prototype_Component_Collider_OBB" };



	inline constexpr _tchar g_wszPreviewObejctModelPath		[]{L"L../../Resources/Models/Map/Level/MakeObjectPreview/Model/"};
	inline constexpr _tchar g_wszMapObjectLayer				[]{ L"MapObject_Layer" };
	inline constexpr _tchar g_wszMapModelPath				[]{ L"../../Resources/Models/Map/"};
	inline constexpr _tchar g_wszStaticLightLayer			[]{ L"StaticLight_Layer" };
	inline constexpr _tchar g_wszMeshEffectPresetPath		[]{ L"../../Resources/Data/EffectData/EffectMeshPreset.json" };
	inline constexpr _tchar g_wszMeshPreviewLayer			[]{ L"MeshPreview_Layer" };
	inline constexpr _tchar g_wszPolygonLayer				[]{ L"Polygon_Layer" };
	inline constexpr _tchar g_wszTriggerBoxLayer			[]{ L"TriggerBox_Layer" };
	inline constexpr _tchar g_wszColMeshLayer				[]{ L"ColMesh_Layer" };
	inline constexpr _tchar g_wszStaticModelLayer			[]{ L"StaticModel_Layer" };
	inline constexpr _tchar g_wszInstanceModelLayer			[]{ L"InstanceModel_Layer" };
	inline constexpr _tchar g_wszCameraLayer				[]{ L"Camera_Layer" };

	inline constexpr _tchar g_wszPrototypeTagCanvas			[]{ L"Prototype_UI_Canvas" };
	inline constexpr _tchar g_wszPrototypeTagLayer			[]{ L"Prototype_UI_Layer" };
	inline constexpr _tchar g_wszPrototypeTagUI				[]{ L"Prototype_UI_UI" };

	inline constexpr _char  g_szMapObject_State				[]{ "g_iMapObject_State" };

	inline constexpr _char  g_szLandScape_TextureUV_LT		[]{ "g_LandScape_TextureUV_LT" };
	inline constexpr _char  g_szLandScape_TextureUV_RB		[]{ "g_LandScape_TextureUV_RB" };


	inline constexpr _tchar g_wszMapTexture_Path			[]{L"../../Resources/Textures/Map"};
	inline constexpr _char	g_szTexLandScape_Mix_RGB		[]{"g_TexLandScape_Mix_RGB" };
	inline constexpr _char	g_szTexLandScape_Mix_RGBA		[]{"g_TexLandScape_Mix_RGBA" };

	inline constexpr _char  g_szBase_Texture				[]{"g_Base_Texture"};
	inline constexpr _char  g_szMix_RGBA_Texture			[]{"g_Mix_RGBA_Texture"};


	inline constexpr _char	g_szMix_DH_Tile_Texture			[]{ "g_Mix_DH_Tile_Texture" };
	inline constexpr _char	g_szMix_NBR_Tile_Texture		[]{ "g_Mix_NBR_Tile_Texture"};


	inline constexpr _char	g_szCB_MIX_RGBA_INFO			[]{"CB_MIX_RGBA_INFO" };
	inline constexpr _char	g_szUse_Mix_RGBA_Map_Count		[]{"g_iUse_Mix_RGBA_Count"};

	inline constexpr _char	g_szCB_WaterData[]{ "CB_WaterData" };
	inline constexpr _char	g_szWaterTexture[]{ "g_WaterTexture" };




	inline constexpr _uint  Uint_NoneIndex{ 0xFFFFFFFF };

#pragma region Enum


#pragma region Map

	// Don't Touch , Talk Before Touch //
	/*----------------------- Map Tool ---------------------------*/
	enum class EMapObject_Type // UELOader에서 사용하는거  MapObject에 필요는 X
	{
		/* 지형지물 = Terrain = Object */
		STATICMODEL,
		INSTANCEMODEL,
		END,
	};

	static EMapObject_Type MapObjectType_StringToType(const std::string& strType)
	{
		if (strType == "STATICMODEL") return EMapObject_Type::STATICMODEL;
		if (strType == "INSTANCEMODEL") return EMapObject_Type::INSTANCEMODEL;

		return EMapObject_Type::END;
	};


#pragma region MapObject Draw Type
	enum class EMapObject_DrawType
	{
		Collider,
		Default,	/* NoneAnim Model */
		Instance,	/* NoneAnim Mdel Instance Mesh */
		END
	};

	static std::string EMapObject_DrawType_ToString(EMapObject_DrawType eType)
	{
		switch (eType)
		{
		case Tool::EMapObject_DrawType::Collider:	return "Collider";
		case Tool::EMapObject_DrawType::Default:	return "Default";
		case Tool::EMapObject_DrawType::Instance:	return "Instance";
		default:									return "UnKnown";
		};
		return "Unknown";
	};

	static EMapObject_DrawType EMapObject_DrawType_ToEnum(std::string strType)
	{
		if (strType == "None")			return Tool::EMapObject_DrawType::Collider;
		else if (strType == "Default")	return Tool::EMapObject_DrawType::Default;
		else if (strType == "Instance") return Tool::EMapObject_DrawType::Instance;

		return Tool::EMapObject_DrawType::END;
	};

#pragma endregion

#pragma region Client Make Path

	enum class EClientMakePath
	{
		/* Map Object 관련 */
		StaticObject,
		LandScape,
		Bush,
		Grass,
		Moss,
		Tree,
		Vine,
		Rock,
		Water,

		/*  생성 위치 잡아주는 역할  */
		Batch_Player,
		Batch_Monster,
		/*  Object Batch 관련  */
		Batch_Object,

		/* Trigger Box 관련 */
		TriggerBox_ChangeLevel,
		TriggerBox_MonsterSpawner,
		TriggerBox_GlobalEvent_BroadCaster,




		/* 맵 기능 관련 */
		Invisible_Wall,			/* 플레이어나 오브젝들이 못가게막아주는 투명벽 */
		Static_Light,			/* Static 점 조명 */

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
		END,
	};

	static string ClientMakePath_ToString(EClientMakePath eType)
	{
		switch (eType)
		{
		case Tool::EClientMakePath::StaticObject:							return "StaticObject";
		case Tool::EClientMakePath::LandScape:								return "LandScape";

			/* ------------------환경 요소---------------- */
		case Tool::EClientMakePath::Bush:									return "Bush";
		case Tool::EClientMakePath::Grass:									return "Grass";
		case Tool::EClientMakePath::Tree:									return "Tree";
		case Tool::EClientMakePath::Vine:									return "Vine";
		case Tool::EClientMakePath::Moss:									return "Moss";
		case Tool::EClientMakePath::Rock:									return "Rock";
		case Tool::EClientMakePath::Water:									return "Water";
			/* ------------------------------------------- */

			/*  --------- 생성 위치 잡아주는 역할 ---------*/

		case Tool::EClientMakePath::Batch_Player:							return "Batch_Player";
		case Tool::EClientMakePath::Batch_Monster:							return "Batch_Monster";
		case Tool::EClientMakePath::Batch_Object:							return "Batch_Object";

			/* -------------- Trigger Box -------------- */
		case Tool::EClientMakePath::TriggerBox_ChangeLevel:					return "TriggerBox_ChangeLevel";
		case Tool::EClientMakePath::TriggerBox_MonsterSpawner:				return "TriggerBox_MonsterSpawner";
		case Tool::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:		return "TriggerBox_GlobalEvent_BroadCaster";


			/* -------------- Invisible Wall ----------- */
		case Tool::EClientMakePath::Invisible_Wall:							return "Invisible_Wall";
		case Tool::EClientMakePath::Static_Light:							return "Static_Light";

		default:															return "Unknown";
		}
	};

	static EClientMakePath ClientMakePath_ToEnum(string strType)
	{
		/* 진짜 Map Object 관련  */
		if (strType == "StaticObject")										return EClientMakePath::StaticObject;
		if (strType == "LandScape")											return EClientMakePath::LandScape;
		if (strType == "Bush")												return EClientMakePath::Bush;
		if (strType == "Grass")												return EClientMakePath::Grass;
		if (strType == "Moss")												return EClientMakePath::Moss;
		if (strType == "Tree")												return EClientMakePath::Tree;
		if (strType == "Vine")												return EClientMakePath::Vine;
		if (strType == "Rock")												return EClientMakePath::Rock;
		if (strType == "Water")												return EClientMakePath::Water;

		/* 생성 위치관련 */
		if (strType == "Batch_Player")										return EClientMakePath::Batch_Player;
		if (strType == "Batch_Monster")										return EClientMakePath::Batch_Monster;
		if (strType == "Batch_Object")										return EClientMakePath::Batch_Object;

		/* Trigger Box 관련 */
		if (strType == "TriggerBox_ChangeLevel")							return EClientMakePath::TriggerBox_ChangeLevel;
		if (strType == "TriggerBox_MonsterSpawner")							return EClientMakePath::TriggerBox_MonsterSpawner;
		if (strType == "TriggerBox_GlobalEvent_BroadCaster")				return EClientMakePath::TriggerBox_GlobalEvent_BroadCaster;



		if (strType == "Invisible_Wall")									return EClientMakePath::Invisible_Wall;
		if (strType == "Static_Light")										return EClientMakePath::Static_Light;


		return EClientMakePath::END;
	}


#pragma endregion

	static std::string MapObjectType_TypeToString(EMapObject_Type eType)
	{

		switch (eType)
		{
		case Tool::EMapObject_Type::STATICMODEL:	return "STATICMODEL";
		default:									return "NONE";
		}

		return "NONE";

	}

	static _uint Get_IndexByMaterialSlotName(const std::wstring& wstrSlotName)
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

	static string Get_MaterialSlotNameAssimp(aiTextureType eType)
	{
		if (eType == aiTextureType_DIFFUSE)
			return "PM_Diffuse";
		else if (eType == aiTextureType_NORMALS)
			return "PM_Normals";
		else if (eType == aiTextureType_SPECULAR)
			return "PM_SpecularMasks";
		else if (eType == aiTextureType_AMBIENT)
			return "PM_Ambient";

		return string();
	}

	/*-----------------------------------------------------------*/

#pragma endregion


#pragma endregion
#pragma region UI
	enum class ERectTransform {
		LT = 0, CT, RT, LC, C, RC, LB, CB, RB, END
	};

	static const _string& RectTransformToString(ERectTransform eType)
	{
		static const _string sLT = "LT";
		static const _string sCT = "CT";
		static const _string sRT = "RT";
		static const _string sLC = "LC";
		static const _string sC = "C";
		static const _string sRC = "RC";
		static const _string sLB = "LB";
		static const _string sCB = "CB";
		static const _string sRB = "RB";
		static const _string sEMPTY = "";

		switch (eType)
		{
		case ERectTransform::LT: return sLT;
		case ERectTransform::CT: return sCT;
		case ERectTransform::RT: return sRT;
		case ERectTransform::LC: return sLC;
		case ERectTransform::C:  return sC;
		case ERectTransform::RC: return sRC;
		case ERectTransform::LB: return sLB;
		case ERectTransform::CB: return sCB;
		case ERectTransform::RB: return sRB;
		default: break;
		}
		return sEMPTY;
	}
	static ERectTransform StringToRectTransform(const _string& str)
	{
		if (::strcmp(str.c_str(), "LT") == 0) return ERectTransform::LT;
		if (::strcmp(str.c_str(), "CT") == 0) return ERectTransform::CT;
		if (::strcmp(str.c_str(), "RT") == 0) return ERectTransform::RT;
		if (::strcmp(str.c_str(), "LC") == 0) return ERectTransform::LC;
		if (::strcmp(str.c_str(), "C") == 0) return ERectTransform::C;
		if (::strcmp(str.c_str(), "RC") == 0) return ERectTransform::RC;
		if (::strcmp(str.c_str(), "LB") == 0) return ERectTransform::LB;
		if (::strcmp(str.c_str(), "CB") == 0) return ERectTransform::CB;
		if (::strcmp(str.c_str(), "RB") == 0) return ERectTransform::RB;
		return ERectTransform::END;
	}

	enum class EUIInteract : uint32_t
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

	enum EUIInteract_Flag : uint32_t
	{
		NONE = 0u,
		HOVER_ENTER = 1u << 1,
		HOVERING = 1u << 2,
		HOVER_EXIT = 1u << 3,
		PRESS_ENTER = 1u << 4,
		PRESSING = 1u << 5,
		PRESS_EXIT = 1u << 6,
		INVOKED = 1u << 7,
		END = 1u << 8
	};

	inline EUIInteract UIInteractFlagToUIInteract(EUIInteract_Flag eFlag)
	{
		switch (eFlag)
		{
		case EUIInteract_Flag::NONE:			return EUIInteract::NONE;
		case EUIInteract_Flag::HOVER_ENTER:	return EUIInteract::HOVER_ENTER;
		case EUIInteract_Flag::HOVERING:		return EUIInteract::HOVERING;
		case EUIInteract_Flag::HOVER_EXIT:	return EUIInteract::HOVER_EXIT;
		case EUIInteract_Flag::PRESS_ENTER:	return EUIInteract::PRESS_ENTER;
		case EUIInteract_Flag::PRESSING:		return EUIInteract::PRESSING;
		case EUIInteract_Flag::PRESS_EXIT:	return EUIInteract::PRESS_EXIT;
		case EUIInteract_Flag::INVOKED:		return EUIInteract::INVOKED;
		default:								return EUIInteract::NONE;
		}
	}

	inline EUIInteract_Flag UIInteractToUIInteractFlag(EUIInteract eEvent)
	{
		switch (eEvent)
		{
		case EUIInteract::NONE:			return EUIInteract_Flag::NONE;
		case EUIInteract::HOVER_ENTER:	return EUIInteract_Flag::HOVER_ENTER;
		case EUIInteract::HOVERING:		return EUIInteract_Flag::HOVERING;
		case EUIInteract::HOVER_EXIT:		return EUIInteract_Flag::HOVER_EXIT;
		case EUIInteract::PRESS_ENTER:	return EUIInteract_Flag::PRESS_ENTER;
		case EUIInteract::PRESSING:		return EUIInteract_Flag::PRESSING;
		case EUIInteract::PRESS_EXIT:		return EUIInteract_Flag::PRESS_EXIT;
		case EUIInteract::INVOKED:		return EUIInteract_Flag::INVOKED;
		default:							return EUIInteract_Flag::NONE;
		}
	}


	NLOHMANN_JSON_SERIALIZE_ENUM(EUIInteract,
		{
			{EUIInteract::NONE, "NONE"},
			{EUIInteract::HOVER_ENTER, "HOVER_ENTER"},
			{EUIInteract::HOVERING, "HOVERING"},
			{EUIInteract::HOVER_EXIT, "HOVER_EXIT"},
			{EUIInteract::PRESS_ENTER, "PRESS_ENTER"},
			{EUIInteract::PRESSING, "PRESSING"},
			{EUIInteract::PRESS_EXIT, "PRESS_EXIT"},
			{EUIInteract::INVOKED, "INVOKED"},
		})

		inline std::string UIInteractToString(EUIInteract eType)
	{
		switch (eType)
		{
		case EUIInteract::NONE: return "NONE";
		case EUIInteract::HOVER_ENTER: return "HOVER_ENTER";
		case EUIInteract::HOVERING: return "HOVERING";
		case EUIInteract::HOVER_EXIT: return "HOVER_EXIT";
		case EUIInteract::PRESS_ENTER: return "PRESS_ENTER";
		case EUIInteract::PRESSING: return "PRESSING";
		case EUIInteract::PRESS_EXIT: return "PRESS_EXIT";
		case EUIInteract::INVOKED: return "INVOKED";
		default: return "";
		}
	}

	inline EUIInteract StringToUIInteract(const std::string& str)
	{
		if (str == "NONE") return EUIInteract::NONE;
		else if (str == "HOVER_ENTER") return EUIInteract::HOVER_ENTER;
		else if (str == "HOVERING") return EUIInteract::HOVERING;
		else if (str == "HOVER_EXIT") return EUIInteract::HOVER_EXIT;
		else if (str == "PRESS_ENTER") return EUIInteract::PRESS_ENTER;
		else if (str == "PRESSING") return EUIInteract::PRESSING;
		else if (str == "PRESS_EXIT") return EUIInteract::PRESS_EXIT;
		else if (str == "INVOKED") return EUIInteract::INVOKED;
		else return EUIInteract::END;
	}

	enum class EUIShaderPass
	{
		DEFAULT = 0,
		COLOR,
		PROGRESS,
		DISOLVE,
		NOISE,
		GLOW,
		END
	};

	inline std::string UIShaderPassToString(EUIShaderPass eType)
	{
		switch (eType)
		{
		case EUIShaderPass::DEFAULT: return "DEFAULT";
		case EUIShaderPass::COLOR: return "COLOR";
		case EUIShaderPass::PROGRESS: return "PROGRESS";
		case EUIShaderPass::DISOLVE: return "DISOLVE";
		case EUIShaderPass::NOISE: return "NOISE";
		case EUIShaderPass::GLOW: return "GLOW";
		default: return "";
		}
	}

	inline EUIShaderPass StringToUIShaderPass(const std::string& str)
	{
		if (str == "DEFAULT") return EUIShaderPass::DEFAULT;
		else if (str == "COLOR") return EUIShaderPass::COLOR;
		else if (str == "PROGRESS") return EUIShaderPass::PROGRESS;
		else if (str == "DISOLVE") return EUIShaderPass::DISOLVE;
		else if (str == "NOISE") return EUIShaderPass::NOISE;
		else if (str == "GLOW") return EUIShaderPass::GLOW;
		else return EUIShaderPass::DEFAULT;
	}

	enum class EUIFlip
	{
		NONE = 0,
		FLIP_X,
		FLIP_Y,
		FLIP_XY,
		END
	};

	enum class EUIPrefabType
	{
		NOT_PREFAB,
		MONSTER_NAMEPLATE,
		DAMAGE_FONTS_COMMON,
		DAMAGE_FONTS_CRITICAL,
		DAMAGE_FONTS_HIT,
		BOSS_NAMEPLATE,
		MINIMAP_MONSTER_ICON,
		TUTORIAL_PANNEL,
		END
	};

	inline std::string UIPrefabTypeToString(EUIPrefabType eType)
	{
		switch (eType)
		{
		case EUIPrefabType::NOT_PREFAB:				return "NOT_PREFAB";
		case EUIPrefabType::MONSTER_NAMEPLATE:		return "MONSTER_NAMEPLATE";
		case EUIPrefabType::DAMAGE_FONTS_COMMON:	return "DAMAGE_FONTS_COMMON";
		case EUIPrefabType::DAMAGE_FONTS_CRITICAL:	return "DAMAGE_FONTS_CRITICAL";
		case EUIPrefabType::DAMAGE_FONTS_HIT:		return "DAMAGE_FONTS_HIT";
		case EUIPrefabType::BOSS_NAMEPLATE:			return "BOSS_NAMEPLATE";
		case EUIPrefabType::MINIMAP_MONSTER_ICON:	return "MINIMAP_MONSTER_ICON";
		case EUIPrefabType::TUTORIAL_PANNEL:		return "TUTORIAL_PANNEL";
		case EUIPrefabType::END:					return "";
		default: return "";
		}
	}
#pragma endregion
	static void Model_Path_Check(OUT wstring& wstrModelPath)
	{
		std::wstring searchPath = wstrModelPath;
		std::replace(searchPath.begin(), searchPath.end(), L'\\', L'/');

		path p(wstrModelPath);

		std::wstring wstrExt = p.extension().wstring();
		std::wstring wstrStem = p.stem().wstring();

		bool bModified = false;

		if (searchPath.find(L"/V1/") != std::wstring::npos)
		{
			if (wstrStem.find(L"_V1") == std::wstring::npos)
			{
				wstrStem += L"_V1";
				bModified = true;
			}
		}
		else if (searchPath.find(L"/V2/") != std::wstring::npos)
		{
			if (wstrStem.find(L"_V2") == std::wstring::npos)
			{
				wstrStem += L"_V2";
				bModified = true;
			}
		}

		if (bModified)
		{
			p.replace_filename(wstrStem + wstrExt);
			wstrModelPath = p.wstring();
		}
	}


#pragma region BroadCast Enum Mapping

	enum class EGlobal_Broadcast_Type
	{
		NONE,
		TUTORIAL_BOSS_CONTATCT,
		TUTORIAL_BOSS_CONTATCT_END,
		END,
	};

	// 헤더 파일의 Enum 선언 바로 밑이나, cpp 파일 상단에 선언해 둡니다.
	static const char* g_szGlobalBroadCastType[(int)EGlobal_Broadcast_Type::END] = {
		"NONE",
		"TUTORIAL_BOSS_CONTATCT",
		"TUTORIAL_BOSS_CONTATCT_END",
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

#pragma region Struct
#pragma endregion
}

using namespace Tool; 