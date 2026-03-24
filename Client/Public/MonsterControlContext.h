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
			NONE,
			FALL = 1 << 0,
			DOWN = 1 << 1,
			AIRBORNE = 1 << 2,
			FLY = 1 << 3,
			HIT = 1 << 4,
			DIE_PROCESS = 1 << 5,
			DIE_POSE = 1 << 6,
			GROGGY_REQ = 1 << 7,
			GROGGY_ACTIVE = 1 << 8,
			ATTACK_LANDED = 1 << 9,
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

	typedef struct tagRuntimeDesc
	{
		Vec3	vMoveDir = {};
		Vec3    vOwnerPos = {};
		Vec3    vOwnerLook = {};
		Vec3    vOwnerRight = {};
		Vec3    vTargetPos = {};
		Vec3    vToTarget = {};
		Vec3    vToTargetDir = {};
		_float  fDistance = { FLT_MAX };
		_float  fDotForward = { 0.f };
		_bool   bTargetValid = { false };
	}RUNTIME_DESC;
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
	void Update_RuntimeDesc(const _float fTiemDelta);
private:
	void Update_Groggy(const _float fTimeDelta);
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
	void Set_PhaseTwo() { m_bPhaseTwo = true; }
	void Set_HitDesc(HIT_DESC hitDesc)
	{
		m_tHitDesc = hitDesc;
		Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::HIT);
	}
	// 해당 요청이 이루어지면 true 요청 취소되면 false
	_bool Set_Groggy(EGroggyState eState, _bool bRequest = true, _float fGroggyDuration = 10.f);
	void Set_DieProcess() { Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::DIE_PROCESS); }
	void Set_DiePose() { Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::DIE_POSE); }
	void Set_AttackLanded() { Engine_Utils::Add_Flag(m_iSubState, SUB_STATE::ATTACK_LANDED); }
/// <summary>
/// Condition
/// </summary>
public:
	// 타겟
	_bool IsTargetFound() const { return m_tRuntimeDesc.bTargetValid && m_tRuntimeDesc.fDistance <= m_tDesc.fDetectionRange; }
	_bool IsTargetLost() const { return !m_tRuntimeDesc.bTargetValid || m_tRuntimeDesc.fDistance > m_tDesc.fDetectionRange; }
	_bool IsTargetAlive() const { return m_tRuntimeDesc.bTargetValid; }
	_bool IsTargetVisible();
	_bool IsTargetFOV();
	_bool IsTargetLeft90() const;
	_bool IsTargetRight90() const;
	_bool IsTargetLeft180() const;
	_bool IsTargetRight180() const;
	_bool IsTargetBehind() const;
	_bool IsTargetSide() const;
	_bool IsTargetClose() const { return m_tRuntimeDesc.bTargetValid && m_tRuntimeDesc.fDistance <= m_tDesc.fCloseRange; }
	_bool IsTargetAhead() const { return m_tRuntimeDesc.bTargetValid && m_tRuntimeDesc.fDotForward > 0.9f; }

	// 절벽
	_bool IsCliffAhead();

	// 페이즈
	_bool IsPhaseTwo() const { return m_bPhaseTwo; }

	// 공격 범위
	_bool IsTargetInMeleeRange() const { return m_tRuntimeDesc.bTargetValid && m_tRuntimeDesc.fDistance <= m_tDesc.fMeleeRange; }
	_bool IsTargetInAttackRange() const { return m_tRuntimeDesc.bTargetValid && m_tRuntimeDesc.fDistance <= m_tDesc.fAttackRange; }
	_bool IsTargetOutOfMeleeRange() const { return IsTargetFound() && !IsTargetInMeleeRange(); }
	_bool IsTargetOutOfAttackRange() const { return IsTargetFound() && !IsTargetInAttackRange(); }
	_bool IsTargetDistanceOver(_float fValue) const { return m_tRuntimeDesc.fDistance > fValue; }

	// 공간
	_bool IsFalling() const { return Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::FALL); }
	_bool IsDown() const { return Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::DOWN); }

	// 상태
	_bool IsHit() const { return (m_tHitDesc.attackDesc.pAttackPreset != nullptr) && Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::HIT); }
	_bool IsHitAdditive();
	_bool IsHitLight();
	_bool IsHitHeavy();
	_bool IsHitLaunch();
	_bool IsHitKnockdown();
	_bool IsNotGroggy() const { return (Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE) == 0) && (Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_REQ) == 0); }
	_bool IsNormalGroggyRequested() const { return (m_eCurrentGroggyState != EGroggyState::Final) && Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_REQ); }
	_bool IsFinalGroggyRequested() const { return (m_eCurrentGroggyState == EGroggyState::Final) && Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_REQ); }
	_bool IsGroggy() const { return Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE); }
	_bool IsNormalGroggy() const { return(m_eCurrentGroggyState != EGroggyState::Final) && Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE); }
	_bool IsFinalGroggy() const { return (m_eCurrentGroggyState == EGroggyState::Final) && Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::GROGGY_ACTIVE); }

	_bool IsAlive();
	_bool IsDying();
	_bool IsDieProcess() { return Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::DIE_PROCESS); }
	_bool IsDiePose() { return Engine_Utils::Has_Flag(m_iSubState, SUB_STATE::DIE_POSE); }

	// 데미지
	_bool IsDamageRecently();

	// 공격 여부
	_bool IsAttackLanded();
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

	void Set_On_Ragdoll();
	void Set_Off_Ragdoll();

	void Consume_GroggyRequest();
	void End_Groggy();

	void Set_RootMotion_Apply(_bool bApply);

	void Set_Target_Offset(_float fX, _float fY, _float fZ, _float fTimeDelta);
	void Auto_Teleport_Chase(_float fMaxLength = 10.f);
	void Genimon_Smart_Chase(_float fX, _float fY, _float fZ, _float fMaxLength, _float fTimeDelta);

private:
	void Clear_RuntimeDesc();

private:
	//EMovementMode m_eCurrentMovement = { EMovementMode::GROUND };
	//CGameObject* m_pTarget = { nullptr };
	//_bool m_bContectedWithTarget = { false };
	//_bool m_bGrounded = { false };
	//_bool m_bGravity = { false };
	//_uint m_iJumpCount = { 0 };

	Vec3 m_vMoveDir = {};
	MONSTER_CONTROLCONTEXT_DESC m_tDesc = {};
	EGroggyState m_eCurrentGroggyState = { EGroggyState::None };
	TIME_LINE m_tGroggyCounter = {};
	HIT_DESC m_tHitDesc = {};
	RUNTIME_DESC m_tRuntimeDesc = {};
	_uint m_iSubState = 0;

	uint64 m_iOwnerID = {};

	_bool m_bPhaseTwo = { false };
public:
	static CMonsterControlContext* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END