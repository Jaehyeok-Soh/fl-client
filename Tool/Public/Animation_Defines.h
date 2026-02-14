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
#include "PhysicsCCT.h"

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
	_uint iCurrentBoneIndex = {};

	_uint fDuration = {};
	_uint fTrackPosition = {};

	_float fTickPerSecond = {};

	_float fPlayRate = { 1.f };

	_bool bPlay = { true };
	_bool bLoop = { true };

	vector<ANIMINFO> vecAnimInfo;
	vector<BONEINFO> vecBoneInfo;

	/// <summary>
	/// 엔진 데이터
	/// </summary>
	CAnimObj* pCurrentObject = { nullptr };
	CModel* pModel = { nullptr };
	_uint  iCurrentAnimationState = {};
	vector<class CBone*> vecBones;
	vector<class CModelAnimation*> vecAnimations;

	// 스케일
	_float fTranformScale = { 1.f };
}ANIMCTRLINFO;