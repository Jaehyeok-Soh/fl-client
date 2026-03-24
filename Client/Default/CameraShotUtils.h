#pragma once
#include "Base.h"

_float Eval_ShotEase(ECameraShotEase eEase, _float fT);
void Shot_Channel(CAMERA_SHOT_CHANNEL_1D& tChannel);
_float Eval_Channel1D(const CAMERA_SHOT_CHANNEL_1D& tChannel, _float fTime);