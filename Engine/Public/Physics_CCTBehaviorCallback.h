#pragma once
#include "Base.h"

#include "GameObject.h"

NS_BEGIN(Engine)

class CPhysics_CCTBehaviorCallback final : public CBase, public PxControllerBehaviorCallback
{
    using Super = CBase;
private:
    CPhysics_CCTBehaviorCallback();
    virtual ~CPhysics_CCTBehaviorCallback() = default;

    HRESULT Initialize();

public:
    static CPhysics_CCTBehaviorCallback* Create();
    virtual void Free() override;








    // PxControllerBehaviorCallback을(를) 통해 상속됨
    PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller) override;
    PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) override { PX_UNUSED(shape); PX_UNUSED(actor);}
    PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle) override { PX_UNUSED(obstacle); }
};

NS_END