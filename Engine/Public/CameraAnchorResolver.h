#pragma once
#include "Engine_Define.h"

////////////////////////////////////////
// 카메라가 실제로 쓸 공용 resolve 진입점 //
// 오직 ICameraAnchorHost만 바라본다    //
// source 해석, transform resolve      //
// host -> provider 루트로 resolve     //
// vLocalOffset 적용까지               //
////////////////////////////////////////

NS_BEGIN(Engine)

class CGameObject;

class CCameraAnchorResolver final
{
public:
    static _bool Resolve(
        const CAMERA_BIND_TARGET_DESC& tTarget,
        CGameObject* pDefaultActor,
        OUT CAMERA_ANCHOR_RESULT& outResult);
};

NS_END