#pragma once
#include "CameraModifierBase.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class CCameraModifier_Shake final : public CCameraModifierBase
{
    using Super = CCameraModifierBase;
private:
    explicit CCameraModifier_Shake(const CAMERA_SHAKE_DESC& desc);
    CCameraModifier_Shake(const CCameraModifier_Shake&) = default;
    virtual ~CCameraModifier_Shake() = default;
public:
    virtual ECameraModifierType Get_Type() const override { return ECameraModifierType::Shake; }
    virtual void Update(_float fTimeDelta) override;
    virtual void Accumulate(const CAMERA_POSE& basePose, CAMERA_MODIFIER_RESULT& outResult) const override;
private:
    CAMERA_SHAKE_DESC m_tDesc = {};
    Vec3   m_vCurLocalPosOffset = Vec3::Zero;
    _float m_fCurYawOffset = 0.f;
    _float m_fCurPitchOffset = 0.f;
public:
    static CCameraModifier_Shake* Create(const CAMERA_SHAKE_DESC& desc);
    virtual void Free() override;
};

NS_END