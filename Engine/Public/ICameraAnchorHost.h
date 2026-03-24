#pragma once
#include <string>
#include "Engine_Define.h"
#include "ICameraAnchorProvider.h"

NS_BEGIN(Engine)

class CTransform;

class ICameraAnchorHost
{
public:
public:
    virtual ~ICameraAnchorHost() = default;

    virtual ICameraAnchorProvider* Get_CameraAnchorProvider(_int iPartIndex = 0) const PURE;
    virtual CTransform* Get_CameraAnchorOwnerTransform() const PURE;
};

NS_END