#pragma once
#include "SkillProjectile.h"

NS_BEGIN(Client)

class CMonster_Fly_Projectile_Circle final : public CSkillProjectile
{
	struct HitSoundHashNum
	{
		enum Enum
		{
			LAUNCHED,
			LOOP,
			LANDED,
			END
		};
	};
	using Super = CSkillProjectile;
private:
	CMonster_Fly_Projectile_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMonster_Fly_Projectile_Circle(const CMonster_Fly_Projectile_Circle& rhs);
	virtual ~CMonster_Fly_Projectile_Circle() = default;

	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
private:
	HRESULT Ready_Moduels();
private:
	array<_uint, HitSoundHashNum::END> m_arrHitSoundHash;
public:
	static CMonster_Fly_Projectile_Circle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END