#pragma once
#include <string>
#include <string_view>

namespace Tool
{
    // Client의 CMonsterState_Factory 등록 목록을 수동으로 옮겨 적는 곳
    inline constexpr const char* kConditionNames[] =
    {
        "condition_true_always",
        "condition_target_encount",
        "condition_in_melee_range",
        "condition_in_attack_range",
        "condition_target_out_of_melee_attack",
        "condition_target_out_of_range_attack",
        "condition_target_ahead",
        "condition_target_behind",
        "condition_target_side",
        "condition_target_close",
        "condition_target_lost",
        "condition_animation_finish",
        "condition_die",
        "condition_none_die",
        "condition_already_die",
        "condition_none_already_die",
        "condition_hit",
        "condition_hit_light",
        "condition_hit_heavy",
        "condition_hit_additive",
        "condition_hit_launch",
        "condition_hit_knockdown",
        "condition_loop_animation",
        "condition_cancellation",
        "condition_non_cancellation",
        "condition_over_lifetime",
        "condition_cooldowntime_satisfy",
        "param_condition_IsTrackPositionBetween",
        "param_condition_IsTrackPositionAt",

        // param examples
        "param_condition_distance_over",
    };

    inline constexpr const char* kFeatureNames[] =
    {
        "feat_walk",
        "feat_chase",
        "feat_align_attack",
        "feat_keep_look_target",
        "feat_TurnToTarget_XZ",
        "feat_TurnToTarget_XZ_Ratio",

        // 8-dir
        "feat_move_front",
        "feat_move_right",
        "feat_move_left",
        "feat_move_backward",
        "feat_move_front_right",
        "feat_move_front_left",
        "feat_move_backward_right",
        "feat_move_backward_left",

        // param examples
        "param_feat_move_local",

        "feat_set_dead",
        "feat_set_deadprocess",

        "feat_set_cct_collision_enable",
        "feat_set_cct_collision_disable",

        "feat_set_on_ragdoll",
        "feat_set_off_ragdoll",
    };

    inline constexpr int kConditionCount = (int)(sizeof(kConditionNames) / sizeof(kConditionNames[0]));
    inline constexpr int kFeatureCount = (int)(sizeof(kFeatureNames) / sizeof(kFeatureNames[0]));

    inline bool Contains(const char* const* list, int count, const std::string& s)
    {
        for (int i = 0; i < count; ++i)
        {
            if (s == list[i]) return true;
        }
        return false;
    }

    inline bool IsKnownCondition(const std::string& s)
    {
        if (s.empty()) return false;
        return Contains(kConditionNames, kConditionCount, s);
    }

    inline bool IsKnownFeature(const std::string& s)
    {
        if (s.empty()) return false;
        return Contains(kFeatureNames, kFeatureCount, s);
    }
}