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

class CPlayer abstract : public CContainerObject
{
	using Super = CContainerObject;
public:
	typedef struct tagPlayerDesc : public Super::GAMEOBJECT_DESC
	{
		wstring wstrBodyModelTag = { L"" };
		wstring wstrNavigationPrototypeTag = { L"" };
		_int iNavigationCellIndex = { -1 };
		Vec3 vSpawnPosition = {};
	}PLAYER_DESC;
	enum Part : _uint
	{
		BODY = 0,
		SWORD,
		SKILL,
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
		,SLIDESKY

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
		,CHARGE
		, JUMPATTSTART
		, JUMPATTEND

		,GUN
		, JUMPGUN

		,SKILL1
		,SKILL2

		,JUMPWALL

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

	// state funcs
public:
	void Change_Weapon(_uint iPart, _uint iState); // 어떤 weapon을 어떤 state로
	_bool Check_OnGround(_float fMaxDist = 0.72f);
	_bool Check_ColliWithMonster();
	void Count_Combo();
	void Count_Dash();

public:
	_bool	Start_Attack(State iState);
	void	End_Attack(State iState);

private:
	HRESULT Ready_BaseStates();
	HRESULT Ready_PartObjects(PLAYER_DESC* pDesc);
	HRESULT Ready_Components(PLAYER_DESC* pDesc);

protected:
	CPhysics_QueryFilterCallback* m_pPhysic_QueryFilter = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg) PURE;
	virtual void Free() override;
};

NS_END
