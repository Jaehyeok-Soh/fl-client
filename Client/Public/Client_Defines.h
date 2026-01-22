#pragma once

#include "Engine_Define.h"
#include <windows.h>
#include "Client_EventDefine.h"

extern HWND			g_hWnd;
extern HINSTANCE	g_hInstance;

namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum class ECollideLayer : unsigned int
	{
		PLAYER_BODY,
		PLAYER_WEAPON,
		PLAYER_SKILL,
		PLAYER_LEFTHAND,
		PLAYER_RIGHTHAND,
		PLAYER_LEFTFOOT,
		PLAYER_RIGHTFOOT,
		PLAYER_LEFTSHOULDER,
		ENEMY_BODY,
		ENEMY_WEAPON,
		ENEMY_SKILL,
		ENEMY_LEFTHAND,
		ENEMY_RIGHTHAND,
		ENEMY_LEFTFOOT,
		ENEMY_RIGHTFOOT,
		ENEMY_LEFTSHOULDER,
		ENEMY_RIGHTSHOULDER,
		TRIGGER,
		COLMESH,
		END
	};

	inline bool Is_PlayerAttackLayer(ECollideLayer eLayer)
	{
		switch (eLayer)
		{
		case Client::ECollideLayer::PLAYER_WEAPON:
		case Client::ECollideLayer::PLAYER_LEFTHAND:
		case Client::ECollideLayer::PLAYER_RIGHTHAND:
		case Client::ECollideLayer::PLAYER_LEFTFOOT:
		case Client::ECollideLayer::PLAYER_RIGHTFOOT:
		case Client::ECollideLayer::PLAYER_LEFTSHOULDER:
			return true;
		default:
			return false;
		}
	}
	inline bool Is_PlayerSkillLayer(ECollideLayer eLayer)
	{
		if (eLayer == Client::ECollideLayer::PLAYER_SKILL)
			return true;
		
		return false;
	}

	inline bool Is_EnemyAttackLayer(ECollideLayer eLayer)
	{
		switch (eLayer)
		{
		case Client::ECollideLayer::ENEMY_WEAPON:
		case Client::ECollideLayer::ENEMY_LEFTHAND:
		case Client::ECollideLayer::ENEMY_RIGHTHAND:
		case Client::ECollideLayer::ENEMY_LEFTFOOT:
		case Client::ECollideLayer::ENEMY_RIGHTFOOT:
		case Client::ECollideLayer::ENEMY_LEFTSHOULDER:
		case Client::ECollideLayer::ENEMY_RIGHTSHOULDER:
			return true;
		default:
			return false;
		}
	}
	inline bool Is_EnemySkillLayer(ECollideLayer eLayer)
	{
		if (eLayer == Client::ECollideLayer::ENEMY_SKILL)
			return true;

		return false;
	}

	inline constexpr unsigned int g_iCollideLayer_Count = static_cast<unsigned int>(ECollideLayer::END);
	
	//===================
	// HitType
	//===================
	enum class EHitType : unsigned int
	{
		BLASTED = 0,
		FRONT,
		LEFT,
		RIGHT,
		ELCTRICKSHOCK,
		SPINBLOWOFF,
		SPINBLOWUP,
		ONEHITDOWNLONG,
		ONEHITDOWNSHORT,
		HITDOWNSHORT,
		HITDOWNLONG,
		WATERPRISON,
		EARTHQUAKE,
		UNIVERSALPULL,
		ELECTRICKSHOCK,
		PAIN_SPECIALSKILL,
		END
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

	enum class ELevelType : unsigned int
	{
		STATIC = 0,
		LOADING,
		LOGO,
		END
	};
	inline constexpr size_t g_iLevelType_Count = static_cast<size_t>(ELevelType::END);

	typedef struct tagColMeshHitInformation
	{
		class CColMesh* pColMesh = { nullptr };
		_float3 vHitPosition = { 0.f, 0.f, 0.f };
		_float3 vNormal = { 0.f, 1.f, 0.f };
		_float fDistance = D3D11_FLOAT32_MAX;
		_int iTriangleIndex = -1;
		ESurfaceType eSurfaceType = ESurfaceType::NONE;
	}COLMESH_HITINFO;

	typedef struct tagWallSpaceInformation
	{
		_float3 vCurrentUp = { 0.f, 1.f, 0.f };
		_float3 vCurrentRight = { 1.f, 0.f, 0.f };
		_float3 vCurrentLook = { 0.f, 0.f, 1.f };

		_float3 vTargetUp = { 0.f, 1.f, 0.f };
		_float3 vTargetRight = { 1.f, 0.f, 0.f };
		_float3 vTargetLook = { 0.f, 0.f, 1.f };

		_float fAttachAlpha = { 0.f };
	}WALLSPACE_INFO;

	typedef struct tagRopeInformation
	{
		_int iTriangleIndex = -1;
		_float fMaxRopeLength = D3D11_FLOAT32_MAX;
		class CColMesh* pColMesh = { nullptr };
		_float3 vHitPosition = { 0.f, 0.f, 0.f };
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

	inline constexpr wchar_t g_wszTriggerBoxLayer[]{ L"TriggerBox_Layer" };
	inline constexpr wchar_t g_wszColMeshLayer[]{ L"ColMesh_Layer" };
	inline constexpr wchar_t g_wszStaticModelLayer[]{ L"StaticModel_Layer" };
	inline constexpr wchar_t g_wszBossLayer[]{ L"Boss_Layer" };
	inline constexpr wchar_t g_wszPlayerLayer[]{ L"Player_Layer" };
	inline constexpr wchar_t g_wszDynamicCameraLayer[]{ L"DynamicCamera_Layer" };
	inline constexpr wchar_t g_wszUILayer[]{ L"UI_Layer" };
	inline constexpr wchar_t g_wszSkillLayer[]{ L"Skill_Layer" };
	inline constexpr wchar_t g_wszEffectLayer[]{ L"Effect_Layer" };
}

using namespace Client;