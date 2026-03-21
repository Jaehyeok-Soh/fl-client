#pragma once
#include "Monster_Body_Base.h"

NS_BEGIN(Client)

class CMonster_Fly_Body final : public CMonster_Body_Base
{
	using Super = CMonster_Body_Base;
private:
	CMonster_Fly_Body(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster_Fly_Body(const CMonster_Fly_Body& rhs);
	virtual ~CMonster_Fly_Body() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT Awake(const _uint iCurrentLevelIndex) override;
	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Ready_Before_Render(_float fTimeDelta) override;
	virtual void OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual void OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
	virtual void OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther) override;
	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components(MONSTERBODY_DESC* pDesc);
	HRESULT Bind_ShaderResources();
public:
	static CMonster_Fly_Body* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END