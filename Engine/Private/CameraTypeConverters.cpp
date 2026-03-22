#include "Engine_pch.h"
#include "CameraTypeConverters.h"

static Vec3 ToVec3(const Float3& v)
{
    return Vec3(v.x, v.y, v.z);
}

CAMERA_SHAKE_DESC ToRuntimeDesc(const CAMERA_SHAKE_DATA& data)
{
    CAMERA_SHAKE_DESC desc;
    desc.fPosAmplitude = data.fPosAmplitude;
    desc.fYawAmplitudeRad = XMConvertToRadians(data.fYawAmplitudeDeg);
    desc.fPitchAmplitudeRad = XMConvertToRadians(data.fPitchAmplitudeDeg);
    desc.fFrequency = data.fFrequency;
    desc.fDuration = data.fDuration;
    return desc;
}

CAMERA_FOV_DESC ToRuntimeDesc(const CAMERA_FOV_DATA& data)
{
    CAMERA_FOV_DESC desc;
    desc.eMode = data.eMode;
    desc.fValueRad = XMConvertToRadians(data.fValueDeg);
    desc.fBlendInTime = data.fBlendInTime;
    desc.fHoldTime = data.fHoldTime;
    desc.fBlendOutTime = data.fBlendOutTime;
    desc.bRestoreOnFinish = data.bRestoreOnFinish;
    return desc;
}

CAMERA_POSITION_OFFSET_DESC ToRuntimeDesc(const CAMERA_POSITION_OFFSET_DATA& data)
{
    CAMERA_POSITION_OFFSET_DESC desc;
    desc.vOffset = ToVec3(data.vOffset);
    desc.eSpace = data.eSpace;
    desc.fBlendInTime = data.fBlendInTime;
    desc.fHoldTime = data.fHoldTime;
    desc.fBlendOutTime = data.fBlendOutTime;
    return desc;
}

CAMERA_ROTATION_OFFSET_DESC ToRuntimeDesc(const CAMERA_ROTATION_OFFSET_DATA& data)
{
    CAMERA_ROTATION_OFFSET_DESC desc;
    desc.fYawRad = XMConvertToRadians(data.fYawDeg);
    desc.fPitchRad = XMConvertToRadians(data.fPitchDeg);
    desc.fRollRad = XMConvertToRadians(data.fRollDeg);
    desc.fBlendInTime = data.fBlendInTime;
    desc.fHoldTime = data.fHoldTime;
    desc.fBlendOutTime = data.fBlendOutTime;
    return desc;
}