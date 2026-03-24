#pragma once
#include <string>
#include "Engine_Define.h"
#include "ICameraAnchorProvider.h"

///////////////////////////////////////////////////
// Character 계열이 구현할 인터페이스                //
// provider를 내놓고 owner transform을 내놓는 역할  //
// 이 인터페이스 구조로 카메라는 타입에 대해서 몰라도됨 //
///////////////////////////////////////////////////

NS_BEGIN(Engine)

class CTransform;

class ICameraAnchorHost
{
public:
public:
    virtual ~ICameraAnchorHost() = default;

    virtual ICameraAnchorProvider* Get_CameraAnchorProvider(_int iPartIndex = 0) PURE;
    virtual CTransform* Get_CameraAnchorOwnerTransform() PURE;
};

NS_END