#pragma once
#include "SkillProjectile.h"

NS_BEGIN(Client)

class CMonster_Veteran_Projectile_Circle final : public CSkillProjectile
{
	using Super = CSkillProjectile;
private:
	CMonster_Veteran_Projectile_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster_Veteran_Projectile_Circle(const CMonster_Veteran_Projectile_Circle& rhs);
	virtual ~CMonster_Veteran_Projectile_Circle() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Moduels();
public:
	static CMonster_Veteran_Projectile_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END