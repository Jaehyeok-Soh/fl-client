#include "pch.h"
#include "GameObject.h"
#include "CameraShotPrestBuilder.h"

namespace
{
    Engine::CAMERA_SHOT_CHANNEL_1D Make_Channel(std::initializer_list<Engine::CAMERA_SHOT_KEY_1D> keys)
    {
        Engine::CAMERA_SHOT_CHANNEL_1D ch;
        ch.vecKeys.assign(keys.begin(), keys.end());
        return ch;
    }
}

SCRIPTED_CAMERA_SHOT_DESC CCameraShotPrestBuilder::Make_PlayerPullBackShot()
{
    Engine::SCRIPTED_CAMERA_SHOT_DESC tDesc = {};
    tDesc.strName = "DebugPlain5SecShot";

    auto Make_Channel = [](std::initializer_list<Engine::CAMERA_SHOT_KEY_1D> keys)
        {
            Engine::CAMERA_SHOT_CHANNEL_1D ch;
            ch.vecKeys.assign(keys.begin(), keys.end());
            return ch;
        };

    auto& Pivot = tDesc.Pivot;
    Pivot.fDuration = 5.0f;
    Pivot.bFollowLivePivot = true;
    Pivot.bFollowLiveLookAt = true;
    Pivot.bLookAtTarget = true;
    Pivot.eBasisMode = Engine::ECameraBasisMode::WORLD;

    // pivot/lookat 자체 이동은 디버그 1차에서는 끔
    Pivot.PivotOffsetX = {};
    Pivot.PivotOffsetY = {};
    Pivot.PivotOffsetZ = {};

    Pivot.LookAtOffsetX = {};
    Pivot.LookAtOffsetY = {};
    Pivot.LookAtOffsetZ = {};

    // 시작 위치에서 너무 확 튀지 않게, 천천히 뒤로만 빠짐
    Pivot.LocalX = {};
    Pivot.LocalY = {};
    Pivot.LocalZ = Make_Channel({
        {0.00f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        {1.20f, 1.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {5.00f, 1.0f, Engine::ECameraShotEase::SmoothStep},
        });

    // orbit은 아주 약하게만
    Pivot.OrbitYawDeg = Make_Channel({
        {0.00f,  0.0f, Engine::ECameraShotEase::SmoothStep},
        {2.50f,  8.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {5.00f,  0.0f, Engine::ECameraShotEase::EaseInOutQuad},
        });

    auto& Ctrl = tDesc.Controller;

    // 디버그용은 controller 영향 제거
    Ctrl.FovDeltaDeg = {};
    Ctrl.RotYawDeg = {};
    Ctrl.RotPitchDeg = {};
    Ctrl.RotRollDeg = {};
    Ctrl.LocalPosX = {};
    Ctrl.LocalPosY = {};
    Ctrl.LocalPosZ = {};

    return tDesc;
}

SCRIPTED_CAMERA_SHOT_DESC CCameraShotPrestBuilder::Make_BossHeadOrbitShot()
{
    Engine::SCRIPTED_CAMERA_SHOT_DESC tDesc = {};
    tDesc.strName = "BossHeadOrbitShot";

    auto& Pivot = tDesc.Pivot;
    Pivot.fDuration = 0.9f;
    Pivot.bFollowLivePivot = true;
    Pivot.bFollowLiveLookAt = true;
    Pivot.bLookAtTarget = true;
    Pivot.eBasisMode = Engine::ECameraBasisMode::ANCHOR_OWNER;

    Pivot.PivotOffsetY = Make_Channel({
        {0.00f, 0.0f,  Engine::ECameraShotEase::SmoothStep},
        {0.30f, 0.12f, Engine::ECameraShotEase::EaseOutQuad},
        {0.90f, 0.0f,  Engine::ECameraShotEase::SmoothStep},
        });

    Pivot.LookAtOffsetY = Make_Channel({
        {0.00f, 0.0f,  Engine::ECameraShotEase::SmoothStep},
        {0.20f, 0.08f, Engine::ECameraShotEase::EaseOutQuad},
        {0.90f, 0.0f,  Engine::ECameraShotEase::SmoothStep},
        });

    Pivot.LocalZ = Make_Channel({
        {0.00f, 0.0f,  Engine::ECameraShotEase::SmoothStep},
        {0.10f, 1.2f,  Engine::ECameraShotEase::EaseOutQuad},
        {0.32f, 2.3f,  Engine::ECameraShotEase::EaseOutBack},
        {0.60f, 1.0f,  Engine::ECameraShotEase::EaseInOutQuad},
        {0.90f, 1.8f,  Engine::ECameraShotEase::SmoothStep},
        });

    Pivot.OrbitYawDeg = Make_Channel({
        {0.00f,  0.0f, Engine::ECameraShotEase::SmoothStep},
        {0.25f, 15.0f, Engine::ECameraShotEase::EaseOutQuad},
        {0.55f, -8.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {0.90f,  0.0f, Engine::ECameraShotEase::SmoothStep},
        });

    auto& Ctrl = tDesc.Controller;
    Ctrl.FovDeltaDeg = Make_Channel({
        {0.00f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        {0.18f, 8.0f, Engine::ECameraShotEase::EaseOutQuad},
        {0.90f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        });

    Ctrl.RotRollDeg = Make_Channel({
        {0.00f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        {0.30f, 3.0f, Engine::ECameraShotEase::EaseOutQuad},
        {0.55f, -2.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {0.90f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        });

    return tDesc;
}

SCRIPTED_CAMERA_SHOT_BINDING_DESC CCameraShotPrestBuilder::Make_PlayerCamSocketBinding()
{
    Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC tBinding = {};

    tBinding.pviot.eSource = Engine::ECameraAnchorSource::ACTOR;
    tBinding.pviot.eResolve = Engine::ECameraAnchorResolve::CAM_SOCKET;
    tBinding.pviot.iPartIndex = 0;
    tBinding.pviot.vLocalOffset = Vec3::Zero;

    tBinding.bUseSeparateLookAt = false;

    return tBinding;
}

SCRIPTED_CAMERA_SHOT_BINDING_DESC CCameraShotPrestBuilder::Make_BossHeadBinding(CGameObject* pBoss)
{
    Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC tBinding = {};

    tBinding.pviot.eSource = Engine::ECameraAnchorSource::OBJECT;
    tBinding.pviot.pObject = pBoss;
    tBinding.pviot.eResolve = Engine::ECameraAnchorResolve::CAM_SOCKET;
    tBinding.LookAt.iPartIndex = 0;
    tBinding.pviot.vLocalOffset = Vec3(0.f, 0.08f, 0.f);

    tBinding.bUseSeparateLookAt = true;

    tBinding.LookAt.eSource = Engine::ECameraAnchorSource::OBJECT;
    tBinding.LookAt.pObject = pBoss;
    tBinding.LookAt.eResolve = Engine::ECameraAnchorResolve::BONE;
    tBinding.LookAt.iPartIndex = 0;
    tBinding.LookAt.strAnchorTag = "foot_r";
    tBinding.LookAt.vLocalOffset = Vec3(0.f, 0.03f, 0.f);

    return tBinding;
}
