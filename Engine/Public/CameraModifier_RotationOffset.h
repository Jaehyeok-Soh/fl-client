#pragma once
#include "CameraModifierBase.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class CCameraModifier_RotationOffset final : public CCameraModifierBase
{
    using Super = CCameraModifierBase;
private:
    explicit CCameraModifier_RotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc);
    CCameraModifier_RotationOffset(const CCameraModifier_RotationOffset&) = default;
    virtual ~CCameraModifier_RotationOffset() = default;
public:
    virtual ECameraModifierType Get_Type() const override { return ECameraModifierType::Rotation_Offset; }
    virtual void Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const override;
private:
    CAMERA_ROTATION_OFFSET_DESC m_tDesc = {};
public:
    static CCameraModifier_RotationOffset* Create(const CAMERA_ROTATION_OFFSET_DESC& desc);
    virtual void Free() override;
};

NS_END


