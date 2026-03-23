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
    if (!m_bStarted || m_bFinished)
        return;

    _float fWeight = Get_BlendWeight();
    if (fWeight <= 0.f)
        return;

    // 회전은 들어갈 때 살짝 과장하면 덜 심심함
    if (m_fBlendInTime > 0.f && m_fElapsed < m_fBlendInTime)
    {
        const _float fT = m_fElapsed / m_fBlendInTime;
        fWeight = Engine_Utils::EvalEase_EaseOutBack(fT);
    }

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