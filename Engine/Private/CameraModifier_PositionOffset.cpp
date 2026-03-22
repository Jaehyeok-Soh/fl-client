#include "Engine_pch.h"
#include "CameraModifier_PositionOffset.h"

CCameraModifier_PositionOffset::CCameraModifier_PositionOffset(const CAMERA_POSITION_OFFSET_DESC& desc)
	: Super()
	, m_tDesc(desc)
{
	m_fBlendInTime = (std::max)(0.f, desc.fBlendInTime);
	m_fHoldTime = (std::max)(0.f, desc.fHoldTime);
	m_fBlendOutTime = (std::max)(0.f, desc.fBlendOutTime);
}

void CCameraModifier_PositionOffset::Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const
{
    const _float fWeight = Get_BlendWeight();
    const Vec3 vOffset = m_tDesc.vOffset * fWeight;

    switch (m_tDesc.eSpace)
    {
    case ECameraSpace::World:
        outResult.vWorldPosOffset += vOffset;
        break;

    case ECameraSpace::Camera_Local:
        outResult.vLocalPosOffset += vOffset;
        break;
    }
}

CCameraModifier_PositionOffset* CCameraModifier_PositionOffset::Create(const CAMERA_POSITION_OFFSET_DESC& desc)
{
	return new CCameraModifier_PositionOffset(desc);
}

void CCameraModifier_PositionOffset::Free()
{
	Super::Free();
}