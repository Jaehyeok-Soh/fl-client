#pragma once
#include "pch.h"
#include "Engine_Define.h"
#include <windows.h>

#include "Animation_Struct.h"

#include "AnimObj.h"
#include "Model.h"
#include "Bone.h"
#include "ModelAnimation.h"
#include "ComputeShader.h"

namespace fs = std::filesystem;

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

struct LoadAnimModel { using Signature = void(path animModelPath); };

typedef struct tagDirectory
{
	fs::path directory;

	vector<fs::path> files;

	vector<tagDirectory> directories;

	vector<fs::path> GetFiles() { return files; }
	vector<tagDirectory> GetDirectories() { return directories; }
}DIR;

struct AnimEvent
{
	enum Enum
	{
		EFFECT,
		OVERLAP,
		SOUND,
		NONE,
		END
	};
};

typedef struct tagAnimEventScriptBase
{
	_float fDuration = {};
	_float Get_Duration() { return fDuration; }

	_bool bIsSelected = { false };

}ANIM_EVENT_SCRIPT_BASE;

typedef struct tagAnimEventBase
{
	AnimEvent::Enum eEventType = AnimEvent::NONE;
	string strAnimTag = {};
	_uint iAnimIndex = {};
	_float fTrackPosition = {};

	vector<ANIM_EVENT_SCRIPT_BASE> vecScript;
}ANIM_EVENT_BASE;

typedef struct tagAnimEventInfo
{
	/// <summary>
	/// 애니메이션 주체 이름
	/// 모델 폴더 이름이 적합
	/// </summary>
	string strOwnerTag = {};

	vector<ANIM_EVENT_BASE> vecAnimEvents[AnimEvent::END];
}ANIM_EVENT_INFO;



///////
/////// panel animation controller
///////

typedef struct tagAnimInfo
{
	_uint iIndex = {};
	string strAnimName = {};
	wstring wstrAnimName = {};
	class CModelAnimation* pModelAnimation = { nullptr };
	_float fDuration = {};

	//chennel
}ANIMINFO;

typedef struct tagBoneInfo
{
	_uint iIndex = {};
	_uint iParentIndex = {};
	string strBoneName = {};
	wstring wstrBoneName = {};
	class CBone* pBone = { nullptr };
	Matrix matTransform = {};
}BONEINFO;

typedef struct tagAnimControllerInfo
{
	_uint iTotalAnimCount = {};
	_uint iCurrentAnimIndex = {};

	_uint fDuration = {};
	_uint fTrackPosition = {};

	_float fTickPerSecond = {};

	_float fPlayRate = { 1.f };

	_bool bPlay = { true };
	_bool bLoop = { true };

	//
	_float fTranformScale = { 1.f };

	vector<ANIMINFO> vecAnimInfo;
	vector<BONEINFO> vecBoneInfo;
}ANIMCTRLINFO;