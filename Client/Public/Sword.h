#pragma once
#include "Weapon.h"

NS_BEGIN(Client)
class CSword final : public CWeapon
{
	using Super = CWeapon;

private:
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSword(const CSword& rhs);
	virtual ~CSword() = default;

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
	static CSword*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END