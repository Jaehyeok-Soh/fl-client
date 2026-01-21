#pragma once
#include "Client_Defines.h"
#include "ControlContext.h"

NS_BEGIN(Engine)
class CGameObject;
class CCameraMan;
NS_END

NS_BEGIN(Client)
class CSkillComponent;

class CPlayerControlContext final : public CControlContext
{
	using Super = CControlContext;
public:
	typedef struct tagPlayerControlContextDesc
	{

	}PLAYERCC_DESC;
private:
	CPlayerControlContext();
	explicit CPlayerControlContext(const CControlContext& rhs);
	virtual ~CPlayerControlContext() = default;

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
	virtual _bool Is_LeftAttackPressed() override;
	virtual _bool Is_RightAttackPressed() override;
	virtual _bool Is_FrontDashPressed() override;
	virtual _bool Is_LeftDashPressed() override;
	virtual _bool Is_RightDashPressed() override;
	virtual _bool Is_BackDashPressed() override;
	virtual _bool Is_MovePressed() override;
	virtual _bool Is_WalkPressed() override { return false; }
	virtual _bool Is_JumpPressed() override;
	virtual _bool Is_WirePressed() override;
	virtual _bool Is_DodgePressed() override;
	virtual _bool Is_ChakraJumpPressed() override;
	virtual _bool Is_FirstSkillPressed() override { return false; }
	virtual _bool Is_SecondSkillPressed() override { return false; }
	virtual _bool Is_ThirdSkillPressed() override { return false; }
	virtual _bool Is_RopePressed() override;
	virtual _fvector Get_MoveDir() override;
private:
	void OnChangeLockonTarget(CGameObject* pGo);
private:
	DelegateHandle m_hChangeLockon;
	CCameraMan* m_pOwnerTargetCamera = { nullptr };
	COLMESH_HITINFO m_CurrentGroundInfo = {};
	ROPE_INFO m_CurrentRopeInfo = {};
public:
	static CPlayerControlContext* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END