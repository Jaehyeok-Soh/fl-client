#pragma once
#include <string>
#include <set>
#include <map>
#include <vector>

#include "Engine_Struct.h"

namespace DTO
{
	typedef struct tagMonsterTimeCounter
	{
		float fTimeAcc = { 0.f };
		float fMaxTime = { 0.f };
		float fMinTime = { 0.f };

		bool bCountTime = { true };
		bool bTimeReset = { true };
	}MONSTERTIME_COUNTER;

	inline void to_json(json& j, const MONSTERTIME_COUNTER& d)
	{
		j["fTimeAcc"] = d.fTimeAcc;
		j["fMaxTime"] = d.fMaxTime;
		j["fMinTime"] = d.fMinTime;
		j["bCountTime"] = d.bCountTime;
		j["bTimeReset"] = d.bTimeReset;
	}

	inline void from_json(const json& j, MONSTERTIME_COUNTER& d)
	{
		j.at("fTimeAcc").get_to(d.fTimeAcc);
		j.at("fMaxTime").get_to(d.fMaxTime);
		j.at("fMinTime").get_to(d.fMinTime);
		j.at("bCountTime").get_to(d.bCountTime);
		j.at("bTimeReset").get_to(d.bTimeReset);
	}

	typedef struct tagStateTransition
	{
		vector<string> vecCondition;
		// state name, weight
		map<string, float> mapRandomStatePool;

		vector<int> vecConditionIdx;
		map<int, float> mapRandomStatePoolIdx;
		float fTotalWeight;
	}STATE_TRANSITION;

	inline void to_json(json& j, const STATE_TRANSITION& d)
	{
		j["vecCondition"] = d.vecCondition;
		j["mapRandomStatePool"] = d.mapRandomStatePool;
	}

	inline void from_json(const json& j, STATE_TRANSITION& d)
	{
		j.at("vecCondition").get_to(d.vecCondition);
		j.at("mapRandomStatePool").get_to(d.mapRandomStatePool);
	}

	typedef struct tagMonsterStateBaseDesc
	{
		string strName = { "Sample" };

		int iStateIdx = {};

		bool bIsBoss = { false };
		bool bIsCombo = { false };

		unsigned int FAniFlags = { 0 };
		bool bBlend = { false };
		bool bLoop = { false };

		MONSTERTIME_COUNTER tStateLifeTime = {};
		MONSTERTIME_COUNTER tStateCoolDownTime = {};

		// state names
		std::set<string>		setStates;

		// state name, anim name
		map<string, string>		mapPreAnimNames;
		vector<string>			vecMainAnimNames;

		// condition(function), to state name
		vector<STATE_TRANSITION> vecGlobalStateTransition;

		// condition(function), to state name
		vector<STATE_TRANSITION> vecStateTransition;

		vector<string> vecFeature;
		vector<int> vecFeatureIdx;
	}MONSTER_STATEBASE_DESC;

	inline void to_json(json& j, const MONSTER_STATEBASE_DESC& d)
	{
		j["strName"] = d.strName;

		j["bIsBoss"] = d.bIsBoss;
		j["bIsCombo"] = d.bIsCombo;

		j["FAniFlags"] = d.FAniFlags;
		j["bBlend"] = d.bBlend;
		j["bLoop"] = d.bLoop;

		j["tStateLifeTime"] = d.tStateLifeTime;
		j["tStateCoolDownTime"] = d.tStateCoolDownTime;

		j["setStates"] = d.setStates;

		j["mapPreAnimNames"] = d.mapPreAnimNames;
		j["vecMainAnimNames"] = d.vecMainAnimNames;

		j["vecGlobalStateTransition"] = d.vecGlobalStateTransition;

		j["vecStateTransition"] = d.vecStateTransition;

		j["vecFeature"] = d.vecFeature;
	}

	inline void from_json(const json& j, MONSTER_STATEBASE_DESC& d)
	{
		j.at("strName").get_to(d.strName);

		j.at("bIsBoss").get_to(d.bIsBoss);
		j.at("bIsCombo").get_to(d.bIsCombo);

		j.at("FAniFlags").get_to(d.FAniFlags);
		j.at("bBlend").get_to(d.bBlend);
		j.at("bLoop").get_to(d.bLoop);

		j.at("tStateLifeTime").get_to(d.tStateLifeTime);
		j.at("tStateCoolDownTime").get_to(d.tStateCoolDownTime);

		j.at("setStates").get_to(d.setStates);

		j.at("mapPreAnimNames").get_to(d.mapPreAnimNames);
		j.at("vecMainAnimNames").get_to(d.vecMainAnimNames);

		if (j.contains("vecGlobalStateTransition"))
			j.at("vecGlobalStateTransition").get_to(d.vecGlobalStateTransition);
		else
			d.vecGlobalStateTransition = vector<STATE_TRANSITION>();

		j.at("vecStateTransition").get_to(d.vecStateTransition);

		if (j.contains("vecFeature"))
			j.at("vecFeature").get_to(d.vecFeature);
		else
			d.vecFeature = vector<string>();
	}

	typedef struct tagMonsterStateDesc
	{
		string strTag = { "Sample" };

		// state names
		std::set<string>		setStates;

		vector<MONSTER_STATEBASE_DESC> vecMonsterStateDesc;
	}MONSTERSTATE_DESC;

	inline void to_json(json& j, const MONSTERSTATE_DESC& d)
	{
		j["strTag"] = d.strTag;
		j["setStates"] = d.setStates;
		j["vecMonsterStateDesc"] = d.vecMonsterStateDesc;
	}

	inline void from_json(const json& j, MONSTERSTATE_DESC& d)
	{
		j.at("strTag").get_to(d.strTag);
		j.at("setStates").get_to(d.setStates);
		j.at("vecMonsterStateDesc").get_to(d.vecMonsterStateDesc);
	}
}

