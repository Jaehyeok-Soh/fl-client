#pragma once

#include <string>

namespace DTO
{
	typedef struct tagAnimEventScriptBase
	{
		string strName = {};
		float fDuration = {};
		float Get_Duration() { return fDuration; }
	}ANIM_EVENT_SCRIPT_BASE;

	inline void to_json(json& j, const ANIM_EVENT_SCRIPT_BASE& d)
	{
		j["strName"] = d.strName;
		j["fDuration"] = d.fDuration;
	}

	inline void from_json(const json& j, ANIM_EVENT_SCRIPT_BASE& d)
	{
		j.at("strName").get_to(d.strName);
		j.at("fDuration").get_to(d.fDuration);
	}
}