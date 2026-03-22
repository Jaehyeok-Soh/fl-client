#pragma once
#include "Base.h"
#include "CameraCommonTypes.h"


/////////////////////////////
// Runtime ¿∫ Radian ±‚¡ÿ!! //
/////////////////////////////

NS_BEGIN(Engine)

typedef struct tagCameraPose
{
    Vec3   vPos = Vec3::Zero;
    Vec3   vRight = Vec3::Right;
    Vec3   vUp = Vec3::Up;
    Vec3   vLook = Vec3::Backward;
    _float fFovRad = XM_PIDIV4;
}CAMERA_POSE;

typedef struct tagCameraModifierResult
{
    Vec3   vWorldPosOffset = Vec3::Zero;
    Vec3   vLocalPosOffset = Vec3::Zero;

    _float fYawOffsetRad = 0.f;
    _float fPitchOffsetRad = 0.f;
    _float fRollOffsetRad = 0.f;

    _float fFovDeltaRad = 0.f;
}CAMERA_MODIFIER_RESULT;

typedef struct tagCameraControllerDesc
{
    _float fMinFovRad = XMConvertToRadians(20.f);
    _float fMaxFovRad = XMConvertToRadians(100.f);
}CAMERA_CONTROLLER_DESC;

typedef struct tagCameraShakeDesc
{
    _float fPosAmplitude = 0.05f;
    _float fYawAmplitudeRad = XMConvertToRadians(0.5f);
    _float fPitchAmplitudeRad = XMConvertToRadians(0.5f);
    _float fFrequency = 25.f;
    _float fDuration = 0.15f;
}CAMERA_SHAKE_DESC;

typedef struct tagCameraFovDesc
{
    ECameraFovMode eMode = ECameraFovMode::Delta;
    _float fValueRad = 0.f;

    _float fBlendInTime = 0.05f;
    _float fHoldTime = 0.f;
    _float fBlendOutTime = 0.05f;

    _bool  bRestoreOnFinish = true;
}CAMERA_FOV_DESC;

typedef struct tagCameraPositionOffsetDesc
{
    Vec3         vOffset = Vec3::Zero;
    ECameraSpace eSpace = ECameraSpace::Camera_Local;

    _float fBlendInTime = 0.05f;
    _float fHoldTime = 0.f;
    _float fBlendOutTime = 0.05f;
}CAMERA_POSITION_OFFSET_DESC;

typedef struct tagCameraRotationOffsetDesc
{
    _float fYawRad = 0.f;
    _float fPitchRad = 0.f;
    _float fRollRad = 0.f;

    _float fBlendInTime = 0.05f;
    _float fHoldTime = 0.f;
    _float fBlendOutTime = 0.05f;
}CAMERA_ROTATION_OFFSET_DESC;

NS_END