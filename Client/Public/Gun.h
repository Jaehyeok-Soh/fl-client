#pragma once
#include "Weapon.h"

NS_BEGIN(Client)
class CGun final : public CWeapon
{
	using Super = CWeapon;

public:
	enum class GunType : _uint
	{
		MACHINGUN, 
	};

	enum class GunState : _uint
	{
		NOATT, ATT, EMPTY, RELOAD
	};

public:
	typedef struct tagGunDesc : public CWeapon::WEAPON_DESC
	{
		_float fAllBullet		= { 0.f };
		_float fCurBullet		= { 0.f };
		_float fAttackCoolTime	= { 0.f };
	}GUN_DESC;

private:
	CGun(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGun(const CGun& rhs);
	virtual ~CGun() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT			Awake(const _uint iCurrentLevelIndex) override;
	virtual void			Update_Priority(_float fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Update_Late(_float fTimeDelta) override;
	virtual void			Ready_Before_Render(_float fTimeDelta) override;
	virtual void			OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT			Render() override;

public:
	void Change_GunState(GunState eState);
	void Change_GunState(_uint iState);


	// getter setter
public:
	_bool Get_CanFire();
	_bool Get_CanReleod();

private:
	GunState m_eGunState = { GunState::NOATT };

	_float m_fSpeed		= { 1.f };

	// 항상 x가 cur개수. y는 max 개수
	MinMax			m_MAllBullet = { 0.f,0.f };
	MinMax			m_MCurBullet = { 0.f,0.f };

	TIME_COUNTER	m_tFireTimeCounter = { 0.f,0.f };

private:
	void State_Start(GunState eState);
	void State_Update(const _float fTimeDelta);
	void State_End(GunState eState);

	void NoAttack_Update(const _float fTimeDelta);
	void Attack_Update(const _float fTimeDelta);
	void Empty_Update(const _float fTimeDelta);
	void Reload_Update(const _float fTimeDelta);

public:
	static CGun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END