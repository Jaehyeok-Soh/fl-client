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


Engine::SCRIPTED_CAMERA_SHOT_DESC CCameraShotPrestBuilder::Make_DebugPlain5SecShot()
{
    Engine::SCRIPTED_CAMERA_SHOT_DESC tDesc = {};
    tDesc.strName = "DebugPlain5SecShot";

    auto& Start = tDesc.Start;
    Start.eMode = Engine::ECameraShotStartMode::FixedFromPivot;
    Start.vLocaloffset = Vec3(0.f, 0.5f, 3.0f);
    Start.bApplyStartPoseImmediately = true;

    auto& Pivot = tDesc.Pivot;
    Pivot.fDuration = 5.0f;
    Pivot.bFollowLivePivot = true;
    Pivot.bFollowLiveLookAt = true;
    Pivot.bLookAtTarget = true;
    Pivot.eBasisMode = Engine::ECameraBasisMode::WORLD;

    Pivot.PivotOffsetX = {};
    Pivot.PivotOffsetY = {};
    Pivot.PivotOffsetZ = {};

    Pivot.LookAtOffsetX = {};
    Pivot.LookAtOffsetY = {};
    Pivot.LookAtOffsetZ = {};

    Pivot.LocalX = {};
    Pivot.LocalY = {};
    Pivot.LocalZ = Make_Channel({
        {0.00f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        {1.20f, 1.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {5.00f, 1.0f, Engine::ECameraShotEase::SmoothStep},
        });

    Pivot.OrbitYawDeg = Make_Channel({
        {0.00f, 0.0f, Engine::ECameraShotEase::SmoothStep},
        {2.50f, 8.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {5.00f, 0.0f, Engine::ECameraShotEase::EaseInOutQuad},
        });

    tDesc.Controller = {};

    auto& Recover = tDesc.Recover;
    Recover.eTarget = Engine::ECameraShotRecoverTarget::GameplaySolved;
    Recover.eMethod = Engine::ECameraShotRecoverMethod::Blend;
    Recover.fBlendTime = 0.6f;
    Recover.eEase = Engine::ECameraShotEase::EaseInOutQuad;

    return tDesc;
}

Engine::SCRIPTED_CAMERA_SHOT_DESC CCameraShotPrestBuilder::Make_DebugBossPlain5SecShot()
{
    Engine::SCRIPTED_CAMERA_SHOT_DESC tDesc = Make_DebugPlain5SecShot();
    tDesc.strName = "DebugBossPlain5SecShot";

    tDesc.Start.eMode = Engine::ECameraShotStartMode::FixedFromPivot;
    tDesc.Start.vLocaloffset = Vec3(0.f, 0.8f, 4.0f);

    tDesc.Pivot.eBasisMode = Engine::ECameraBasisMode::WORLD;
    tDesc.Pivot.OrbitYawDeg = Make_Channel({
        {0.00f,  0.0f, Engine::ECameraShotEase::SmoothStep},
        {2.50f, 12.0f, Engine::ECameraShotEase::EaseInOutQuad},
        {5.00f,  0.0f, Engine::ECameraShotEase::EaseInOutQuad},
        });

    return tDesc;
}

Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC CCameraShotPrestBuilder::Make_DebugPlayerBinding()
{
    Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC tBinding = {};

    tBinding.pviot.eSource = Engine::ECameraAnchorSource::ACTOR;
    tBinding.pviot.eResolve = Engine::ECameraAnchorResolve::CAM_SOCKET;
    tBinding.pviot.iPartIndex = 0;
    tBinding.pviot.vLocalOffset = Vec3::Zero;

    tBinding.bUseSeparateLookAt = false;

    return tBinding;
}

Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC CCameraShotPrestBuilder::Make_DebugBossBinding(Engine::CGameObject* pBoss)
{
    Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC tBinding = {};

    tBinding.pviot.eSource = Engine::ECameraAnchorSource::OBJECT;
    tBinding.pviot.pObject = pBoss;
    tBinding.pviot.eResolve = Engine::ECameraAnchorResolve::BONE;
    tBinding.pviot.iPartIndex = 0;
    tBinding.pviot.strAnchorTag = "foot_r";
    tBinding.pviot.vLocalOffset = Vec3(0.f, 0.05f, 0.f);

    tBinding.bUseSeparateLookAt = false;

    return tBinding;
}

void CCameraShotPrestBuilder::Set_Start_InheritCurrent(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc)
{
    tDesc.Start.eMode = Engine::ECameraShotStartMode::InheritCurrent;
    tDesc.Start.vLocaloffset = Vec3(0.f, 0.5f, 3.0f);
    tDesc.Start.bApplyStartPoseImmediately = true;
}

void CCameraShotPrestBuilder::Set_Start_FixedFromPivot(
    Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
    const Vec3& vLocalOffset,
    _bool bApplyImmediately)
{
    tDesc.Start.eMode = Engine::ECameraShotStartMode::FixedFromPivot;
    tDesc.Start.vLocaloffset = vLocalOffset;
    tDesc.Start.bApplyStartPoseImmediately = bApplyImmediately;
}

void CCameraShotPrestBuilder::Set_Recover_PreShotSnap(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc)
{
    tDesc.Recover.eTarget = Engine::ECameraShotRecoverTarget::PreshotSnap;
    tDesc.Recover.eMethod = Engine::ECameraShotRecoverMethod::Snap;
    tDesc.Recover.fBlendTime = 0.0f;
    tDesc.Recover.eEase = Engine::ECameraShotEase::EaseInOutQuad;
}

void CCameraShotPrestBuilder::Set_Recover_PreShotBlend(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc, _float fBlendTime)
{
    tDesc.Recover.eTarget = Engine::ECameraShotRecoverTarget::PreshotSnap;
    tDesc.Recover.eMethod = Engine::ECameraShotRecoverMethod::Blend;
    tDesc.Recover.fBlendTime = fBlendTime;
    tDesc.Recover.eEase = Engine::ECameraShotEase::EaseInOutQuad;
}

void CCameraShotPrestBuilder::Set_Recover_GameplayBlend(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc, _float fBlendTime)
{
    tDesc.Recover.eTarget = Engine::ECameraShotRecoverTarget::GameplaySolved;
    tDesc.Recover.eMethod = Engine::ECameraShotRecoverMethod::Blend;
    tDesc.Recover.fBlendTime = fBlendTime;
    tDesc.Recover.eEase = Engine::ECameraShotEase::EaseInOutQuad;
}

void CCameraShotPrestBuilder::Make_Test_PlayerShot_PreShotBlend(
    OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
    OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding)
{
    tDesc = Make_DebugPlain5SecShot();
    tBinding = Make_DebugPlayerBinding();

    Set_Start_FixedFromPivot(tDesc, Vec3(0.f, 0.5f, 3.0f), true);
    Set_Recover_PreShotBlend(tDesc, 0.7f);
}

void CCameraShotPrestBuilder::Make_Test_PlayerShot_GameplayBlend(
    OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
    OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding)
{
    tDesc = Make_DebugPlain5SecShot();
    tBinding = Make_DebugPlayerBinding();

    Set_Start_FixedFromPivot(tDesc, Vec3(0.f, 0.5f, 3.0f), true);
    Set_Recover_GameplayBlend(tDesc, 0.6f);
}

void CCameraShotPrestBuilder::Make_Test_BossShot_PreShotBlend(Engine::CGameObject* pBoss, OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc, OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding)
{

}

void CCameraShotPrestBuilder::Make_Test_BossShot_GameplayBlend(
    Engine::CGameObject* pBoss,
    OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
    OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding)
{
    tDesc = Make_DebugBossPlain5SecShot();
    tBinding = Make_DebugBossBinding(pBoss);

    Set_Start_FixedFromPivot(tDesc, Vec3(0.f, 0.8f, 4.0f), true);
    Set_Recover_GameplayBlend(tDesc, 2.f);
}