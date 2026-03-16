#pragma once
#include "json_forward.h"
#include <set>
#include "Engine_Utils.h"

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
			AREA_EXIT,
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

		int iEnterDialogueId = { -1 };
		int iExitDialogueId = { -1 };
		int iInteractDialogueId = { -1 };
	}QUESTDESC;

	inline void to_json(json& j, const QUESTDESC& d)
	{
		j["iId"] = d.iId;
		j["iPrevId"] = d.iPrevId;
		j["iNextId"] = d.iNextId;
		j["iParentId"] = d.iParentId;
		j["wstrTitle"] = Engine_Utils::ToString(d.wstrTitle);
		j["wstrSubTitle"] = Engine_Utils::ToString(d.wstrSubTitle);
		j["wstrExplain"] = Engine_Utils::ToString(d.wstrExplain);
		j["wstrDescription"] = Engine_Utils::ToString(d.wstrDescription);
		j["iEnterDialogueId"] = d.iEnterDialogueId;
		j["iExitDialogueId"] = d.iExitDialogueId;
		j["iInteractDialogueId"] = d.iInteractDialogueId;
	}

	inline void from_json(const json& j, QUESTDESC& d)
	{
		j.at("iId").get_to(d.iId);
		j.at("iPrevId").get_to(d.iPrevId);
		j.at("iNextId").get_to(d.iNextId);
		j.at("iParentId").get_to(d.iParentId);

		string strBuffer = {};
		j.at("wstrTitle").get_to(strBuffer);
		d.wstrTitle = Engine_Utils::ToWString(strBuffer);

		j.at("wstrSubTitle").get_to(strBuffer);
		d.wstrSubTitle = Engine_Utils::ToWString(strBuffer);

		j.at("wstrExplain").get_to(strBuffer);
		d.wstrExplain = Engine_Utils::ToWString(strBuffer);

		j.at("wstrDescription").get_to(strBuffer);
		d.wstrDescription = Engine_Utils::ToWString(strBuffer);

		if (j.contains("iEnterDialogueId"))
			j.at("iEnterDialogueId").get_to(d.iEnterDialogueId);
		else
			d.iEnterDialogueId = -1;

		if (j.contains("iExitDialogueId"))
			j.at("iExitDialogueId").get_to(d.iExitDialogueId);
		else
			d.iExitDialogueId = -1;

		if (j.contains("iInteractDialogueId"))
			j.at("iInteractDialogueId").get_to(d.iInteractDialogueId);
		else
			d.iInteractDialogueId = -1;
	}

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

	inline void to_json(json& j, const QUEST_CHAPTERDESC& d)
	{
		j["eEvent"] = d.eEvent;
		j["eTargetType"] = d.eTargetType;
		j["iCount"] = d.iCount;
		j["tQuestDesc"] = d.tQuestDesc;
	}

	inline void from_json(const json& j, QUEST_CHAPTERDESC& d)
	{
		j.at("eEvent").get_to(d.eEvent);
		j.at("eTargetType").get_to(d.eTargetType);
		j.at("iCount").get_to(d.iCount);
		j.at("tQuestDesc").get_to(d.tQuestDesc);
	}

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