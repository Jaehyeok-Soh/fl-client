#pragma once
#include <string>
#include <set>
#include <map>
#include <vector>

#include "Engine_Struct.h"

namespace DTO
{
	/////////////////------- StateParam -------/////////////////
	typedef struct tagStateParam
	{
		int		iParam[4]{ 0 };
		float	fParam[4]{ 0.f };
		bool	bParam[4]{ 0 };
	}STATE_PARAM;

	inline void to_json(json& j, const STATE_PARAM& d)
	{
		j["iParam"] = vector<int>(d.iParam, d.iParam + 4);
		j["fParam"] = vector<float>(d.fParam, d.fParam + 4);
		j["bParam"] = vector<bool>(d.bParam, d.bParam + 4);
	}

	inline void from_json(const json& j, STATE_PARAM& d)
	{
		d = {};

		if (j.contains("iParam"))
		{
			auto v = j.at("iParam").get<vector<int>>();
			const size_t n = (std::min<size_t>)(4, v.size());
			for (size_t i = 0; i < n; ++i) d.iParam[i] = v[i];
		}

		if (j.contains("fParam"))
		{
			auto v = j.at("fParam").get<vector<float>>();
			const size_t n = (std::min<size_t>)(4, v.size());
			for (size_t i = 0; i < n; ++i) d.fParam[i] = v[i];
		}

		if (j.contains("bParam"))
		{
			auto v = j.at("bParam").get<vector<bool>>();
			const size_t n = (std::min<size_t>)(4, v.size());
			for (size_t i = 0; i < n; ++i) d.bParam[i] = v[i];
		}
	}

	/////////////////------- Condition Entry -------/////////////////
	typedef struct tagConditionEntry
	{
		string strCondition{ "" };
		STATE_PARAM tParam{};
	}CONDITION_ENTRY;

	inline void to_json(json& j, const CONDITION_ENTRY& d)
	{
		j["strCondition"] = d.strCondition;
		j["tParam"] = d.tParam;
	}

	inline void from_json(const json& j, CONDITION_ENTRY& d)
	{
		j.at("strCondition").get_to(d.strCondition);
		if (j.contains("tParam"))
			j.at("tParam").get_to(d.tParam);
	}

	/////////////////------- ConditionFeature Desc -------/////////////////
	typedef struct tagFeatureEntry
	{
		string strFeature{ "" };
		STATE_PARAM tParam{};
		bool IsOnce = { false };
		bool IsExecuted = { false };
	}FEATURE_ENTRY;

	inline void to_json(json& j, const FEATURE_ENTRY& d)
	{
		j["strFeature"] = d.strFeature;
		j["tParam"] = d.tParam;

		j["IsOnce"] = d.IsOnce;
		j["IsExecuted"] = d.IsExecuted;
	}

	inline void from_json(const json& j, FEATURE_ENTRY& d)
	{
		j.at("strFeature").get_to(d.strFeature);
		if (j.contains("tParam"))
			j.at("tParam").get_to(d.tParam);
		
		if (j.contains("IsOnce"))
			j.at("IsOnce").get_to(d.IsOnce);

		if (j.contains("IsExecuted"))
			j.at("IsExecuted").get_to(d.IsExecuted);
	}


	/////////////////------- ConditionFeature Entry -------/////////////////
	typedef struct tagConditionFeatureEntry
	{
		CONDITION_ENTRY cond{};
		FEATURE_ENTRY   feat{};
	}CONDITIONFEATURE_ENTRY;

	inline void to_json(json& j, const CONDITIONFEATURE_ENTRY& d)
	{
		j["cond"] = d.cond;
		j["feat"] = d.feat;
	}

	inline void from_json(const json& j, CONDITIONFEATURE_ENTRY& d)
	{
		if (j.contains("cond") || j.contains("feat"))
		{
			if (j.contains("cond")) j.at("cond").get_to(d.cond);
			if (j.contains("feat")) j.at("feat").get_to(d.feat);
		}
	}

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
		vector<CONDITION_ENTRY> vecConditionEntry;
		// state name, weight
		map<string, float> mapRandomStatePool;

