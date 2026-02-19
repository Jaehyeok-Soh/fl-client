#pragma once
#include <vector>

#include "Anim_Event_AttackOverlap.h"

namespace DTO
{
	typedef struct tagAnimEventInfo1
	{
		/// <summary>
		/// 애니메이션 주체 이름
		/// 모델 폴더 이름이 적합
		/// 아니면 데이터 리소스 폴더 이름
		/// </summary>
		string strOwnerTag = {};

		vector<ATTACKEVENT> vecAttackEvents;
		vector<ANIM_EVENT_BASE1> vecVFXEvents;
		//vector<ANIM_EVENT_BASE> vecSoundEvents;
		//vector<ANIM_EVENT_BASE> vecCameraEvents;
	}ANIM_EVENT_INFO1;

	inline void to_json(json& j, const ANIM_EVENT_INFO1& d)
	{
		j["strOwnerTag"] = d.strOwnerTag;
		j["vecAttackEvents"] = d.vecAttackEvents;
		j["vecVFXEvents"] = d.vecVFXEvents;
	}

	inline void from_json(const json& j, ANIM_EVENT_INFO1& d)
	{
		j.at("strOwnerTag").get_to(d.strOwnerTag);
		j.at("vecAttackEvents").get_to(d.vecAttackEvents);
		j.at("vecVFXEvents").get_to(d.vecVFXEvents);
	}
}
