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

	enum class EToolObjectType : unsigned int
	{
		MAPOBJECT,
		MESHEFFECT,
		UI,
		END
	};

	enum class EStaticModel_Type
	{
		DEFUALT,
		INSTANCE,
		END,
	};


	static string StaticModelType_ToString(EStaticModel_Type eType)
	{
		switch (eType)
		{
		case Tool::EStaticModel_Type::DEFUALT: return "DEFUALT";
		case Tool::EStaticModel_Type::INSTANCE:	return "INSTANCE";
		default		:						return "NONE";
		}


		return "NONE";
	}
	static EStaticModel_Type StaticModelType_ToEnum(string streType)
	{
		if (streType == "DEFUALT") return EStaticModel_Type::DEFUALT;
		if (streType == "INSTANCE") return EStaticModel_Type::INSTANCE;
		return EStaticModel_Type::END;
	}



	static string TypeToString(EToolObjectType eType)
	{
		switch (eType)
		{
		case Tool::EToolObjectType::MAPOBJECT:	return "MAPOBJECT";
		case Tool::EToolObjectType::MESHEFFECT: return "MESHEFFECT";
		case Tool::EToolObjectType::UI:			return "UI";
		default:								return "NONE";
		}

		return "NONE";
	}

	static EToolObjectType StringToType(const string& strType)
	{
		if (::strcmp(strType.c_str(), "MAPOBJECT") == 0)  return EToolObjectType::MAPOBJECT;
		else if (::strcmp(strType.c_str(), "MESHEFFECT") == 0) return EToolObjectType::MESHEFFECT;
		if (::strcmp(strType.c_str(), "UI") == 0) return EToolObjectType::UI;

			return EToolObjectType::END;
	}


	enum class EClientLevelType : unsigned int
	{
		STATIC = 0,
		LOGO,
		LOADING,
		END
	};

	inline constexpr size_t	g_iClientLevelType_Count = static_cast<size_t>(EClientLevelType::END);

	static string ClientleveltypeToString(EClientLevelType eType)
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

	inline constexpr _tchar g_wszStaticLightLayer[]{ L"StaticLight_Layer" };
	inline constexpr _tchar g_wszMeshEffectPresetPath[]{ L"../../Resources/Data/EffectData/EffectMeshPreset.json" };
	inline constexpr _tchar g_wszMeshPreviewLayer[]{ L"MeshPreview_Layer" };
	inline constexpr _tchar g_wszPolygonLayer[]{ L"Polygon_Layer" };
	inline constexpr _tchar g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer" };
	inline constexpr _tchar g_wszColMeshLayer[]{ L"ColMesh_Layer" };
	inline constexpr _tchar g_wszStaticModelLayer[]{ L"StaticModel_Layer" };
	inline constexpr _tchar g_wszCameraLayer[]{ L"Camera_Layer" };

	inline constexpr _tchar g_wszPrototypeTagCanvas[]{ L"Prototype_UI_Canvas" };
	inline constexpr _tchar g_wszPrototypeTagLayer[]{ L"Prototype_UI_Layer" };
	inline constexpr _tchar g_wszPrototypeTagUI[]{ L"Prototype_UI_UI" };
#pragma region Enum


#pragma region Map
	// Don't Touch , Talk Before Touch //
	/*----------------------- Map Tool ---------------------------*/
	enum class EMapObject_Type
	{
		/* 지형지물 = Terrain = Object */
		STATICMODEL,
		END,
	};

	static EMapObject_Type MapObjectType_StringToType(const string& strType)
	{
		if (strType == "STATICMODEL") return EMapObject_Type::STATICMODEL;


		return EMapObject_Type::END;
	}

	static string MapObjectType_TypeToString(EMapObject_Type eType)
	{

		switch (eType)
		{
		case Tool::EMapObject_Type::STATICMODEL:	return "STATICMODEL";
		default:									return "NONE";
		}

		return "NONE";

	}


	/*-----------------------------------------------------------*/

#pragma endregion


#pragma endregion


#pragma region Struct

#pragma endregion


}

using namespace Tool; 