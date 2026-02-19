#pragma once
#include "Anim_Event_AttackOverlap.h"
#include <vector>
#include <string>

namespace DTO
{
	typedef struct tagAttackOverlapDesc
	{
		string strTag = { "Sample" };
		unsigned int iNumPool = {};
		vector<DTO::ATTACKEVENT> attackEvents;
	}ATTACKOVERLAP_DESC;

	inline void to_json(json& j, const ATTACKOVERLAP_DESC& d)
	{
		j["strTag"] = d.strTag;
		j["iNumPool"] = d.iNumPool;
		j["attackEvents"] = d.attackEvents;
	}

	inline void from_json(const json& j, ATTACKOVERLAP_DESC& d)
	{
		j.at("strTag").get_to(d.strTag);
		j.at("iNumPool").get_to(d.iNumPool);
		j.at("attackEvents").get_to(d.attackEvents);
	}
}
