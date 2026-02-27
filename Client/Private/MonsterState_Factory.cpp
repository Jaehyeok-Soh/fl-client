#include "pch.h"
#include "MonsterState_Factory.h"

#include "GameObject.h"

#include "StateBase_Monster.h"
#include "ActionState.h"
#include "ControlContext.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"

IMPLEMENT_SINGLETON(CMonsterState_Factory)

#define REGISTER_CONDITION(NAME, FUNC) if(FAILED(RegisterCondition(NAME, FUNC))) return E_FAIL;
#define REGISTER_FEATURE(NAME, FUNC)   if(FAILED(RegisterFeature(NAME, FUNC))) return E_FAIL;

#define MONSTERACTIONSTATE(STATE) GetActionState(STATE)
#define MONSTERCC(STATE)          GetControlContext(STATE)

// 시그니처 통일
#define CONDITION [](class CStateBase_Monster* state, const DTO::STATE_PARAM& param)->_bool
#define FEATURE   [](class CStateBase_Monster* state, const _float& fTimeDelta, const DTO::STATE_PARAM& param)

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

	REGISTER_CONDITION("condition_target_ahead", CONDITION{ return MONSTERCC(state)->IsTargetAhead(); });

	REGISTER_CONDITION("condition_target_behind", CONDITION{ return MONSTERCC(state)->IsTargetBehind(); });

	REGISTER_CONDITION("condition_target_side", CONDITION{ return MONSTERCC(state)->IsTargetSide(); });

	REGISTER_CONDITION("condition_target_close", CONDITION{ return MONSTERCC(state)->IsTargetClose(); });

	REGISTER_CONDITION("condition_target_lost", CONDITION{ return MONSTERCC(state)->IsTargetLost(); });
	
	REGISTER_CONDITION("condition_animation_finish", CONDITION{ return state->Is_MainAnimFinished(); });
	
	REGISTER_CONDITION("condition_die", CONDITION{ return state->Get_OwnerObject()->IsDead(); });

	REGISTER_CONDITION("condition_hit_light", CONDITION{ return false; /*hit light*/ });

	REGISTER_CONDITION("condition_hit_heavy", CONDITION{ return false; /*hit heady*/ });

	REGISTER_CONDITION("condition_hit_additive", CONDITION{ return false; /*hit additive*/ });

	REGISTER_CONDITION("condition_loop_animation", CONDITION{ return state->IsLoop(); });

	REGISTER_CONDITION("condition_cancellation", CONDITION{ return state->IsCancellation(); });

	REGISTER_CONDITION("condition_non_cancellation", CONDITION{ return !state->IsCancellation(); });

	REGISTER_CONDITION("condition_over_lifetime", CONDITION{ return state->IsOverLifeTime(); });


	// 02-27 구조 변경 후 예시
	REGISTER_CONDITION("param_condition_distance_over", CONDITION{ return MONSTERCC(state)->IsTargetDistanceOver(param.fParam[0]); });
	return S_OK;
}

HRESULT CMonsterState_Factory::Ready_Feature()
{
	REGISTER_FEATURE("feat_walk", FEATURE{ MONSTERCC(state)->UpdateWalk(fTimeDelta); state->Align_Movement(fTimeDelta); });

	REGISTER_FEATURE("feat_chase", FEATURE{ MONSTERCC(state)->UpdateChase(fTimeDelta); state->Align_Movement(fTimeDelta); });

	REGISTER_FEATURE("feat_align_attack", FEATURE{ MONSTERCC(state)->UpdateChase(fTimeDelta); state->Turn_byCam(fTimeDelta); });

	REGISTER_FEATURE("feat_keep_look_target", FEATURE{ state->SetupLook_Target_XZ(); });

	// 8방향 움직임
	REGISTER_FEATURE("feat_move_front", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 1.f, 0.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_right", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 0.f, 1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_left", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 0.f, -1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_backward", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, -1.f, 0.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_front_right", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 1.f, 1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_front_left", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 1.f, -1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_backward_right", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, -1.f, 1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_backward_left", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, -1.f, -1.f); state->Align_Movement_MoveDir(fTimeDelta); });

	// 02-27 구조 변경 후 예시
	REGISTER_FEATURE("param_feat_move_local", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, param.fParam[0], param.fParam[1]); state->Align_Movement(fTimeDelta); });
	return S_OK;
}

HRESULT CMonsterState_Factory::RegisterCondition(string name, ConditionFunc func)
{
	auto iter = m_mapCondition.find(name);
	if (iter != m_mapCondition.end())
		return E_FAIL;

	m_mapCondition.emplace(name, func);

	return S_OK;
}

ConditionFunc CMonsterState_Factory::GetCondition(string name)
{
	auto iter = m_mapCondition.find(name);
	if (iter == m_mapCondition.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CMonsterState_Factory::RegisterFeature(string name, FeatureFunc func)
{
	auto iter = m_mapFeature.find(name);
	if (iter != m_mapFeature.end())
		return E_FAIL;

	m_mapFeature.emplace(name, func);

	return S_OK;
}

FeatureFunc CMonsterState_Factory::GetFeature(string name)
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
	m_mapCondition.clear();
	m_mapFeature.clear();
}
