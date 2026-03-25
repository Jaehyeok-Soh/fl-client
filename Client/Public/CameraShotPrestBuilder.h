#pragma once

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)

class CCameraShotPrestBuilder final
{
public:
    static SCRIPTED_CAMERA_SHOT_DESC Make_PlayerPullBackShot();
    static SCRIPTED_CAMERA_SHOT_DESC Make_BossHeadOrbitShot();

    static SCRIPTED_CAMERA_SHOT_BINDING_DESC Make_PlayerCamSocketBinding();
    static SCRIPTED_CAMERA_SHOT_BINDING_DESC Make_BossHeadBinding(CGameObject* pBoss);
};

NS_END