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
    if (m_bStarted == false || m_bFinished)
        return;

    const _float fWeight = Get_BlendWeight();
    if (fWeight <= 0.f)
        return;

    _float fDeltaRad = 0.f;

    if (m_tDesc.eMode == ECameraFovMode::Delta)
        fDeltaRad = m_tDesc.fValueRad;
    else
        fDeltaRad = (m_tDesc.fValueRad - basePose.fFovRad);

    outResult.fFovDeltaRad += fDeltaRad * fWeight;
}

CCameraModifier_Fov* CCameraModifier_Fov::Create(const CAMERA_FOV_DESC& desc)
{
	return new CCameraModifier_Fov(desc);
}

void CCameraModifier_Fov::Free()
{
	Super::Free();
}
