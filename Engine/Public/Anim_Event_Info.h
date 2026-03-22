#pragma once
#include <vector>
#include "Anim_Event_AttackOverlap.h"
#include "DataStruct_EffectEvent.h"
#include "DataStruct_SoundEvent.h"
 
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
		vector<EFFECTEVENT> vecVFXEvents;
		vector<SOUNDEVENT> vecSoundEvents;
		//vector<ANIM_EVENT_BASE> vecCameraEvents;
	}ANIM_EVENT_INFO1;

	inline void to_json(json& j, const ANIM_EVENT_INFO1& d)
	{
		j["strOwnerTag"] = d.strOwnerTag;
		j["vecAttackEvents"] = d.vecAttackEvents;
		j["vecVFXEvents"] = d.vecVFXEvents;
		j["vecSoundEvents"] = d.vecSoundEvents;
	}

	inline void from_json(const json& j, ANIM_EVENT_INFO1& d)
	{
		j.at("strOwnerTag").get_to(d.strOwnerTag);
		j.at("vecAttackEvents").get_to(d.vecAttackEvents);
		j.at("vecVFXEvents").get_to(d.vecVFXEvents);
		if (j.contains("vecSoundEvents"))
			j.at("vecSoundEvents").get_to(d.vecSoundEvents);
	}
}
