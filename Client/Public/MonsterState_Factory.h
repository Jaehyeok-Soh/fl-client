#pragma once
#include "Base.h"
#include "DTO_MonsterState.h"

NS_BEGIN(Client)

using ConditionFunc = std::function<_bool(class CStateBase_Monster*, const DTO::STATE_PARAM&)>;
using FeatureFunc = std::function<void(class CStateBase_Monster*, const _float&, const DTO::STATE_PARAM&)>;

class CMonsterState_Factory final : public CBase
{
	DECLARE_SINGLETON(CMonsterState_Factory)

	using Super = CBase;
private:
	CMonsterState_Factory();
	virtual ~CMonsterState_Factory() = default;

	HRESULT Ready_Condition();
	HRESULT Ready_Feature();
public:
	HRESULT Initialize();

	HRESULT					RegisterCondition(string name, ConditionFunc func);
	ConditionFunc			GetCondition(string name);

	HRESULT					RegisterFeature(string name, FeatureFunc func);
	FeatureFunc				GetFeature(string name);
private:
	static class CMonsterActionState* GetActionState(class CStateBase_Monster* state);
	static class CMonsterControlContext* GetControlContext(class CStateBase_Monster* state);

private:
	map<string, ConditionFunc> m_mapCondition;
	map<string, FeatureFunc> m_mapFeature;
public:
	virtual void Free() override;
};

NS_END