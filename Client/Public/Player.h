#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)
class CNavigation;
class CStateBase;
NS_END

NS_BEGIN(Client)
class CStatComponent;
class CSkillComponent;
class CCameraMan_Targeter;
class CBody;
class CGun;

class CPlayer abstract : public CContainerObject
{
	using Super = CContainerObject;
public:
	enum class PLAYER_TYPE { MOON, END };

	typedef struct tagPlayerDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = { L"" };
		wstring wstrNavigationPrototypeTag = { L"" };
		_int iNavigationCellIndex = { -1 };
		Vec3 vSpawnPosition = {};

		PLAYER_TYPE ePlayerType = { PLAYER_TYPE::END };

	}PLAYER_DESC;

	enum Part : _uint
	{
		BODY = 0,
		SWORD,
		SKILL,
		GUN,
		SWORD2,
		EFFECT,
		END
	};

	enum Skill : _uint
	{
		MoonE = 0,
		MoonQ
	};

	enum class State : _uint
	{
		IDLE
		,WALK
		,CROUCH
		,CROUCHWALK
		,SLIDE

		,DASHBACK
		,DASHSKY

		,RUNSHORT
		,RUNLOOP

		,JUMP
		,JUMPDOUBLE
		,JUMPBULLET
		,JUMPBACK

		,FALL
		,LAND

		,COMBO
		, JUMPATTSTART
		, JUMPATTEND
		,CHARGE


		,GUNIDLE
		,GUNWALK
		,GUNATTACK
		,GUNRELOAD

		,SKILL1
		,SKILL2

		,JUMPWALL

		,HITADDTIVE
		,HITFLYSTART
		,HITFLYEND
		,HITSTRONG

		,END
	};
protected:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT		Awake(const _uint iCurrentLevelID) override;
	virtual void		Update_Priority(const _float fTimeDelta) override;
	virtual void		Update(const _float fTimeDelta) override;
	virtual void		Update_Late(const _float fTimeDelta) override;
	virtual void		Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	virtual _int		Get_AnimationIndex(const wstring& wstrName) override;
	virtual _wstring	Get_AnimationName(_uint iAniIndex);

public:
	PLAYER_TYPE Get_PlayerType() const { return m_ePlayerType; }

	// state funcs
public:
	void	Change_Weapon(_uint iPart, _uint iState); // 어떤 weapon을 어떤 state로
	_bool	Check_OnGround(_float fMaxDist = 0.72f);
	_bool	Check_ColliWithMonster();
	void	Count_Combo();
	void	Count_Dash();
	void	Set_RootMotion_Apply(_bool bApply);

	_bool	Check_DoubleJump();
	void	Set_DoubleJumpCount(_bool bCount) { m_tDoubleJumpCount.bCountTime = bCount; if (!bCount) m_tDoubleJumpCount.fTimeAcc = 0.f; }

	void	Change_CamState(_uint iCamState);

public:
	_bool	Start_Attack(State iState);
	void	End_Attack(State iState);

protected:
	CPhysics_QueryFilterCallback* m_pPhysic_QueryFilter = { nullptr };

protected:
	TIME_COUNTER	m_tDoubleJumpCount = {};
	PLAYER_TYPE		m_ePlayerType = { PLAYER_TYPE::END };

private:
	HRESULT Ready_BaseStates();
	HRESULT Ready_HitStates();
	HRESULT Ready_PartObjects(PLAYER_DESC* pDesc);
	HRESULT Ready_Components(PLAYER_DESC* pDesc);

private:
	void Count_DoubleJump(const _float fTimeDelta);

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END