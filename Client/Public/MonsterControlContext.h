#pragma once
#include "ControlContext.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)
class CSkillComponent;

class CMonsterControlContext final : public CControlContext
{
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
	_bool Is_FootRayEnabled();
	void Set_Grounded(_bool bGrounded, const COLMESH_HITINFO* pHit);
	void Clear_Grounded();
	ROPE_INFO& Get_RopeInfo() { return m_CurrentRopeInfo; }
	void Set_RopeInfo(const ROPE_INFO& ropeInfo) { m_CurrentRopeInfo = ropeInfo; }

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

	virtual Vec3  Get_MoveDir() override;
private:
	void OnChangeLockonTarget(CGameObject* pGo);

private:
	DelegateHandle		m_hChangeLockon;
	COLMESH_HITINFO		m_CurrentGroundInfo = {};
	ROPE_INFO			m_CurrentRopeInfo = {};

public:
	static CMonsterControlContext* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END