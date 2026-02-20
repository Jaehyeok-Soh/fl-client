#pragma once
#include <string>

#include "Engine_Enum.h"

namespace DTO
{
	typedef struct tagAnimEventBase1
	{
		EAnimEvent::Enum eEventType = EAnimEvent::NONE;
		string strDescription = {};
		string strAnimTag = {};
		unsigned int iAnimIndex = {};
		float fStartTrackPosition = {};
	}ANIM_EVENT_BASE1;

	inline void to_json(json& j, const ANIM_EVENT_BASE1& d)
	{
		j["eEventType"] = d.eEventType;
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
		j.at("strDescription").get_to(d.strDescription);
		if (j.contains("strAnimTag"))
			j.at("strAnimTag").get_to(d.strAnimTag);
		else
			d.strAnimTag = "";
		j.at("iAnimIndex").get_to(d.iAnimIndex);
		j.at("fStartTrackPosition").get_to(d.fStartTrackPosition);
	}
}