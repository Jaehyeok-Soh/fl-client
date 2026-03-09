#pragma once
#include "SkillProjectile.h"

NS_BEGIN(Client)

class CSkillPierceProjectile : public CSkillProjectile
{
    using Super = CSkillProjectile;
public:

protected:
    CSkillPierceProjectile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CSkillPierceProjectile(const CSkillPierceProjectile& rhs);
    virtual ~CSkillPierceProjectile() = default;

    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
protected:
    virtual void Handle_Hit(_uint iMyLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo) override;
public:
    static CSkillPierceProjectile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END