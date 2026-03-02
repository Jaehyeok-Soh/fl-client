#pragma once
#include "ControlContext.h"

NS_BEGIN(Client)

class CBossControlContext final : public CControlContext
{
	using Super = CControlContext;
public:
	typedef struct tagBossControlContextDesc
	{
		_float fMeleeRange = {};
		_float fAttackRange = {};
		_float fCloseRange = {};
		_float fDetectionRange = {};
		// _int iSkillCount = { -1 };
		// vector<_int> vecSkillRange;
	}BOSS_CONTROLCONTEXT_DESC;
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
	CBossControlContext();
	explicit CBossControlContext(const CBossControlContext& rhs);
	virtual ~CBossControlContext() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;
	void Update(const _float fTimeDelta);
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
	_bool IsTargetValid() const { return m_runtimeDesc.bTargetValid; }
	_bool IsTargetFound() const { return m_runtimeDesc.bTargetValid && m_runtimeDesc.fDistance <= m_desc.fDetectionRange; }
	_bool IsTargetLost() const { return !m_runtimeDesc.bTargetValid || m_runtimeDesc.fDistance > m_desc.fDetectionRange; }
	_bool IsTargetInMeleeRange() const { return m_runtimeDesc.bTargetValid && m_runtimeDesc.fDistance <= m_desc.fMeleeRange; }
	_bool IsTargetInAttackRange() const { return m_runtimeDesc.bTargetValid && m_runtimeDesc.fDistance <= m_desc.fAttackRange; }
	_bool IsTargetOutOfMeleeRange() const { return IsTargetFound() && !IsTargetInMeleeRange(); }
	_bool IsTargetOutOfAttackRange() const { return IsTargetFound() && !IsTargetInAttackRange(); }
	_bool IsTargetClose() const { return m_runtimeDesc.bTargetValid && m_runtimeDesc.fDistance <= m_desc.fCloseRange; }
	_bool IsTargetDistanceOver(_float fValue) const { return m_runtimeDesc.fDistance > fValue; }

	_bool IsTargetAhead() const { return m_runtimeDesc.bTargetValid && m_runtimeDesc.fDotForward > 0.9f; }
	_bool IsTargetBehind() const { return m_runtimeDesc.bTargetValid && m_runtimeDesc.fDotForward < 0.f; }
	_bool IsTargetSide() const { return m_runtimeDesc.bTargetValid && std::abs(m_runtimeDesc.fDotForward) < 0.7f; }
public:
	virtual Vec3 Get_MoveDir() override { return m_runtimeDesc.vMoveDir; }
	void UpdateChase(const _float fTimeDelta);
	void UpdateWalk(const _float fTimeDelta);
	void Update_8Dir_LocalAxisXZ(const _float fTimeDelta, _float fForward, _float fRight);
private:
	BOSS_CONTROLCONTEXT_DESC m_desc{};
	RUNTIME_DESC m_runtimeDesc{};
public:
	static CBossControlContext* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END