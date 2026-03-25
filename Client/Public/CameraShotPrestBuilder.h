#pragma once

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CCameraShotPrestBuilder final
{
public:
    static Engine::SCRIPTED_CAMERA_SHOT_DESC Make_DebugPlain5SecShot();
    static Engine::SCRIPTED_CAMERA_SHOT_DESC Make_DebugBossPlain5SecShot();

    static Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC Make_DebugPlayerBinding();
    static Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC Make_DebugBossBinding(Engine::CGameObject* pBoss);

    static void Set_Start_InheritCurrent(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc);
    static void Set_Start_FixedFromPivot(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc, const Vec3& vLocalOffset, _bool bApplyImmediately = true);

    static void Set_Recover_PreShotSnap(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc);
    static void Set_Recover_PreShotBlend(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc, _float fBlendTime = 0.7f);
    static void Set_Recover_GameplayBlend(Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc, _float fBlendTime = 0.6f);

    static void Make_Test_PlayerShot_PreShotBlend(
        OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
        OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding);

    static void Make_Test_PlayerShot_GameplayBlend(
        OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
        OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding);

    static void Make_Test_BossShot_PreShotBlend(
        Engine::CGameObject* pBoss,
        OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
        OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding);

    static void Make_Test_BossShot_GameplayBlend(
        Engine::CGameObject* pBoss,
        OUT Engine::SCRIPTED_CAMERA_SHOT_DESC& tDesc,
        OUT Engine::SCRIPTED_CAMERA_SHOT_BINDING_DESC& tBinding);
};

NS_END