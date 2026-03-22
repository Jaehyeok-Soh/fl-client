#include "Engine_pch.h"
#include "CameraModifier_RotationOffset.h"

CCameraModifier_RotationOffset::CCameraModifier_RotationOffset(const CAMERA_ROTATION_OFFSET_DESC& desc)
	: Super()
	, m_tDesc(desc)
{
	m_fBlendInTime = (std::max)(0.f, desc.fBlendInTime);
	m_fHoldTime = (std::max)(0.f, desc.fHoldTime);
	m_fBlendOutTime = (std::max)(0.f, desc.fBlendOutTime);
}

void CCameraModifier_RotationOffset::Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const
{
	const _float fWeight = Get_BlendWeight();

	outResult.fYawOffsetRad += m_tDesc.fYawRad * fWeight;
	outResult.fPitchOffsetRad += m_tDesc.fPitchRad * fWeight;
	outResult.fRollOffsetRad += m_tDesc.fRollRad * fWeight;
}

CCameraModifier_RotationOffset* CCameraModifier_RotationOffset::Create(const CAMERA_ROTATION_OFFSET_DESC& desc)
{
	return new CCameraModifier_RotationOffset(desc);
}

void CCameraModifier_RotationOffset::Free()
{
	Super::Free();
}