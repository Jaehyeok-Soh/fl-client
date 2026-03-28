#include "pch.h"
#include "MonsterState_Factory.h"
#include "GameObject.h"
#include "StateBase_Monster.h"
#include "StatCom_Boss.h"
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

	REGISTER_CONDITION("condition_target_left90", CONDITION{ return MONSTERCC(state)->IsTargetLeft90(); });

	REGISTER_CONDITION("condition_target_right90", CONDITION{ return MONSTERCC(state)->IsTargetRight90(); });

	REGISTER_CONDITION("condition_target_left180", CONDITION{ return MONSTERCC(state)->IsTargetLeft180(); });

	REGISTER_CONDITION("condition_target_right180", CONDITION{ return MONSTERCC(state)->IsTargetRight180(); });

	REGISTER_CONDITION("condition_target_close", CONDITION{ return MONSTERCC(state)->IsTargetClose(); });

	REGISTER_CONDITION("condition_target_lost", CONDITION{ return MONSTERCC(state)->IsTargetLost(); });
	
	REGISTER_CONDITION("condition_animation_finish", CONDITION{ return state->Is_MainAnimFinished(); });
	
	REGISTER_CONDITION("condition_alive", CONDITION{ return MONSTERCC(state)->IsAlive(); });

	REGISTER_CONDITION("condition_none_alive", CONDITION{ return !MONSTERCC(state)->IsAlive(); });

	REGISTER_CONDITION("condition_dying", CONDITION{ return MONSTERCC(state)->IsDying(); });

	REGISTER_CONDITION("condition_none_dying", CONDITION{ return !MONSTERCC(state)->IsDying(); });
	
	REGISTER_CONDITION("condition_die_process", CONDITION{ return MONSTERCC(state)->IsDieProcess(); });
	
	REGISTER_CONDITION("condition_none_die_process", CONDITION{ return !MONSTERCC(state)->IsDieProcess(); });

	REGISTER_CONDITION("condition_die_pose", CONDITION{ return MONSTERCC(state)->IsDiePose(); });
	
	REGISTER_CONDITION("condition_none_die_pose", CONDITION{ return !MONSTERCC(state)->IsDiePose(); });

	REGISTER_CONDITION("condition_loop_animation", CONDITION{ return state->IsLoop(); });

	REGISTER_CONDITION("condition_cancellation", CONDITION{ return state->IsCancellation(); });

	REGISTER_CONDITION("condition_non_cancellation", CONDITION{ return !state->IsCancellation(); });
	
	REGISTER_CONDITION("condition_hit", CONDITION{ return MONSTERCC(state)->IsHit(); });
	
	REGISTER_CONDITION("condition_hit_additive", CONDITION{ return MONSTERCC(state)->IsHitAdditive(); });

	REGISTER_CONDITION("condition_hit_light", CONDITION{ return MONSTERCC(state)->IsHitLight(); });

	REGISTER_CONDITION("condition_hit_heavy", CONDITION{ return MONSTERCC(state)->IsHitHeavy(); });

	REGISTER_CONDITION("condition_hit_launch", CONDITION{ return MONSTERCC(state)->IsHitLaunch(); });

	REGISTER_CONDITION("condition_hit_knockdown", CONDITION{ return MONSTERCC(state)->IsHitKnockdown(); });

	REGISTER_CONDITION("condition_fall", CONDITION{ return MONSTERCC(state)->IsFalling(); });

	REGISTER_CONDITION("condition_down", CONDITION{ return MONSTERCC(state)->IsDown(); });

	REGISTER_CONDITION("condition_over_lifetime", CONDITION{ return state->IsOverLifeTime(); });

	REGISTER_CONDITION("condition_cooldowntime_satisfy", CONDITION{ return state->IsCooldownTimeSatisfy(); });

	REGISTER_CONDITION("condition_true_always", CONDITION{ return true; });

	REGISTER_CONDITION("condition_normal_groggy_requested", CONDITION{ return MONSTERCC(state)->IsNormalGroggyRequested(); });

	REGISTER_CONDITION("condition_final_groggy_requested", CONDITION{ return MONSTERCC(state)->IsFinalGroggyRequested(); });

	REGISTER_CONDITION("condition_normal_groggy", CONDITION{ return MONSTERCC(state)->IsNormalGroggy(); });

	REGISTER_CONDITION("condition_final_groggy", CONDITION{ return MONSTERCC(state)->IsFinalGroggy(); });

	REGISTER_CONDITION("condition_not_groggy", CONDITION{ return MONSTERCC(state)->IsNotGroggy(); });

	REGISTER_CONDITION("condition_phase_two", CONDITION{ return MONSTERCC(state)->IsPhaseTwo(); });

	REGISTER_CONDITION("param_condition_IsTrackPositionBetween", CONDITION{ return MONSTERACTIONSTATE(state)->Is_AnimTrackPositionBetweenRaw(param.fParam[0], param.fParam[1]); });

	REGISTER_CONDITION("param_condition_IsTrackPositionAt", CONDITION{ return MONSTERACTIONSTATE(state)->Is_AnimTrackPositionAtRaw(param.fParam[0]); });
	
	REGISTER_CONDITION("condition_attack_landed", CONDITION{ return MONSTERCC(state)->IsAttackLanded(); });
	
	REGISTER_CONDITION("condition_attack_none_landed", CONDITION{ return !MONSTERCC(state)->IsAttackLanded(); });
	
	REGISTER_CONDITION("condition_is_ground", CONDITION{ return MONSTERCC(state)->IsGround(); });

	return S_OK;
}

