#pragma once
#include "Client_Defines.h"
#include "ControlContext.h"


/* 플레이어 키 인풋에 대한 관리를 한다 */

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
	enum KEYFLAGS : Flags
	{
		MOVE		= 0x00001
		, JUMP		= 0x00002
		, DASH		= 0x00004
		, SPECIAL   = 0x00008
		, COMBO		= 0x00010
		, SKILL1	= 0x00020
		, SKILL2	= 0x00040
		, INTERACT	= 0x00080
		, GUN		= 0x00100
	};

	typedef struct tagPlayerControlContextDesc
	{
		Flags	FKeys				= {};
	}PLAYER_CONTROLCONTEXT_DESC;
private:
	CPlayerControlContext();
	explicit CPlayerControlContext(const CPlayerControlContext& rhs);
	virtual ~CPlayerControlContext() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iLevelIndex) override;

public:
	void Count_Time(const _float fTimeDelta); // 내부에서 키 인풋 쿨타임을 카운트 하기 위함 등

public:
	_bool Is_FootRayEnabled();
	void Set_Grounded(_bool bGrounded, const COLMESH_HITINFO* pHit);
	void Clear_Grounded();
	ROPE_INFO& Get_RopeInfo() { return m_CurrentRopeInfo; }
	void Set_RopeInfo(const ROPE_INFO& ropeInfo) { m_CurrentRopeInfo = ropeInfo; }

	// 하나하나 컨트롤 
	void Set_CheckKey(KEYFLAGS FKey, _bool bOn);	// 외부에서 onoff를 해야할 시
	// 다 끌꺼면
	void Set_AllKeyFlag(_bool bOn);					// 외부에서 onoff를 해야할 시
	// 이전에 플래그대로 활성화
	void Set_PreKeyFlag();

	void Set_AttackLanded();

public:
	virtual _bool Is_LeftAttackPressed() override;
	virtual _bool Is_RightAttackPressed() override;

	virtual _bool Is_MovePressed() override;
	virtual _bool Is_WalkPressed() override { return false; }
	virtual _bool Is_JumpPressed() override;
	virtual _bool Is_WirePressed() override;
	virtual _bool Is_DodgePressed() override;

	virtual _bool Is_DashPressed() override;
	virtual _bool Is_SepcialMovePressed() override;
	virtual _bool Is_ComboAtt1Pressed() override;
	virtual _bool Is_ComboAtt2Pressed() override;
	virtual _bool Is_ComboAtt3Pressed() override;
	virtual _bool Is_ComboAtt4Pressed() override;
	virtual _bool Is_Skill1Pressed() override;
	virtual _bool Is_Skill2Pressed() override;
	virtual _bool Is_InteractionPressed() override;

	virtual _bool Is_ChargingAttackPressed() override;

	_bool Is_AttackLanded();

	virtual Vec3  Get_MoveDir() override;

	virtual void Clear_WhenChangeLevel() override;
private:
	void OnChangeLockonTarget(CGameObject* pGo);

private:
	DelegateHandle		m_hChangeLockon;
	CCameraMan*			m_pOwnerTargetCamera	= { nullptr };
	COLMESH_HITINFO		m_CurrentGroundInfo		= {};
	ROPE_INFO			m_CurrentRopeInfo		= {};

private:
	Flags				m_FKeys				= { 0 };
	Flags				m_FPreKeys			= { 0 };

	_bool				m_bIsAttackLanded = { false };

public:
	static CPlayerControlContext* Create();
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END