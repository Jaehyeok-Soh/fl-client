#pragma once
#include <vector>

#include "Anim_Event_AttackOverlap.h"

namespace DTO
{
	typedef struct tagAnimEventInfo
	{
		/// <summary>
		/// 애니메이션 주체 이름
		/// 모델 폴더 이름이 적합
		/// 아니면 데이터 리소스 폴더 이름
		/// </summary>
		string strOwnerTag = {};

		vector<ATTACKEVENT> vecAttackEvents;
		vector<ANIM_EVENT_BASE> vecVFXEvents;
		//vector<ANIM_EVENT_BASE> vecSoundEvents;
		//vector<ANIM_EVENT_BASE> vecCameraEvents;
	}ANIM_EVENT_INFO;

	inline void to_json(json& j, const ANIM_EVENT_INFO& d)
	{
		j["strOwnerTag"] = d.strOwnerTag;
		j["vecAttackEvents"] = d.vecAttackEvents;
		j["vecVFXEvents"] = d.vecVFXEvents;
	}

	inline void from_json(const json& j, ANIM_EVENT_INFO& d)
	{
		j.at("strOwnerTag").get_to(d.strOwnerTag);
		j.at("vecAttackEvents").get_to(d.vecAttackEvents);
		j.at("vecVFXEvents").get_to(d.vecVFXEvents);
	}
}
