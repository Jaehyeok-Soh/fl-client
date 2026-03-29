#pragma once
#include "Weapon.h"

NS_BEGIN(Engine)
class CPhysicsAttackRaycast;
NS_END

NS_BEGIN(Client)
class CGun : public CWeapon
{
	using Super = CWeapon;

public:
	enum class GunType : _uint
	{
		MACHINGUN, 
	};

public:
	typedef struct tagGunInfo
	{
		_float fTotalBullet		= { 0.f };
		_float fCurrentBullet	= { 0.f };
	}GUN_INFO;

	typedef struct tagGunDesc : public CWeapon::WEAPON_DESC
	{
		_float fAllBullet		= { 0.f };
		_float fCurBullet		= { 0.f };
		_float fAttackCoolTime	= { 0.f };

		_uint iFireSoundHash = {};

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
	virtual void			OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void			OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual HRESULT			Render() override;

private:
	HRESULT Ready_Components();

public:
	void					Reload_Bullet();

	// getter setter
public:
	_bool Get_CanFire();
	_bool Get_CanReleod();
	const MinMax Get_TotalButtlet() const { return m_MTotalBullet; }
	const MinMax Get_CurButtlet() const { return m_MCurBullet; }
	const _bool Get_isFire()const { return m_isFire; }

	void Set_FireTimer(_bool bCount) { m_tFireTimeCounter.bCountTime = bCount; }
	void Reset_FireTimer() { m_tFireTimeCounter.fTimeAcc = m_tFireTimeCounter.fMaxTime * 0.5f; m_tFireTimeCounter.bCountTime = false; }

	_bool Get_OnTarget() { return m_bOnTarget; }

private:
	class CTransform* m_pCameraTransform = { nullptr };
	class CPhysicsAttackRaycast* m_pAttackRaycast = { nullptr };

	_float m_fSpeed		= { 1.f };

	// 항상 x가 cur개수. y는 max 개수
	MinMax			m_MTotalBullet		= { 0.f,0.f };
	MinMax			m_MCurBullet		= { 0.f,0.f };

	TIME_COUNTER	m_tFireTimeCounter	= { 0.f,0.f };

	_bool m_isFire = { false };

	_bool m_bOnTarget = { false }; // 조준 히트

	_uint m_iFireSoundHash = {};

private:
	void NoAttack_Update(const _float fTimeDelta);
	void Attack_Update(const _float fTimeDelta);
	void Empty_Update(const _float fTimeDelta);
	void Reload_Update(const _float fTimeDelta);

private:
	void Fire();

public:
	static CGun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END