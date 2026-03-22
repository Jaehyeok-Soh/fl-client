#pragma once
#include "CameraModifierBase.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class CCameraModifier_Fov final : public CCameraModifierBase
{
    using Super = CCameraModifierBase;
private:
    explicit CCameraModifier_Fov(const CAMERA_FOV_DESC& desc);
    CCameraModifier_Fov(const CCameraModifier_Fov&) = default;
    virtual ~CCameraModifier_Fov() = default;
public:
    virtual ECameraModifierType Get_Type() const override { return ECameraModifierType::Fov; }
    virtual void Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const override;
private:
    CAMERA_FOV_DESC m_tDesc = {};
public:
    static CCameraModifier_Fov* Create(const CAMERA_FOV_DESC& desc);
    virtual void Free() override;
};

NS_END