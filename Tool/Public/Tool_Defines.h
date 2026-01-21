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

#include "ImGuizmo.h"
#include "ImSequencer.h"
#include "ImZoomSlider.h"
#include "ImCurveEdit.h"
#include "GraphEditor.h"
#pragma pop_macro("new")

#define MAX_ROW 129
#define MAX_COL 129

namespace Tool
{
	extern unsigned int g_iWinSizeX;
	extern unsigned int g_iWinSizeY;
	extern float g_fAspectio;

	enum class LEVELID : unsigned int
	{
		STATIC = 0,
		LOADING,
		MAP,
		ANIMATION,
		EFFECT,
		CAMERA,
		ASSET_CONVERT,
		END
	};
	const extern size_t g_iLevelID_Count;

	enum class ToolObjectType : unsigned int
	{
		TERRAIN = 0,
		COLMESH,
		STATICMODEL,
		DUMMY,
		LIGHT,
		COMPONENT_COLLIDER,
		VERTEXOBJECT,
		POLYGON,
		TRIGGERBOX,
		MESHEFFECT,
		END
	};

	static string TypeToString(ToolObjectType eType)
	{
		switch (eType)
		{
		case Tool::ToolObjectType::TERRAIN:
			return "TERRAIN";
		case Tool::ToolObjectType::COLMESH:
			return "COLMESH";
		case Tool::ToolObjectType::STATICMODEL:
			return "STATICMODEL";
		case Tool::ToolObjectType::DUMMY:
			return "DUMMY";
		case Tool::ToolObjectType::LIGHT:
			return "LIGHT";
		case Tool::ToolObjectType::COMPONENT_COLLIDER:
			return "COMPONENT_COLLIDER";
		case Tool::ToolObjectType::VERTEXOBJECT:
			return "VERTEXOBJECT";
		case Tool::ToolObjectType::POLYGON:
			return "POLYGON";
		case Tool::ToolObjectType::TRIGGERBOX:
			return "TRIGGERBOX";
		case Tool::ToolObjectType::MESHEFFECT:
			return "MESHEFFECT";
		}

		return "NONE";
	}

	static ToolObjectType StringToType(const string& strType)
	{
		if (::strcmp(strType.c_str(), "TERRAIN") == 0)
			return ToolObjectType::TERRAIN;
		else if (::strcmp(strType.c_str(), "COLMESH") == 0)
			return ToolObjectType::COLMESH;
		else if (::strcmp(strType.c_str(), "STATICMODEL") == 0)
			return ToolObjectType::STATICMODEL;
		else if (::strcmp(strType.c_str(), "DUMMY") == 0)
			return ToolObjectType::DUMMY;
		else if (::strcmp(strType.c_str(), "LIGHT") == 0)
			return ToolObjectType::LIGHT;
		else if (::strcmp(strType.c_str(), "COMPONENT_COLLIDER") == 0)
			return ToolObjectType::COMPONENT_COLLIDER;
		else if (::strcmp(strType.c_str(), "VERTEXOBJECT") == 0)
			return ToolObjectType::VERTEXOBJECT;
		else if (::strcmp(strType.c_str(), "POLYGON") == 0)
			return ToolObjectType::POLYGON;
		else if (::strcmp(strType.c_str(), "TRIGGERBOX") == 0)
			return ToolObjectType::TRIGGERBOX;
		else if (::strcmp(strType.c_str(), "MESHEFFECT") == 0)
			return ToolObjectType::MESHEFFECT;
		else
			return ToolObjectType::END;
	}

	inline constexpr _tchar g_wszStaticLightLayer[]{ L"StaticLight_Layer\0" };
	inline constexpr _tchar g_wszMeshEffectPresetPath[]{ L"../../Resources/Data/EffectData/EffectMeshPreset.json\0" };
	inline constexpr _tchar g_wszMeshPreviewLayer[]{ L"MeshPreview_Layer\0" };
	inline constexpr _tchar g_wszPolygonLayer[]{ L"Polygon_Layer\0" };
	inline constexpr _tchar g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer\0" };
	inline constexpr _tchar g_wszColMeshLayer[]{ L"ColMesh_Layer\0" };
	inline constexpr _tchar g_wszStaticModelLayer[]{ L"StaticModel_Layer\0" };
	inline constexpr _tchar g_wszDummyColliderLayer[]{ L"DummyCollider_Layer\0" };

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
}

using namespace Tool; 