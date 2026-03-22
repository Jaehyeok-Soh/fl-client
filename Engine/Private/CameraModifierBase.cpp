#include "Engine_pch.h"
#include "CameraModifierBase.h"

void CCameraModifierBase::Start()
{
    m_fElapsed = 0.f;
    m_bStarted = true;
    m_bFinished = false;
}

void CCameraModifierBase::Update(_float fTimeDelta)
{
    if (m_bFinished)
        return;

    if (!m_bStarted)
        Start();

    m_fElapsed += fTimeDelta;

    const _float fTotal = Get_TotalDuration();
    if (m_fElapsed >= fTotal)
    {
        m_fElapsed = fTotal;
        m_bFinished = true;
    }
}

_float CCameraModifierBase::Get_BlendWeight() const
{
    const _float fInStart = 0.f;
    const _float fInEnd = m_fBlendInTime;

    const _float fHoldStart = fInEnd;
    const _float fHoldEnd = fHoldStart + m_fHoldTime;

    const _float fOutStart = fHoldEnd;
    const _float fOutEnd = fOutStart + m_fBlendOutTime;

    if (m_fElapsed <= 0.f)
        return 0.f;

    if (m_fElapsed < fInEnd)
    {
        if (m_fBlendInTime <= 0.f)
            return 1.f;

        return std::clamp(m_fElapsed / m_fBlendInTime, 0.f, 1.f);
    }

    if (m_fElapsed < fHoldEnd)
        return 1.f;

    if (m_fElapsed < fOutEnd)
    {
        if (m_fBlendOutTime <= 0.f)
            return 0.f;

        const _float fT = (m_fElapsed - fOutStart) / m_fBlendOutTime;
        return std::clamp(1.f - fT, 0.f, 1.f);
    }

    return 0.f;
}

_float CCameraModifierBase::Rand_Signed() const
{
    return ((_float)rand() / (_float)RAND_MAX) * 2.f - 1.f;
}
