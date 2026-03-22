#include "Engine_pch.h"
#include "CameraModifierBase.h"

void CCameraModifierBase::Start()
{
    if (m_bStarted)
        return;

    m_fElapsed = 0.f;
    m_bStarted = true;
    m_bFinished = false;
}

void CCameraModifierBase::Update(_float fTimeDelta)
{
    if (m_bFinished)
        return;

    if (m_bStarted == false)
        Start();

    m_fElapsed += fTimeDelta;

    const _float fTotal = Get_TotalDuration();
    if (fTotal <= 0.f)
    {
        m_bFinished = true;
        return;
    }

    if (m_fElapsed >= fTotal)
    {
        m_fElapsed = fTotal;
        m_bFinished = true;
    }
}

_float CCameraModifierBase::Get_BlendWeight() const
{
    if (!m_bStarted)
        return 0.f;

    if (Get_TotalDuration() <= 0.f)
        return 1.f;

    return Engine_Utils::EvalTimingWeight(
        m_fElapsed,
        m_fBlendInTime,
        m_fHoldTime,
        m_fBlendOutTime);
}

_float CCameraModifierBase::Rand_Signed() const
{
    return ((_float)rand() / (_float)RAND_MAX) * 2.f - 1.f;
}
