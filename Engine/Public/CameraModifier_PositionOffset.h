#pragma once
#include "CameraModifierBase.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

class CCameraModifier_PositionOffset final : public CCameraModifierBase
{
    using Super = CCameraModifierBase;
private:
    explicit CCameraModifier_PositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc);
    CCameraModifier_PositionOffset(const CCameraModifier_PositionOffset&) = default;
    virtual ~CCameraModifier_PositionOffset() = default;
public:
    virtual ECameraModifierType Get_Type() const override { return ECameraModifierType::Position_Offset; }
    virtual void Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const override;
private:
    CAMERA_POSITION_OFFSET_DESC m_tDesc = {};
public:
    static CCameraModifier_PositionOffset* Create(const CAMERA_POSITION_OFFSET_DESC& desc);
    virtual void Free() override;
};

NS_END