#include "Engine_pch.h"
#include "CameraModifier_Fov.h"

CCameraModifier_Fov::CCameraModifier_Fov(const CAMERA_FOV_DESC& desc)
	: Super()
	, m_tDesc(desc)
{
	m_fBlendInTime = (std::max)(0.f, desc.fBlendInTime);
	m_fHoldTime = (std::max)(0.f, desc.fHoldTime);
	m_fBlendOutTime = (std::max)(0.f, desc.fBlendOutTime);
}

void CCameraModifier_Fov::Accumulate(const CAMERA_POSE& basePose, OUT CAMERA_MODIFIER_RESULT& outResult) const
{
	const _float fWeight = Get_BlendWeight();
	_float fDelta = 0.f;

	switch (m_tDesc.eMode)
	{
	case ECameraFovMode::Delta:
		fDelta = m_tDesc.fValueRad * fWeight;
		break;
	case ECameraFovMode::Absolute:
		fDelta = (m_tDesc.fValueRad - basePose.fFovRad) * fWeight;
		break;
	}

	outResult.fFovDeltaRad += fDelta;
}

CCameraModifier_Fov* CCameraModifier_Fov::Create(const CAMERA_FOV_DESC& desc)
{
	return new CCameraModifier_Fov(desc);
}

void CCameraModifier_Fov::Free()
{
	Super::Free();
}
