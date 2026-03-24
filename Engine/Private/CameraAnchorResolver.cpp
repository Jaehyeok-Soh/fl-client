#include "Engine_pch.h"
#include "GameObject.h"
#include "ICameraAnchorHost.h"
#include "CameraAnchorResolver.h"

void CCameraAnchorResolver::Apply_LocalOffset(OUT CAMERA_ANCHOR_RESULT& outResult, const Vec3& vLocalOffset)
{
    outResult.vPos += outResult.vRight * vLocalOffset.x;
    outResult.vPos += outResult.vUp * vLocalOffset.y;
    outResult.vPos += outResult.vLook * vLocalOffset.z;
}

_bool CCameraAnchorResolver::Resolve_FromTransform(CGameObject* pOwner, OUT CAMERA_ANCHOR_RESULT& outResult)
{
    if (pOwner == nullptr)
        return false;

    CTransform* pTransform = pOwner->Get_Component<CTransform>();
    if (pTransform == nullptr)
        return false;

    outResult.vPos = pTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
    outResult.vRight = pTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    outResult.vUp = pTransform->Get_Info(TRANSFORM_INFO_STATE::UP);
    outResult.vLook = pTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);

    Normalize_AnchorResult(outResult);
    return true;
}

_bool CCameraAnchorResolver::Resolve_FromProvider(
    CGameObject* pTargetObject,
    const CAMERA_BIND_TARGET_DESC& tTarget,
    OUT CAMERA_ANCHOR_RESULT& outResult)
{
    if (pTargetObject == nullptr)
        return false;

    ICameraAnchorHost* pHost = dynamic_cast<ICameraAnchorHost*>(pTargetObject);
    if (pHost == nullptr)
        return false;

    ICameraAnchorProvider* pProvider = pHost->Get_CameraAnchorProvider(tTarget.iPartIndex);
    CTransform* pOwnerTransform = pHost->Get_CameraAnchorOwnerTransform();

    if (pProvider == nullptr || pOwnerTransform == nullptr)
        return false;

    const Matrix matOwnerWorld = pOwnerTransform->Get_WorldMatrix();

    if (pProvider->Resolve_CameraAnchor(
        tTarget.eResolve,
        tTarget.strAnchorTag,
        matOwnerWorld,
        outResult) == false)
    {
        return false;
    }

    Normalize_AnchorResult(outResult);
    return true;
}

_bool CCameraAnchorResolver::Resolve(const CAMERA_BIND_TARGET_DESC& tTarget, CGameObject* pDefaultActor, OUT CAMERA_ANCHOR_RESULT& outResult)
{
    outResult = {};

    CGameObject* pTargetObject = nullptr;

    //////////////////////
    // Source Resolve
    //////////////////////
    switch (tTarget.eSource)
    {
    case ECameraAnchorSource::ACTOR:
        pTargetObject = pDefaultActor;
        break;

    case ECameraAnchorSource::OBJECT:
        pTargetObject = tTarget.pObject;
        break;

    case ECameraAnchorSource::WORLDPOINT:
        outResult.vPos = tTarget.vWorldPoint;
        outResult.vRight = Vec3::Right;
        outResult.vUp = Vec3::Up;
        outResult.vLook = Vec3::Backward;

        Normalize_AnchorResult(outResult);
        Apply_LocalOffset(outResult, tTarget.vLocalOffset);
        return true;

    default:
        return false;
    }

    if (pTargetObject == nullptr)
        return false;

    //////////////////////
    // Resolve Route
    //////////////////////
    _bool bSuccess = false;

    switch (tTarget.eResolve)
    {
    case ECameraAnchorResolve::TRANSFORM:
        bSuccess = Resolve_FromTransform(pTargetObject, outResult);
        break;

    case ECameraAnchorResolve::CAM_SOCKET:
    case ECameraAnchorResolve::BONE:
        bSuccess = Resolve_FromProvider(pTargetObject, tTarget, outResult);
        break;

    default:
        return false;
    }

    if (bSuccess == false)
        return false;

    //////////////////////
    // Anchor Offset
    //////////////////////
    Apply_LocalOffset(outResult, tTarget.vLocalOffset);

    return true;
}

void CCameraAnchorResolver::Normalize_AnchorResult(OUT CAMERA_ANCHOR_RESULT& outResult)
{
    if (outResult.vRight.LengthSquared() > g_XMEpsilon.f[0])
        outResult.vRight.Normalize();
    else
        outResult.vRight = Vec3::Right;

    if (outResult.vUp.LengthSquared() > g_XMEpsilon.f[0])
        outResult.vUp.Normalize();
    else
        outResult.vUp = Vec3::Up;

    if (outResult.vLook.LengthSquared() > g_XMEpsilon.f[0])
        outResult.vLook.Normalize();
    else
        outResult.vLook = Vec3::Backward;
}