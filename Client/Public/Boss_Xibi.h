#pragma once
#include "Monster_Base.h"

NS_BEGIN(Client)

class CBoss_Xibi final : public CMonster_Base
{
	using Super = CMonster_Base;
public:
	enum EStateForDirecting
	{
		Idle = 0,
		Condemned_Die,
		Condemned_End,
		Direction,
		COUNT
	};
private:
	CBoss_Xibi(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CBoss_Xibi(const CBoss_Xibi& rhs);
	virtual ~CBoss_Xibi() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT		Awake(const _uint iCurrentLevelID) override;
	virtual void		Update_Priority(const _float fTimeDelta) override;
	virtual void		Update(const _float fTimeDelta) override;
	virtual void		Update_Late(const _float fTimeDelta) override;
	virtual void		Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual _int		Get_WeaponAnimationIndex(const wstring& wstrName) override;
public:
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;

	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;
	HRESULT Change_State_ForDirecting(EStateForDirecting eState);
private:
	HRESULT Ready_Ability();
	HRESULT Ready_Weapon();
	HRESULT Ready_Components(void* pArg);
	HRESULT Ready_StateIndexForDirecting();
private:
	array<_int, (size_t)EStateForDirecting::COUNT> m_arrStateIndex;

public:
	static CBoss_Xibi* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END