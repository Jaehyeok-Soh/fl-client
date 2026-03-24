#include "pch.h"
#include "CameraShotUtils.h"

_float Eval_ShotEase(ECameraShotEase eEase, _float fT)
{
    fT = std::clamp(fT, 0.f, 1.f);

    switch (eEase)
    {
    case ECameraShotEase::Linear:
        return fT;

    case ECameraShotEase::SmoothStep:
        return Engine_Utils::EvalEase_SmoothStep(fT);

    case ECameraShotEase::EaseOutQuad:
        return Engine_Utils::EvalEase_EaseOutQuad(fT);

    case ECameraShotEase::EaseInOutQuad:
        return Engine_Utils::EvalEase_EaseInOutQuad(fT);

    case ECameraShotEase::EaseOutBack:
        return Engine_Utils::EvalEase_EaseOutBack(fT);
    }

    return fT;
}

void Shot_Channel(CAMERA_SHOT_CHANNEL_1D& tChannel)
{
    std::sort(tChannel.vecKeys.begin(), tChannel.vecKeys.end(),
        [](const auto& a, const auto& b) { return a.fTime < b.fTime; });
}

_float Eval_Channel1D(const CAMERA_SHOT_CHANNEL_1D& tChannel, _float fTime)
{
    const auto& Keys = tChannel.vecKeys;
    if (Keys.empty())
        return 0.f;

    if (Keys.size() == 1)
        return Keys[0].fValue;

    if (fTime <= Keys.front().fTime)
        return Keys.front().fValue;

    if (fTime >= Keys.back().fTime)
        return Keys.back().fValue;

    for (size_t i = 0; i + 1 < Keys.size(); ++i)
    {
        const auto& k0 = Keys[i];
        const auto& k1 = Keys[i + 1];

        if (fTime >= k0.fTime && fTime <= k1.fTime)
        {
            const _float fLen = k1.fTime - k0.fTime;
            if (fLen <= g_XMEpsilon.f[0])
                return k1.fValue;

            _float fT = (fTime - k0.fTime) / fLen;
            fT = Eval_ShotEase(k0.eEase, fT);
            return std::lerp(k0.fValue, k1.fValue, fT);
        }
    }

    return Keys.back().fValue;
}
