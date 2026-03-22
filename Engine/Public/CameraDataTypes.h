#pragma once
#include "CameraCommonTypes.h"

/////////////////////////
// Data´Â Degree ±âÁØ!! //
/////////////////////////

namespace Engine
{
    struct Float3
    {
        float x = 0.f;
        float y = 0.f;
        float z = 0.f;
    };

    typedef struct tagCameraShakeData
    {
        float fPosAmplitude = 0.05f;
        float fYawAmplitudeDeg = 0.5f;
        float fPitchAmplitudeDeg = 0.5f;
        float fFrequency = 25.f;
        float fDuration = 0.15f;
    }CAMERA_SHAKE_DATA;

    typedef struct tagCameraFovData
    {
        ECameraFovMode eMode = ECameraFovMode::Delta;
        float fValueDeg = 0.f;

        float fBlendInTime = 0.05f;
        float fHoldTime = 0.f;
        float fBlendOutTime = 0.05f;

        bool bRestoreOnFinish = true;
    }CAMERA_FOV_DATA;

    typedef struct tagCameraPositionOffsetData
    {
        Float3 vOffset = {};
        ECameraSpace eSpace = ECameraSpace::Camera_Local;

        float fBlendInTime = 0.05f;
        float fHoldTime = 0.f;
        float fBlendOutTime = 0.05f;
    }CAMERA_POSITION_OFFSET_DATA;

    typedef struct tagCameraRotationOffsetData
    {
        float fYawDeg = 0.f;
        float fPitchDeg = 0.f;
        float fRollDeg = 0.f;

        float fBlendInTime = 0.05f;
        float fHoldTime = 0.f;
        float fBlendOutTime = 0.05f;
    }CAMERA_ROTATION_OFFSET_DATA;
}