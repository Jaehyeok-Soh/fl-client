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
		LOGO,
		END
	};

	inline constexpr size_t	g_iClientLevelType_Count = static_cast<size_t>(EClientLevelType::END);

	static std::string ClientleveltypeToString(EClientLevelType eType)
	{
		switch (eType)
		{
		case Tool::EClientLevelType::STATIC: return "STATIC";
		case Tool::EClientLevelType::LOGO:	 return "LOGO";
		case Tool::EClientLevelType::LOADING:return "LOADING";
		default:							 return "NONE";
		}
		return "NONE";
	}

	static EClientLevelType StringToClientleveltype(const _string& str)
	{
		if (::strcmp(str.c_str(), "STATIC") == 0)
			return EClientLevelType::STATIC;
		else if (::strcmp(str.c_str(), "LOGO") == 0)
			return EClientLevelType::LOGO;
		else if (::strcmp(str.c_str(), "LOADING") == 0)
			return EClientLevelType::LOADING;
		else
			return EClientLevelType::END;
	}

	inline constexpr _tchar g_wszMapObjectLayer[]{ L"MapObject_Layer" };
	inline constexpr _tchar g_wszStaticLightLayer[]{ L"StaticLight_Layer" };
	inline constexpr _tchar g_wszMeshEffectPresetPath[]{ L"../../Resources/Data/EffectData/EffectMeshPreset.json" };
	inline constexpr _tchar g_wszMeshPreviewLayer[]{ L"MeshPreview_Layer" };
	inline constexpr _tchar g_wszPolygonLayer[]{ L"Polygon_Layer" };
	inline constexpr _tchar g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer" };
	inline constexpr _tchar g_wszColMeshLayer[]{ L"ColMesh_Layer" };
	inline constexpr _tchar g_wszStaticModelLayer[]{ L"StaticModel_Layer" };
	inline constexpr _tchar g_wszInstanceModelLayer[]{ L"InstanceModel_Layer" };
	inline constexpr _tchar g_wszCameraLayer[]{ L"Camera_Layer" };

	inline constexpr _tchar g_wszPrototypeTagCanvas[]{ L"Prototype_UI_Canvas" };
	inline constexpr _tchar g_wszPrototypeTagLayer[]{ L"Prototype_UI_Layer" };
	inline constexpr _tchar g_wszPrototypeTagUI[]{ L"Prototype_UI_UI" };


	inline constexpr _char  g_szMapObject_State[]{ "g_iMapObject_State" };

	inline constexpr _char  g_szLandScape_TextureUV_LT[]{ "g_LandScape_TextureUV_LT" };
	inline constexpr _char  g_szLandScape_TextureUV_RB[]{ "g_LandScape_TextureUV_RB" };


	inline constexpr _uint  g_Uint_NoneIndex{ 0xFFFFFFFF };

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
		StaticObject,
		LandScape,
		END,
	};

	static string ClientMakePath_ToString(EClientMakePath eType)
	{
		switch (eType)
		{
		case Tool::EClientMakePath::StaticObject:	return "StaticObject";
		case Tool::EClientMakePath::LandScape:		return "LandScape";
		default:									return "Unknown";
		}
	};

	static EClientMakePath ClientMakePath_ToEnum(string strType)
	{
		if (strType == "StaticObject")	return EClientMakePath::StaticObject;
		if (strType == "LandScape")		return EClientMakePath::LandScape;

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
		HOVERING = 1u << 2,
		HOVER_EXIT = 1u << 3,
		PRESS_ENTER = 1u << 4,
		PRESSING = 1u << 5,
		PRESS_EXIT = 1u << 6,
		INVOKED = 1u << 7,
		END = 1u << 8
	};

	inline EUIEvent EventFlagToEvent(EUIEvent_Flag eFlag)
	{
		switch (eFlag)
		{
		case EUIEvent_Flag::NONE:			return EUIEvent::NONE;
		case EUIEvent_Flag::HOVER_ENTER:	return EUIEvent::HOVER_ENTER;
		case EUIEvent_Flag::HOVERING:		return EUIEvent::HOVERING;
		case EUIEvent_Flag::HOVER_EXIT:	return EUIEvent::HOVER_EXIT;
		case EUIEvent_Flag::PRESS_ENTER:	return EUIEvent::PRESS_ENTER;
		case EUIEvent_Flag::PRESSING:		return EUIEvent::PRESSING;
		case EUIEvent_Flag::PRESS_EXIT:	return EUIEvent::PRESS_EXIT;
		case EUIEvent_Flag::INVOKED:		return EUIEvent::INVOKED;
		default:								return EUIEvent::NONE;
		}
	}

	inline EUIEvent_Flag EventToEventFlag(EUIEvent eEvent)
	{
		switch (eEvent)
		{
		case EUIEvent::NONE:			return EUIEvent_Flag::NONE;
		case EUIEvent::HOVER_ENTER:	return EUIEvent_Flag::HOVER_ENTER;
		case EUIEvent::HOVERING:		return EUIEvent_Flag::HOVERING;
		case EUIEvent::HOVER_EXIT:		return EUIEvent_Flag::HOVER_EXIT;
		case EUIEvent::PRESS_ENTER:	return EUIEvent_Flag::PRESS_ENTER;
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

	enum class EUIShaderPass
	{
		DEFAULT = 0,
		DEFAULT_ALPHA,
		COLOR,
		FADE,
		PROGRESS,
		END
	};

	inline std::string UIShaderPassToString(EUIShaderPass eType)
	{
		switch (eType)
		{
		case EUIShaderPass::DEFAULT: return "DEFAULT";
		case EUIShaderPass::DEFAULT_ALPHA: return "DEFAULT_ALPHA";
		case EUIShaderPass::COLOR: return "COLOR";
		case EUIShaderPass::FADE: return "FADE";
		case EUIShaderPass::PROGRESS: return "PROGRESS";
		default: return "";
		}
	}

	inline EUIShaderPass StringToUIShaderPass(const std::string& str)
	{
		if (str == "DEFAULT") return EUIShaderPass::DEFAULT;
		else if (str == "DEFAULT_ALPHA") return EUIShaderPass::DEFAULT_ALPHA;
		else if (str == "COLOR") return EUIShaderPass::COLOR;
		else if (str == "FADE") return EUIShaderPass::FADE;
		else if (str == "PROGRESS") return EUIShaderPass::PROGRESS;
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

#pragma region Struct

#pragma endregion


}

using namespace Tool; 