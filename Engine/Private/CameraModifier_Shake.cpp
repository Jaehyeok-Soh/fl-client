#include "Engine_pch.h"
#include "CameraModifierBase.h"
#include "CameraModifier_Shake.h"

CCameraModifier_Shake::CCameraModifier_Shake(const CAMERA_SHAKE_DESC& desc)
	: Super()
	, m_tDesc(desc)
{
	m_fBlendInTime = 0.f;
	m_fHoldTime = (std::max)(0.f, desc.fDuration);
	m_fBlendOutTime = 0.f;
}

void CCameraModifier_Shake::Update(_float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (IsFinished() == true)
	{
		m_vCurLocalPosOffset = Vec3::Zero;
		m_fCurYawOffset = 0.f;
		m_fCurPitchOffset = 0.f;
		return;
	}

	const _float fWeight = Get_BlendWeight();

	m_vCurLocalPosOffset = Vec3(
		Rand_Signed() * m_tDesc.fPosAmplitude,
		Rand_Signed() * m_tDesc.fPosAmplitude,
		Rand_Signed() * m_tDesc.fPosAmplitude) * fWeight;

	m_fCurYawOffset = Rand_Signed() * m_tDesc.fYawAmplitudeRad * fWeight;
	m_fCurPitchOffset = Rand_Signed() * m_tDesc.fPitchAmplitudeRad * fWeight;
}

void CCameraModifier_Shake::Accumulate(const CAMERA_POSE& basePose, CAMERA_MODIFIER_RESULT& outResult) const
{
	outResult.vLocalPosOffset += m_vCurLocalPosOffset;
	outResult.fYawOffsetRad += m_fCurYawOffset;
	outResult.fPitchOffsetRad += m_fCurPitchOffset;
}

CCameraModifier_Shake* CCameraModifier_Shake::Create(const CAMERA_SHAKE_DESC& desc)
{
	return new CCameraModifier_Shake(desc);
}

void CCameraModifier_Shake::Free()
{
	Super::Free();
}
