#pragma once
#include "SkillPierceProjectile.h"

NS_BEGIN(Client)

class CMoon_SkillE_Obj final : public CSkillPierceProjectile
{
	using Super = CSkillPierceProjectile;
private:
	CMoon_SkillE_Obj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMoon_SkillE_Obj(const CMoon_SkillE_Obj& rhs);
	virtual ~CMoon_SkillE_Obj() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
public:
	virtual HRESULT Awake(const _uint iCurrentLevelID) override;
	virtual void Try_Attack(const HIT_DESC& hitDesc) override;
private:
	HRESULT Ready_Modules();
public:
	static CMoon_SkillE_Obj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END