#include "pch.h"
#include "MonsterState_Factory.h"

#include "StateBase_Monster.h"
#include "ActionState.h"
#include "ControlContext.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"

IMPLEMENT_SINGLETON(CMonsterState_Factory)

#define REGISTER_CONDITION(NAME, FUNC) if(FAILED(RegisterCondition(NAME, FUNC))) return E_FAIL;
#define REGISTER_FEATURE(NAME, FUNC) if(FAILED(RegisterFeature(NAME, FUNC))) return E_FAIL;

#define MONSTERACTIONSTATE(STATE) GetActionState(STATE)
#define MONSTERCC(STATE) GetControlContext(STATE)

#define CONDITION [](class CStateBase_Monster* state)->_bool
#define FEATURE [](class CStateBase_Monster* state, const _float& fTimeDelta)

CMonsterState_Factory::CMonsterState_Factory()
	: Super()
{
}

HRESULT CMonsterState_Factory::Initialize()
{
	if (FAILED(Ready_Condition()))
		return E_FAIL;

	if (FAILED(Ready_Feature()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonsterState_Factory::Ready_Condition()
{
	REGISTER_CONDITION("condition_target_encount", CONDITION{ return MONSTERCC(state)->IsTargetFound(); });

	REGISTER_CONDITION("condition_in_melee_range", CONDITION{ return MONSTERCC(state)->IsTargetInMeleeRange(); });

	REGISTER_CONDITION("condition_in_attack_range", CONDITION{ return MONSTERCC(state)->IsTargetInAttackRange(); });

	REGISTER_CONDITION("condition_target_out_of_melee_attack", CONDITION{ return MONSTERCC(state)->IsTargetOutOfMeleeRange(); });

	REGISTER_CONDITION("condition_target_out_of_range_attack", CONDITION{ return MONSTERCC(state)->IsTargetOutOfAttackRange(); });

	REGISTER_CONDITION("condition_skill_enable", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_target_out_of_skill_range", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_target_ahead", CONDITION{ return MONSTERCC(state)->IsTargetAhead(); });

	REGISTER_CONDITION("condition_target_behind", CONDITION{ return MONSTERCC(state)->IsTargetBehind(); });

	REGISTER_CONDITION("condition_target_side", CONDITION{ return MONSTERCC(state)->IsTargetSide(); });

	REGISTER_CONDITION("condition_target_close", CONDITION{ return MONSTERCC(state)->IsTargetClose(); });

	REGISTER_CONDITION("condition_damage_recently", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_airborne", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_down", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_minTime_satisfy", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_maxTime_satisfy", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_falling", [](class CStateBase_Monster* state)->_bool {

		});

	REGISTER_CONDITION("condition_target_lost", CONDITION{ return MONSTERCC(state)->IsTargetLost(); });

	return S_OK;
}

HRESULT CMonsterState_Factory::Ready_Feature()
{
	REGISTER_FEATURE("feat_find_target", [](class CStateBase_Monster* state, const _float& fTimeDelta) {
		// 가장 가까운 놈 찾기
		});

	REGISTER_FEATURE("feat_walk", FEATURE{ return MONSTERCC(state)->UpdateWalk(fTimeDelta); state->Align_Movement(fTimeDelta); });

	REGISTER_FEATURE("feat_chase", FEATURE{ return MONSTERCC(state)->UpdateChase(fTimeDelta); state->Align_Movement(fTimeDelta); });

	REGISTER_FEATURE("feat_walk_back", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_run", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_sidewalk_right", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_sidewalk_right", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_sidewalk_left", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_circleWalk_right", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_circleWalk_left", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_jump", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_fly", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	REGISTER_FEATURE("feat_skill", [](class CStateBase_Monster* state, const _float& fTimeDelta) {

		});

	return S_OK;
}

HRESULT CMonsterState_Factory::RegisterCondition(string name, std::function<_bool(class CStateBase_Monster*)> func)
{
	auto iter = m_mapCondition.find(name);
	if (iter != m_mapCondition.end())
		return E_FAIL;

	m_mapCondition.emplace(name, func);

	return S_OK;
}

std::function<_bool(class CStateBase_Monster*)> CMonsterState_Factory::GetCondition(string name)
{
	auto iter = m_mapCondition.find(name);
	if (iter == m_mapCondition.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CMonsterState_Factory::RegisterFeature(string name, std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> func)
{
	auto iter = m_mapFeature.find(name);
	if (iter != m_mapFeature.end())
		return E_FAIL;

	m_mapFeature.emplace(name, func);

	return S_OK;
}

std::function<void(class CStateBase_Monster*, const _float& fTimeDelta)> CMonsterState_Factory::GetFeature(string name)
{
	auto iter = m_mapFeature.find(name);
	if (iter == m_mapFeature.end())
		return nullptr;

	return (*iter).second;
}

CMonsterActionState* CMonsterState_Factory::GetActionState(CStateBase_Monster* state)
{
	return static_cast<CMonsterActionState*>(state->m_pOwnerStateComp);
}

CMonsterControlContext* CMonsterState_Factory::GetControlContext(CStateBase_Monster* state)
{
	return static_cast<CMonsterControlContext*>(MONSTERACTIONSTATE(state)->GetOwnerControlContext());
}

void CMonsterState_Factory::Free()
{
	for (auto& cond : m_mapCondition)
		Safe_Delete(cond.second);
	m_mapCondition.clear();

	for (auto& feat : m_mapFeature)
		Safe_Delete(feat.second);
	m_mapFeature.clear();
}
