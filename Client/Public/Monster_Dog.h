#pragma once
#include "Monster_Base.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CMonster_Dog final : public CMonster_Base
{
	using Super = CMonster_Base;
private:
	CMonster_Dog(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonster_Dog(const CMonster_Dog& rhs);
	virtual ~CMonster_Dog() = default;

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
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;

private:
	HRESULT Ready_Ability();
	HRESULT Ready_BaseStates();
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components(void *pArg);

	HRESULT Ready_StateIndexForDirecting();
	HRESULT Change_State_ForDirecting(_int iStateIdx);

public:
	static MONSTER_DESC Get_PreSetDesc(_uint iLevelId);

public:
	static CMonster_Dog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END