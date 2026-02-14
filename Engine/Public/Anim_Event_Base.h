#pragma once
#include <string>

#include "Engine_Enum.h"

namespace DTO
{
	typedef struct tagAnimEventBase
	{
		AnimEvent::Enum eEventType = AnimEvent::NONE;
		string strDescription = {};
		string strAnimTag = {};
		unsigned int iAnimIndex = {};
		float fStartTrackPosition = {};
	}ANIM_EVENT_BASE;

	inline void to_json(json& j, const ANIM_EVENT_BASE& d)
	{
		j["eEventType"] = d.eEventType;
		j["strDescription"] = d.strDescription;
		j["strAnimTag"] = d.strAnimTag;
		j["iAnimIndex"] = d.iAnimIndex;
		j["fStartTrackPosition"] = d.fStartTrackPosition;
	}

	inline void from_json(const json& j, ANIM_EVENT_BASE& d)
	{
		j.at("eEventType").get_to(d.eEventType);
		j.at("strDescription").get_to(d.strDescription);
		j.at("strAnimTag").get_to(d.strAnimTag);
		j.at("iAnimIndex").get_to(d.iAnimIndex);
		j.at("fStartTrackPosition").get_to(d.fStartTrackPosition);
	}
}