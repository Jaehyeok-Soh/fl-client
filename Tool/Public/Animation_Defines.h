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

#include "PhysicsAttackOverlap.h"

namespace fs = std::filesystem;

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

struct LoadAnimModel { using Signature = void(path animModelPath); };
struct LoadAttackOverlap { using Signature = void(CPhysicsAttackOverlap* pAttackOverlap); };

static const char* eventTypeItems[EAnimEvent::END] = { "OVERLAP", "EFFECT", "SOUND", "NONE" };
static const char* overlapTypeItems[EOverlapType::END] = { "Box", "Sphere", "Capsule"};
static const char* filterGroupItems[EPhysicsFilterType::END] =
{
	"PLAYER",
	"ATTACK",
	"SKILL",
	"ATTACK_PROJECTTILE",
	"SKILL_PROJECTTILE",
	"MONSTER",
	"MONSTER_ATTACK",
	"MONSTER_SKILL",
	"MONSTER_ATTACK_PROJECTTILE",
	"MONSTER_SKILL_PROJECTTILE",
	"MAP",
	"OBJECT1",
	"OBJECT2",
	"TRIGGER_UI",
	"TRIGGER_QUEST",
	"TRIGGER_SPAWN",
	"TRIGGER_DIRECTION",
	"NONE"
};

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

	_int iCurrentAttackEventIndex = { -1 };
	_int iCurrentEffectEventIndex = { -1 };

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
	fs::path modelPath = {};
	CAnimObj* pCurrentObject = { nullptr };
	CModel* pModel = { nullptr };
	_uint  iCurrentAnimationState = {};
	vector<class CBone*> vecBones;
	vector<class CModelAnimation*> vecAnimations;

	// 스케일
	_float fTranformScale = { 1.f };
}ANIMCTRLINFO;