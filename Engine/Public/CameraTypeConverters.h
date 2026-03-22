#pragma once
#include "CameraDataTypes.h"
#include "CameraRuntimeTypes.h"

NS_BEGIN(Engine)

CAMERA_SHAKE_DESC           ToRuntimeDesc(const CAMERA_SHAKE_DATA& data);
CAMERA_FOV_DESC             ToRuntimeDesc(const CAMERA_FOV_DATA& data);
CAMERA_POSITION_OFFSET_DESC ToRuntimeDesc(const CAMERA_POSITION_OFFSET_DATA& data);
CAMERA_ROTATION_OFFSET_DESC ToRuntimeDesc(const CAMERA_ROTATION_OFFSET_DATA& data);

NS_END