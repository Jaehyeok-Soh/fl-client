#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CPhysics_CCTFilterCallback : public CBase, public PxControllerFilterCallback
{
    using Super = CBase;
private:
    CPhysics_CCTFilterCallback();
    virtual ~CPhysics_CCTFilterCallback() = default;

    HRESULT Initialize();

public:
    static CPhysics_CCTFilterCallback* Create();
    virtual void Free() override;








    // PxControllerFilterCallback을(를) 통해 상속됨
    bool filter(const PxController& a, const PxController& b) override;
};

NS_END