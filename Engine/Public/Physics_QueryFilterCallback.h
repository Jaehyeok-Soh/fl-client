#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGameObject;

class CPhysics_QueryFilterCallback final : public CBase, public PxQueryFilterCallback
{
    using Super = CBase;
private:
    CPhysics_QueryFilterCallback();
    virtual ~CPhysics_QueryFilterCallback() = default;

    HRESULT Initialize();

public:
    void SetOwner(CGameObject* pOwner);

private:
    CGameObject* m_pOwner = { nullptr };

public:
    static CPhysics_QueryFilterCallback* Create();
    virtual void Free() override;






    // PxQueryFilterCallback을(를) 통해 상속됨
    PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) override;
    PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) override;
};

NS_END