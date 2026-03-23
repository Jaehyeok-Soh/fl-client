#pragma once
#include "NPC_Base.h"

NS_BEGIN(Engine)
NS_END

NS_BEGIN(Client)

class CNPC_Kid_1 final : public CNPC_Base
{
	using Super = CNPC_Base;

private:
	CNPC_Kid_1(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CNPC_Kid_1(const CNPC_Kid_1& rhs);
	virtual ~CNPC_Kid_1() = default;

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
	HRESULT Ready_PartObjects();
	HRESULT Ready_Components(void* pArg);

public:
	static NPC_DESC Get_PreSetDesc(_uint iLevelId);

private:
	virtual void QuestEnter() override;
	virtual void QuestExit() override;

	virtual void Interact() override;

public:
	static CNPC_Kid_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END