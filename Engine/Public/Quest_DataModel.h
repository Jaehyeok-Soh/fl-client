#pragma once
#include "json_forward.h"
#include <set>

namespace DTO
{
	// Äù½ºÆ® Á¤º¸
	typedef struct EQuestEvent
	{
		enum Enum
		{
			MONSTER_KILL,
			NPC_TALK,
			AREA_ENTER,
			OBJECT_INTERACT
		};
	}QUESTEVENT;

	typedef struct EQuestLayer
	{
		enum Enum
		{
			SCENARIO,
			CHAPTER
		};
	}QUESTLAYER;

	typedef struct EQuestState
	{
		enum Enum
		{
			LOCKED,
			AVAILABLE,
			IN_PROGRESS,
			COMPLETE
		};
	}QUESTSTATE;

	typedef struct tagQuestDesc
	{
		QUESTLAYER::Enum eType = QUESTLAYER::SCENARIO;
		QUESTSTATE::Enum eState = QUESTSTATE::LOCKED;
		int iId = { -1 };
		int iPrevId = { -1 };
		int iNextId = { -1 };

		int iParentId = { -1 };

		wstring wstrTitle = { L"" };
		wstring wstrSubTitle = { L"" };
		wstring wstrExplain = { L"" };

		wstring wstrDescription = { L"" };
	}QUESTDESC;

	inline void to_json(json& j, const QUESTDESC& d);
	inline void from_json(const json& j, QUESTDESC& d);

	typedef struct tagQuestChapterDesc
	{
		QUESTEVENT::Enum eEvent = QUESTEVENT::AREA_ENTER;
		std::set<OBJECT_ENUM_TAG::Enum> eTargetType; //OBJECT_ENUM_TAG::Enum
		//MinMax MTargetTypeRange = {};
		int iCount = {};

		QUESTDESC tQuestDesc = {};

		unsigned long long iObjectId = {};
		class CGameObject* pObject = { nullptr };
		wstring wstrObjectLayer = {};
		DirectX::SimpleMath::Vector3 vObjectPosition = {};

		int iCurrentCount = {};
	}QUEST_CHAPTERDESC;

	inline void to_json(json& j, const QUEST_CHAPTERDESC& d);
	inline void from_json(const json& j, QUEST_CHAPTERDESC& d);

	typedef struct tagQuestEventSignature
	{
		QUESTEVENT::Enum eEvent = QUESTEVENT::AREA_ENTER;
		OBJECT_ENUM_TAG::Enum eTargetType; //OBJECT_ENUM_TAG::Enum
		int iCount = { 0 };
	}QUEST_EVENT_SIGNATURE;

	typedef struct tagQuestInfoBucket
	{
		QUESTDESC tScenarioInfo = {};
		QUEST_CHAPTERDESC tChapterInfo = {};
	}QUEST_INFOBUCKET;
}