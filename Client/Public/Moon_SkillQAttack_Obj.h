#pragma once
#include "SkillProjectile.h"

NS_BEGIN(Client)

class CMoon_SkillQAttack_Obj final : public CSkillProjectile
{
	using Super = CSkillProjectile;
private:
	CMoon_SkillQAttack_Obj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMoon_SkillQAttack_Obj(const CMoon_SkillQAttack_Obj& rhs);
	virtual ~CMoon_SkillQAttack_Obj() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Update_Priority(const _float fTimeDelta) override;
	virtual void Update(const _float fTimeDelta) override;
	virtual void Update_Late(const _float fTimeDelta) override;
	virtual void Ready_Before_Render(const _float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual _bool On_Hit(const HIT_DESC& hitDesc) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;
	virtual void Handle_Hit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
private:
	TimeCount m_TAttackCoolTime = { 1.f,1.f };

private:
	HRESULT Ready_Components();
	
private:
	void Count_CoolTime(const _float fTimeDelta);

public:
	static CMoon_SkillQAttack_Obj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END

