#pragma once
#include <string>
#include "json_forward.h"
#include "Engine_Enum.h"

NS_BEGIN(Engine)
	NLOHMANN_JSON_SERIALIZE_ENUM(EAnimNotifyPhase,
		{
			{EAnimNotifyPhase::Immediatley, "Immediatley"},
			{EAnimNotifyPhase::Late, "Late"},
			{EAnimNotifyPhase::PreRender, "PreRender"},
			{EAnimNotifyPhase::END, "END"},
		}
	)
NS_END

NS_BEGIN(DTO)
	typedef struct tagAnimEventBase1
	{
		EAnimEvent::Enum eEventType = EAnimEvent::NONE;
		Engine::EAnimNotifyPhase ePhase{ Engine::EAnimNotifyPhase::Immediatley };
		string strDescription = {};
		string strAnimTag = {};
		unsigned int iAnimIndex = {};
		float fStartTrackPosition = {};
	}ANIM_EVENT_BASE1;

	inline void to_json(json& j, const ANIM_EVENT_BASE1& d)
	{
		j["eEventType"] = d.eEventType;
		j["ePhase"] = d.ePhase;
		j["strDescription"] = d.strDescription;
		j["strAnimTag"] = d.strAnimTag;
		j["iAnimIndex"] = d.iAnimIndex;
		j["fStartTrackPosition"] = d.fStartTrackPosition;

	}

	inline void from_json(const json& j, ANIM_EVENT_BASE1& d)
	{
		if (j.contains("eEventType"))
			j.at("eEventType").get_to(d.eEventType);
		else
			d.eEventType = EAnimEvent::OVERLAP;
		if (j.contains("ePhase") == true)
			d.ePhase = j["ePhase"].get<Engine::EAnimNotifyPhase>();
		j.at("strDescription").get_to(d.strDescription);
		if (j.contains("strAnimTag"))
			j.at("strAnimTag").get_to(d.strAnimTag);
		else
			d.strAnimTag = "";
		j.at("iAnimIndex").get_to(d.iAnimIndex);
		j.at("fStartTrackPosition").get_to(d.fStartTrackPosition);
	}
NS_END