#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class CGameObject;

class CCameraAnchorResolver final
{
public:
    static _bool Resolve(
        const CAMERA_BIND_TARGET_DESC& tTarget,
        CGameObject* pDefaultActor,
        CAMERA_ANCHOR_RESULT& outResult);
};

NS_END