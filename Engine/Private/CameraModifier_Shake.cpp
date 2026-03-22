#include "Engine_pch.h"
#include "CameraModifierBase.h"
#include "CameraModifier_Shake.h"

CCameraModifier_Shake::CCameraModifier_Shake(const CAMERA_SHAKE_DESC& desc)
	: Super()
	, m_tDesc(desc)
{
	const _float fAttack = std::min(0.05f, m_tDesc.fDuration * 0.15f);

	m_fBlendInTime = fAttack;
	m_fHoldTime = 0.f;
	m_fBlendOutTime = (std::max)(0.f, m_tDesc.fDuration - fAttack);
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
	if (!m_bStarted || m_bFinished)
		return;

	const _float fWeight = Get_BlendWeight();
	if (fWeight <= 0.f)
		return;

	const _float fPhase = m_fElapsed * m_tDesc.fFrequency * XM_2PI;

	const _float sx = std::sinf(fPhase * 1.00f + 0.17f);
	const _float sy = std::sinf(fPhase * 1.37f + 1.11f);
	const _float sz = std::sinf(fPhase * 1.91f + 2.53f);

	const _float syaw = std::sinf(fPhase * 1.23f + 0.42f);
	const _float spitch = std::sinf(fPhase * 1.61f + 0.89f);

	outResult.vLocalPosOffset += Vec3(sx, sy, sz) * (m_tDesc.fPosAmplitude * fWeight);
	outResult.fYawOffsetRad += syaw * (m_tDesc.fYawAmplitudeRad * fWeight);
	outResult.fPitchOffsetRad += spitch * (m_tDesc.fPitchAmplitudeRad * fWeight);
}

CCameraModifier_Shake* CCameraModifier_Shake::Create(const CAMERA_SHAKE_DESC& desc)
{
	return new CCameraModifier_Shake(desc);
}

void CCameraModifier_Shake::Free()
{
	Super::Free();
}
