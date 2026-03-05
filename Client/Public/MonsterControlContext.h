#pragma once
#include "ControlContext.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CSkillComponent;

class CMonsterControlContext final : public CControlContext
{
public:
	typedef struct tagSubState
	{
		enum Enum
		{
			FALL = 1 << 0,
			DOWN = 1 << 1,
			AIRBORNE = 1 << 2,
			FLY = 1 << 3,
			HIT = 1 << 4,
			DEAD = 1 << 5,
			DEAD_PROCESS = 1 << 6,
			END
		};
	}SUB_STATE;

	typedef struct tagMonsterControlContextDesc
	{
		_float fMeleeRange = {};
		_float fAttackRange = {};
		_float fCloseRange = {};
		_float fDetectionRange = {};
		_float fSpeed = {};
		_int iSkillCount = { -1 };
		vector<_int> vecSkillRange;
	}MONSTER_CONTROLCONTEXT_DESC;

private:
	using Super = CControlContext;

private:
	CMonsterControlContext();
	explicit CMonsterControlContext(const CMonsterControlContext& rhs);
	virtual ~CMonsterControlContext() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;

public:
	virtual _bool Is_LeftAttackPressed() override { return false; }
	virtual _bool Is_RightAttackPressed() override { return false; }
	virtual _bool Is_ChargingAttackPressed() override { return false; }

	virtual _bool Is_MovePressed() override { return false; }
	virtual _bool Is_WalkPressed() override { return false; }
	virtual _bool Is_JumpPressed() override { return false; }
	virtual _bool Is_WirePressed() override { return false; }
	virtual _bool Is_DodgePressed() override { return false; }

	virtual _bool Is_DashPressed() override { return false; }
	virtual _bool Is_SepcialMovePressed() override { return false; }
	virtual _bool Is_ComboAtt1Pressed() override { return false; }
	virtual _bool Is_ComboAtt2Pressed() override { return false; }
	virtual _bool Is_ComboAtt3Pressed() override { return false; }
	virtual _bool Is_ComboAtt4Pressed() override { return false; }
	virtual _bool Is_Skill1Pressed() override { return false; }
	virtual _bool Is_Skill2Pressed() override { return false; }
	virtual _bool Is_InteractionPressed() override { return false; }

public:
	virtual Vec3  Get_MoveDir() override;

	void Set_Dead();
	void Set_Dead_Process() { m_iSubState |= SUB_STATE::DEAD_PROCESS; }
	void Set_HitDesc(HIT_DESC hitDesc)
	{
		m_tHitDesc = hitDesc;
		m_iSubState |= SUB_STATE::HIT;
	}

/// <summary>
/// Condition
/// </summary>
public:
	// 타겟
	_bool IsTargetFound();
	_bool IsTargetLost();

	_bool IsTargetAlive();
	_bool IsTargetVisible();
	_bool IsTargetFOV();
	_bool IsTargetBehind();
	_bool IsTargetSide();
	_bool IsTargetClose();
	_bool IsTargetAhead();

	// 절벽
	_bool IsCliffAhead();

	// 페이즈
	_bool IsPhaseTwo();

	// 공격 범위
	_bool IsTargetInMeleeRange();
	_bool IsTargetInAttackRange();
	_bool IsTargetOutOfMeleeRange();
	_bool IsTargetOutOfAttackRange();
	_bool IsTargetDistanceOver(_float fValue);

	// 공간
	_bool IsFalling();
	_bool IsDown();

	// 상태
	_bool IsHit();
	_bool IsHitAdditive();
	_bool IsHitLight();
	_bool IsHitHeavy();
	_bool IsHitLaunch();
	_bool IsHitKnockdown();

	_bool IsDead();
	_bool IsDeadProcessing();

	// 데미지
	_bool IsDamageRecently();

	// 공격 여부
	_bool IsComboPossible();
	_bool CanAttackMelee();
	_bool CanAttackRanged();
	_bool IsSkillReady();

	// 이동 도착
	_bool IsArrived();
	_bool IsPathBlocked();

/// <summary>
/// Feature
/// </summary>
public:
	void UpdateWalk(const _float fTimeDelta);
	void UpdateChase(const _float fTimeDelta);
	void Update_TurnToTarget_XZ(const _float fTimeDelta);
	void Update_8Dir_LocalAxisXZ(const _float fTimeDelta, _float fForward, _float fRight);
	void UpdateRun(const _float fTimeDelta);
	void UpdateFly(const _float fTimeDelta);
	void UpdateFall(const _float fTimeDelta);
	void UpdateJump(const _float fTimeDelta);
	void UpdateDash(const _float fTimeDelta);
	void UpdateCircleMove(const _float fTimeDelta);
	void UpdateSideWalk(const _float fTimeDelta);
	void UpdateTurn90(const _float fTimeDelta);
	void UpdateTrun180(const _float fTimeDelta);
	void Set_CCT_Collision_Disable();
	void Set_CCT_Collision_Enable();

private:
	//EMovementMode m_eCurrentMovement = { EMovementMode::GROUND };
	//CGameObject* m_pTarget = { nullptr };
	//_bool m_bContectedWithTarget = { false };
	//_bool m_bGrounded = { false };
	//_bool m_bGravity = { false };
	//_uint m_iJumpCount = { 0 };

	Vec3 m_vMoveDir = {};

	MONSTER_CONTROLCONTEXT_DESC m_tDesc = {};
	HIT_DESC m_tHitDesc = {};
	_uint m_iSubState = 0;

public:
	static CMonsterControlContext* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END