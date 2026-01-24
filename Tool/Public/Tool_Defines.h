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

	enum class ELevelType : unsigned int
	{
		STATIC = 0,
		LOADING,
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
		END
	};

	static string TypeToString(EToolObjectType eType)
	{
		switch (eType)
		{
		case Tool::EToolObjectType::MAPOBJECT:	return "MAPOBJECT";
		case Tool::EToolObjectType::MESHEFFECT: return "MESHEFFECT";
		default:								return "NONE";
		}

		return "NONE";
	}

	static EToolObjectType StringToType(const string& strType)
	{
		if		(strType == "MAPOBJECT") return EToolObjectType::MAPOBJECT;
		else if (strType == "MAPOBJECT") return EToolObjectType::MESHEFFECT;

		return EToolObjectType::END;
	}

	inline constexpr _tchar g_wszStaticLightLayer[]{ L"StaticLight_Layer" };
	inline constexpr _tchar g_wszMeshEffectPresetPath[]{ L"../../Resources/Data/EffectData/EffectMeshPreset.json" };
	inline constexpr _tchar g_wszMeshPreviewLayer[]{ L"MeshPreview_Layer" };
	inline constexpr _tchar g_wszPolygonLayer[]{ L"Polygon_Layer" };
	inline constexpr _tchar g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer" };
	inline constexpr _tchar g_wszColMeshLayer[]{ L"ColMesh_Layer" };
	inline constexpr _tchar g_wszStaticModelLayer[]{ L"StaticModel_Layer" };
	inline constexpr _tchar g_wszDummyColliderLayer[]{ L"DummyCollider_Layer" };
	inline constexpr _tchar g_wszCameraLayer[]{ L"Camera_Layer" };

	struct HoleBridges
	{
		_int iHolePolyIndex = { -1 };
		_int iOuterVertexIndex = { -1 };
		_int iHoleVertexIndex = { -1 };
	};

	typedef struct tagEffectPreset
	{
		_uint iPressetID = 0;
		string strName = "";

		EFFECT_PRESET_SNAPSHOT snapShot = {};
	}EFFECT_PRESET;

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