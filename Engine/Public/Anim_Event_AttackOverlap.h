#pragma once
#include "Anim_Event_Base.h"
#include "Anim_Event_Hitbox.h"

namespace DTO
{
	typedef struct tagAttackEvent : public ANIM_EVENT_BASE
	{
		HITBOX_DESC tHitboxDesc;

		/////////////////////
		// ANIM_EVENT_BASE //
		/////////////////////
		//AnimEvent::Enum eEventType = AnimEvent::NONE;
		//string strDescription = {};
		//string strAnimTag = {};
		//unsigned int iAnimIndex = {};
		//float fStartTrackPosition = {};
	}ATTACKEVENT;

	inline void to_json(json& j, const ATTACKEVENT& d)
	{
		j["tHitboxDesc"] = d.tHitboxDesc;
	}

	inline void from_json(const json& j, ATTACKEVENT& d)
	{
		j.at("tHitboxDesc").get_to(d.tHitboxDesc);
	}
}