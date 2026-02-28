#pragma once
#include "Component.h"

NS_BEGIN(Engine)

typedef struct tagSKillColldownInfo
{
	_float fRemaind = { 0.f };
	_float fDuration = { 0.f };
}SKILL_COOLDOWN;

class CGameObject;

class ENGINE_DLL CControlContext abstract : public CComponent
{
	using Super = CComponent;
public:
	constexpr static EComponentType _ID = EComponentType::CONTROLCONTEXT;

	enum class CONTROL_KEY {
		MOVE
		, DASH
		, WALK
		, SPECIALMV
		, JUMP
		, WIRE
		, DODGE
		, LATT
		, RATT
		, CHARGATT
		, COMBO1
		, COMBO2
		, COMBO3
		, COMBO4
		, SKILL1
		, SKILL2
	};

protected:
	CControlContext();
	explicit CControlContext(const CControlContext& rhs);
	virtual ~CControlContext() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) PURE;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) PURE;
public:
	_bool Is_Grounded() const { return m_bGrounded; }
	_uint Get_JumpCount() const { return m_iJumpCount; }

	void Set_Grounded(_bool bGrounded) { m_bGrounded = bGrounded; }
	void Set_JumpCount(_uint iCount) { m_iJumpCount = iCount; }
	void Set_MovementMode(EMovementMode eMode) { m_eCurrentMovement = eMode; }
	_bool Is_GroundMode() const { return m_eCurrentMovement == EMovementMode::GROUND; }
	_bool Is_WallMode() const { return m_eCurrentMovement == EMovementMode::WALL; }
	_bool Set_ContectWithTarget(_bool bContect) { return m_bContectedWithTarget = bContect; }
	_bool Is_ContectedWithTarget() const { return m_bContectedWithTarget; }

public:
	virtual _bool	Is_MovePressed()				PURE;
	virtual _bool	Is_DashPressed()				PURE;
	virtual _bool	Is_WalkPressed()				PURE;
	virtual _bool	Is_SepcialMovePressed()			PURE;
	virtual _bool	Is_JumpPressed()				PURE;

	virtual _bool	Is_WirePressed()				PURE;
	virtual _bool	Is_DodgePressed()				PURE;

	virtual _bool	Is_LeftAttackPressed()			PURE;
	virtual _bool	Is_RightAttackPressed()			PURE;
	virtual _bool	Is_ChargingAttackPressed()		PURE;

	virtual _bool	Is_ComboAtt1Pressed()			PURE;
	virtual _bool	Is_ComboAtt2Pressed()			PURE;
	virtual _bool	Is_ComboAtt3Pressed()			PURE;
	virtual _bool	Is_ComboAtt4Pressed()			PURE;

	virtual _bool	Is_Skill1Pressed()				PURE;
	virtual _bool	Is_Skill2Pressed()				PURE;

	virtual _bool	Is_InteractionPressed()			PURE;

	virtual Vec3	Get_MoveDir()					PURE;

	CGameObject* Get_Target() { return m_pTarget; }
	virtual void	Clear_WhenChangeLevel() override;
protected:
	EMovementMode	m_eCurrentMovement		= { EMovementMode::GROUND };
	CGameObject*	m_pTarget				= { nullptr };
	_bool			m_bContectedWithTarget	= { false };
	_bool			m_bGrounded				= { false };
	_uint			m_iJumpCount			= { 0 };
public:
	virtual CComponent* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END