		vector<int> vecConditionIdx;
		map<int, float> mapRandomStatePoolIdx;
		float fTotalWeight;
	}STATE_TRANSITION;

	inline void to_json(json& j, const STATE_TRANSITION& d)
	{
		j["vecConditionEntry"] = d.vecConditionEntry;
		j["mapRandomStatePool"] = d.mapRandomStatePool;
	}

	inline void from_json(const json& j, STATE_TRANSITION& d)
	{
		if (j.contains("vecConditionEntry"))
		{
			j.at("vecConditionEntry").get_to(d.vecConditionEntry);
		}
		else if (j.contains("vecCondition"))
		{
			auto vec = j.at("vecCondition").get<vector<string>>();
			for (auto& str : vec)
			{
				CONDITION_ENTRY entry{};
				entry.strCondition = str;
				d.vecConditionEntry.push_back(entry);
			}
		}
		j.at("mapRandomStatePool").get_to(d.mapRandomStatePool);
	}

	typedef struct tagMonsterStateBaseDesc
	{
		string strName = { "Sample" };

		int iStateIdx = {};

		bool bIsBoss = { false };
		bool bIsCombo = { false };
		bool bIsCustom = { false };

		unsigned int FAniFlags = { 0 };
		bool bBlend = { false };
		bool bLoop = { false };
		bool bCancellation = { false };
		float fAnimationSpeed = { 1.f };

		MONSTERTIME_COUNTER tStateLifeTime = {};
		MONSTERTIME_COUNTER tStateCoolDownTime = {};

		// state names
		std::set<string>		setStates;

		// state name, anim name, weapon anim name
		map<string, string>		mapPreAnimNames;
		vector<string>			vecMainAnimNames;
		vector<string>			vecWeaponAnimNames;

		// condition(function), to state name
		vector<STATE_TRANSITION> vecGlobalStateTransition;

		// condition(function), to state name
		vector<STATE_TRANSITION> vecStateTransition;

		// condition feature
		vector<CONDITIONFEATURE_ENTRY> vecConditionFeature;

		// start - condition feature
		vector<CONDITIONFEATURE_ENTRY> vecStartConditionFeature;
		// end - condition feature
		vector<CONDITIONFEATURE_ENTRY> vecEndConditionFeature;

		vector<FEATURE_ENTRY> vecFeatureEntry;
		vector<int> vecFeatureIdx;
	}MONSTER_STATEBASE_DESC;

	inline void to_json(json& j, const MONSTER_STATEBASE_DESC& d)
	{
		j["strName"] = d.strName;

		j["bIsBoss"] = d.bIsBoss;
		j["bIsCombo"] = d.bIsCombo;
		j["bIsCustom"] = d.bIsCustom;

		j["FAniFlags"] = d.FAniFlags;
		j["bBlend"] = d.bBlend;
		j["bLoop"] = d.bLoop;
		j["bCancellation"] = d.bCancellation;
		j["fAnimationSpeed"] = d.fAnimationSpeed;

		j["tStateLifeTime"] = d.tStateLifeTime;
		j["tStateCoolDownTime"] = d.tStateCoolDownTime;

		j["setStates"] = d.setStates;

		j["mapPreAnimNames"] = d.mapPreAnimNames;
		j["vecMainAnimNames"] = d.vecMainAnimNames;
		j["vecWeaponAnimNames"] = d.vecWeaponAnimNames;

		j["vecGlobalStateTransition"] = d.vecGlobalStateTransition;

		j["vecStateTransition"] = d.vecStateTransition;

		j["vecConditionFeature"] = d.vecConditionFeature;

		j["vecStartConditionFeature"] = d.vecStartConditionFeature;
		j["vecEndConditionFeature"] = d.vecEndConditionFeature;

		j["vecFeatureEntry"] = d.vecFeatureEntry;
	}

	inline void from_json(const json& j, MONSTER_STATEBASE_DESC& d)
	{
		j.at("strName").get_to(d.strName);

		j.at("bIsBoss").get_to(d.bIsBoss);
		j.at("bIsCombo").get_to(d.bIsCombo);
		if (j.contains("bIsCustom"))
			j.at("bIsCustom").get_to(d.bIsCustom);

		j.at("FAniFlags").get_to(d.FAniFlags);
		j.at("bBlend").get_to(d.bBlend);
		j.at("bLoop").get_to(d.bLoop);

		if (j.contains("bCancellation"))
			j.at("bCancellation").get_to(d.bCancellation);
		else
			d.bCancellation = false;

		if (j.contains("fAnimationSpeed"))
			j.at("fAnimationSpeed").get_to(d.fAnimationSpeed);
		else
			d.fAnimationSpeed = 1.f;

		j.at("tStateLifeTime").get_to(d.tStateLifeTime);
		j.at("tStateCoolDownTime").get_to(d.tStateCoolDownTime);

		j.at("setStates").get_to(d.setStates);

		j.at("mapPreAnimNames").get_to(d.mapPreAnimNames);
		j.at("vecMainAnimNames").get_to(d.vecMainAnimNames);
		if (j.contains("vecWeaponAnimNames"))
			d.vecWeaponAnimNames = j["vecWeaponAnimNames"].get<vector<string>>();

		if (j.contains("vecGlobalStateTransition"))
			j.at("vecGlobalStateTransition").get_to(d.vecGlobalStateTransition);
		else
			d.vecGlobalStateTransition = vector<STATE_TRANSITION>();

		j.at("vecStateTransition").get_to(d.vecStateTransition);

		if (j.contains("vecConditionFeature"))
			j.at("vecConditionFeature").get_to(d.vecConditionFeature);
		else
			d.vecConditionFeature = vector<CONDITIONFEATURE_ENTRY>();

		if (j.contains("vecFeatureEntry"))
		{
			j.at("vecFeatureEntry").get_to(d.vecFeatureEntry);
		}
		else if (j.contains("vecFeature"))
		{
			auto vec = j.at("vecFeature").get<vector<string>>();
			for (auto& str : vec)
			{
				FEATURE_ENTRY entry{};
				entry.strFeature = str;
				d.vecFeatureEntry.push_back(entry);
			}
		}

		if (j.contains("vecStartConditionFeature"))
			j.at("vecStartConditionFeature").get_to(d.vecStartConditionFeature);

		if (j.contains("vecEndConditionFeature"))
			j.at("vecEndConditionFeature").get_to(d.vecEndConditionFeature);
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

