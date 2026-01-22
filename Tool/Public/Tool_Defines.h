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




	static string TypeToString(EToolObjectType eType)
	{
		switch (eType)
		{
		case Tool::EToolObjectType::TERRAIN:
			return "TERRAIN";
		case Tool::EToolObjectType::COLMESH:
			return "COLMESH";
		case Tool::EToolObjectType::STATICMODEL:
			return "STATICMODEL";
		case Tool::EToolObjectType::DUMMY:
			return "DUMMY";
		case Tool::EToolObjectType::LIGHT:
			return "LIGHT";
		case Tool::EToolObjectType::COMPONENT_COLLIDER:
			return "COMPONENT_COLLIDER";
		case Tool::EToolObjectType::VERTEXOBJECT:
			return "VERTEXOBJECT";
		case Tool::EToolObjectType::POLYGON:
			return "POLYGON";
		case Tool::EToolObjectType::TRIGGERBOX:
			return "TRIGGERBOX";
		case Tool::EToolObjectType::MESHEFFECT:
			return "MESHEFFECT";
		}

		return "NONE";
	}

	static EToolObjectType StringToType(const string& strType)
	{
		if (::strcmp(strType.c_str(), "TERRAIN") == 0)
			return EToolObjectType::TERRAIN;
		else if (::strcmp(strType.c_str(), "COLMESH") == 0)
			return EToolObjectType::COLMESH;
		else if (::strcmp(strType.c_str(), "STATICMODEL") == 0)
			return EToolObjectType::STATICMODEL;
		else if (::strcmp(strType.c_str(), "DUMMY") == 0)
			return EToolObjectType::DUMMY;
		else if (::strcmp(strType.c_str(), "LIGHT") == 0)
			return EToolObjectType::LIGHT;
		else if (::strcmp(strType.c_str(), "COMPONENT_COLLIDER") == 0)
			return EToolObjectType::COMPONENT_COLLIDER;
		else if (::strcmp(strType.c_str(), "VERTEXOBJECT") == 0)
			return EToolObjectType::VERTEXOBJECT;
		else if (::strcmp(strType.c_str(), "POLYGON") == 0)
			return EToolObjectType::POLYGON;
		else if (::strcmp(strType.c_str(), "TRIGGERBOX") == 0)
			return EToolObjectType::TRIGGERBOX;
		else if (::strcmp(strType.c_str(), "MESHEFFECT") == 0)
			return EToolObjectType::MESHEFFECT;
		else
			return EToolObjectType::END;
	}


	enum class EClientLevelType : unsigned int
	{
		STATIC = 0,
		LOGO,
		END
	};

	inline constexpr size_t	g_iClientLevelType_Count = static_cast<size_t>(EClientLevelType::END);

	static string ClientleveltypeToString(EClientLevelType eType)
	{
		switch (eType)
		{
		case Tool::EClientLevelType::STATIC:
			return "STATIC";
			break;
		case Tool::EClientLevelType::LOGO:
			return "LOGO";
			break;
		case Tool::EClientLevelType::END:
			break;
		}
	}

	static EClientLevelType StringToClientleveltype(const _string& str)
	{
		if (::strcmp(str.c_str(), "STATIC") == 0)
			return EClientLevelType::STATIC;
		else if (::strcmp(str.c_str(), "LOGO") == 0)
			return EClientLevelType::LOGO;
		else
			return EClientLevelType::END;
	}


	inline constexpr _tchar g_wszStaticLightLayer[]{ L"StaticLight_Layer\0" };
	inline constexpr _tchar g_wszMeshEffectPresetPath[]{ L"../../Resources/Data/EffectData/EffectMeshPreset.json\0" };
	inline constexpr _tchar g_wszMeshPreviewLayer[]{ L"MeshPreview_Layer\0" };
	inline constexpr _tchar g_wszPolygonLayer[]{ L"Polygon_Layer\0" };
	inline constexpr _tchar g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer\0" };
	inline constexpr _tchar g_wszColMeshLayer[]{ L"ColMesh_Layer\0" };
	inline constexpr _tchar g_wszStaticModelLayer[]{ L"StaticModel_Layer\0" };
	inline constexpr _tchar g_wszDummyColliderLayer[]{ L"DummyCollider_Layer\0" };
	inline constexpr _tchar g_wszCameraLayer[]{ L"Camera_Layer\0" };

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