HRESULT CMonsterState_Factory::Ready_Feature()
{
	REGISTER_FEATURE("feat_walk", FEATURE{ MONSTERCC(state)->UpdateWalk(fTimeDelta); state->Align_Movement(fTimeDelta); });

	REGISTER_FEATURE("feat_chase", FEATURE{ MONSTERCC(state)->UpdateChase(fTimeDelta); state->Align_Movement(fTimeDelta); });

	REGISTER_FEATURE("feat_align_attack", FEATURE{ MONSTERCC(state)->UpdateChase(fTimeDelta); state->Turn_byCam(fTimeDelta); });

	REGISTER_FEATURE("feat_keep_look_target", FEATURE{ state->SetupLook_Target_XZ(); });

	REGISTER_FEATURE("feat_TurnToTarget_XZ", FEATURE{ MONSTERCC(state)->Update_TurnToTarget_XZ(fTimeDelta); });
	REGISTER_FEATURE("feat_TurnToTarget_XZ_Ratio", FEATURE{ MONSTERCC(state)->Update_TurnToTarget_XZ(fTimeDelta * param.fParam[0]); });

	REGISTER_FEATURE("feat_consume_groggy", FEATURE{ MONSTERCC(state)->Consume_GroggyRequest(); });
	REGISTER_FEATURE("feat_end_groggy", FEATURE{ MONSTERCC(state)->End_Groggy(); });

	// 8방향 움직임
	REGISTER_FEATURE("feat_move_front", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 1.f, 0.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_right", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 0.f, 1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_left", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 0.f, -1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_backward", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, -1.f, 0.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_front_right", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 1.f, 1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_front_left", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, 1.f, -1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_backward_right", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, -1.f, 1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	REGISTER_FEATURE("feat_move_backward_left", FEATURE{ MONSTERCC(state)->Update_8Dir_LocalAxisXZ(fTimeDelta, -1.f, -1.f); state->Align_Movement_MoveDir(fTimeDelta); });
	
	REGISTER_FEATURE("feat_set_die_process", FEATURE{ MONSTERCC(state)->Set_DieProcess(); });
	
	REGISTER_FEATURE("feat_set_die_pose", FEATURE{ MONSTERCC(state)->Set_DiePose(); });
	REGISTER_FEATURE("feat_set_dead", FEATURE{ state->Get_OwnerObject()->Set_Dead(); });
	
	REGISTER_FEATURE("feat_set_cct_collision_enable", FEATURE{ MONSTERCC(state)->Set_CCT_Collision_Enable(); });
	REGISTER_FEATURE("feat_set_cct_collision_disable", FEATURE{ MONSTERCC(state)->Set_CCT_Collision_Disable(); });

	REGISTER_FEATURE("feat_set_on_ragdoll", FEATURE{ MONSTERCC(state)->Set_On_Ragdoll(); });
	REGISTER_FEATURE("feat_set_off_ragdoll", FEATURE{ MONSTERCC(state)->Set_Off_Ragdoll(); });

	// 가속도 관련
	REGISTER_FEATURE("feat_set_zero_velocity", FEATURE{ MONSTERACTIONSTATE(state)->Set_ZeroVelocity(); });
	REGISTER_FEATURE("feat_set_zero_horizontal_velocity", FEATURE{ MONSTERACTIONSTATE(state)->Set_ZeroHorizontalVelocity(); });
	REGISTER_FEATURE("feat_set_zero_vertical_velocity", FEATURE{ MONSTERACTIONSTATE(state)->Set_ZeroVerticalVelocity(); });
	REGISTER_FEATURE("feat_set_zero_deaccel_rate", FEATURE{ MONSTERACTIONSTATE(state)->Set_ZeroDeAccelRate(); });
	REGISTER_FEATURE("feat_set_deaccel_rate", FEATURE{ MONSTERACTIONSTATE(state)->Set_DeAccelRate(param.fParam[0]); });
	REGISTER_FEATURE("feat_reset_deaccel_rate", FEATURE{ MONSTERACTIONSTATE(state)->Reset_DeAccelRate(); });
	REGISTER_FEATURE("feat_set_apply_gravity", FEATURE{ MONSTERACTIONSTATE(state)->Set_ApplyGravity(param.bParam[0]); });
	REGISTER_FEATURE("feat_set_impuls", FEATURE{ MONSTERACTIONSTATE(state)->SetCCTImpuls_Conversion(Vec3(param.fParam[0], param.fParam[1], param.fParam[2])); });
	REGISTER_FEATURE("feat_set_impuls_right", FEATURE{ MONSTERACTIONSTATE(state)->SetCCTImpuls_Conversion(Vec3(param.fParam[0], 0.f, 0.f)); });
	REGISTER_FEATURE("feat_set_impuls_up", FEATURE{ MONSTERACTIONSTATE(state)->SetCCTImpuls_Conversion(Vec3(0.f, param.fParam[0], 0.f)); });
	REGISTER_FEATURE("feat_set_impuls_front", FEATURE{ MONSTERACTIONSTATE(state)->SetCCTImpuls_Conversion(Vec3(0.f, 0.f, param.fParam[0])); });

	REGISTER_FEATURE("feat_set_root_motion_apply", FEATURE{ MONSTERCC(state)->Set_RootMotion_Apply(param.bParam[0]); });
	
	REGISTER_FEATURE("feat_set_target_offset", FEATURE{ MONSTERCC(state)->Set_Target_Offset(param.fParam[0], param.fParam[1], param.fParam[2], fTimeDelta); });
	REGISTER_FEATURE("feat_auto_teleport_chase", FEATURE{ MONSTERCC(state)->Auto_Teleport_Chase(param.fParam[0]); });
	REGISTER_FEATURE("feat_geniemon_smart_chase", FEATURE{ MONSTERCC(state)->Genimon_Smart_Chase(param.fParam[0], param.fParam[1], param.fParam[2], param.fParam[3], fTimeDelta); });

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

CStatCom_Boss* CMonsterState_Factory::GetBossStat(CStateBase_Monster* state)
{
	return nullptr;//pOwner->;
}

void CMonsterState_Factory::Free()
{
	m_mapCondition.clear();
	m_mapFeature.clear();

	Super::Free();
}
