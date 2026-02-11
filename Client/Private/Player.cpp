#include "pch.h"
#include "Player.h"

// component
#include "Navigation.h"
#include "StatComponent.h"
#include "Model.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bone.h"
#include "PlayerActionState.h"
#include "ColliderPart.h"
#include "Collider.h"
#include "ComputeShader.h"

// parts objs
#include "Weapon.h"
#include "Body.h"

#include "MainPlayer.h"
#include "CameraMan_Targeter.h"

#pragma region States
#include "State_Idle.h"
#include "State_Walk.h"
#include "State_Crouch.h"
#include "State_CrouchWalk.h"
#include "State_Slide.h"

#include "State_DashBack.h"
#include "State_DashSky.h"

#include "State_RunShort.h"
#include "State_RunLoop.h"

#include "State_Jump.h"
#include "State_JumpDouble.h"
#include "State_JumpBullet.h"
#include "State_JumpBack.h"

#include "State_Fall.h"
#include "State_Land.h"

#pragma endregion

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

CPlayer::CPlayer(const CPlayer& rhs)
    : Super(rhs)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    PLAYER_DESC* pDesc = static_cast<PLAYER_DESC*>(pArg);

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_BaseStates()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_Actor_Object(this);
    if (CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>())
        if (FAILED(pPlayerState->Awake(iCurrentLevelID)))
            return E_FAIL;

    return S_OK;
}

void CPlayer::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CPlayer::Update(const _float fTimeDelta)
{
    if (CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>())
    {
        pPlayerState->Update(fTimeDelta);
    }

    Super::Update(fTimeDelta);
}

void CPlayer::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CPlayer::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);

#ifdef _DEBUG
    CTransform* pTrans = Get_Component<CTransform>();

    Vec3 vPos = pTrans->Get_Info(TRANSFORM_INFO_STATE::POS);

    float x = vPos.x;
    float y = vPos.y;
    float z = vPos.z;

    char buf[128];
    sprintf_s(buf, "Position : %.3f, %.3f, %.3f\n", x, y, z);
    OutputDebugStringA(buf);

    wchar_t buf2[128];
    swprintf_s(buf2, L"Position : %.3f, %.3f, %.3f", x, y, z);
    //SetWindowText(g_hWnd, buf);
#endif

}

HRESULT CPlayer::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}

_int CPlayer::Get_AnimationIndex(const wstring& wstrName)
{
    if (CBody* pBody = Get_Part<CBody>(Part::BODY))
    {
        if (CModel* pModel = pBody->Get_Component<CModel>())
        {
            return pModel->Get_AnimationIndex(wstrName);
        }
        return -1;
    }
    return -1;
}

_wstring CPlayer::Get_AnimationName(_uint iAniIndex)
{
    if (CBody* pBody = Get_Part<CBody>(Part::BODY))
    {
        if (CModel* pModel = pBody->Get_Component<CModel>())
        {
            return pModel->Get_AnimationName(iAniIndex);
        }
        return L"";
    }
    return L"";
}

HRESULT CPlayer::Ready_BaseStates()
{
    CPlayerActionState* pActionState = { nullptr };
    CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
    if (!pModel)
        return E_FAIL;

    if (!(pActionState = Get_Component<CPlayerActionState>()))
        return E_FAIL;

    vector<_uint> vecChangeState_ByKey{};
    vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), ENUM_TO_UINT(State::END));

    TIME_COUNTER tKeyTimer = {};
    tKeyTimer.bCountTime = false;
    tKeyTimer.bTimeReset = false;

    // Idle
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        //desc.FAniFlags = 0;
        desc.vecPreAnims    = { 
                                {ENUM_TO_UINT(State::RUNSHORT), Get_AnimationIndex(L"Animation_PlayerMoon_Dodge")} 
                                ,{ENUM_TO_UINT(State::WALK), Get_AnimationIndex(L"Animation_PlayerMoon_Run_Stop_L")} 
                                ,{ENUM_TO_UINT(State::CROUCH), Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_To_Stand")}
                                ,{ENUM_TO_UINT(State::CROUCHWALK), Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_To_Stand")}
                                ,{ENUM_TO_UINT(State::SLIDE), Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_To_Stand")}
                                ,{ENUM_TO_UINT(State::RUNLOOP), Get_AnimationIndex(L"Animation_PlayerMoon_Run_Stop_L_Acc")}
        };
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_Idle") }; //Animation_PlayerMoon_Idle //Animation_Pino_Combo_Slash1
        desc.bBlend         = false;
        desc.bLoop          = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(CPlayer::State::GUN);

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime    = true;
        tKeyTimer.fMaxTime      = 0.05f;
        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::IDLE), CState_Idle::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Walk
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni | CStateBase::STATEANI_FLAG::SA_PreNonEvent;
        desc.FAniFlags = 0;
        desc.vecPreAnims    = {
                        {ENUM_TO_UINT(State::SLIDE), Get_AnimationIndex(L"Animation_PlayerMoon_Slide_To_Run")}
                        //,{ENUM_TO_UINT(State::LAND), Get_AnimationIndex(L"Animation_PlayerMoon_Land_To_Running")}
                        ,{-1, Get_AnimationIndex(L"Animation_PlayerMoon_Run_Start_L")}
        };
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop") }; // Animation_PlayerMoon_Run_Loop //Animation_Pino_Turn
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::WALK), CState_Walk::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Crouch
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_Idle") };
        desc.bBlend = true;
        desc.bLoop = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::CROUCHWALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CROUCH), CState_Crouch::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // CrouchWalk
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags  = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_Loop") };
        desc.bBlend     = true;
        desc.bLoop      = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CROUCHWALK), CState_CrouchWalk::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Slide & SlideSky
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        //desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.FAniFlags = 0;
        desc.vecPreAnims = {
                        //{-1, Get_AnimationIndex(L"Animation_PlayerMoon_Slide_Start")}
        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Slide_End") ,Get_AnimationIndex(L"Animation_PlayerMoon_SlideInAir_Loop")};
        desc.bBlend     = true;
        desc.bLoop      = false;


        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;
        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SLIDE), CState_Slide::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // DASHBACK
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = 0;
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_DodgeBack") };
        desc.bBlend         = true;
        desc.bLoop          = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;
        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::DASHBACK), CState_DashBack::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // DASHSKY
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Dodge_InAir"), Get_AnimationIndex(L"Animation_PlayerMoon_DodgeBack_InAir")};
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); // 원래는 fall
        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        tKeyTimer.bCountTime    = true;
        tKeyTimer.fMaxTime      = 0.4f;
        desc.tKeyTimer          = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::DASHSKY), CState_DashSky::Create(pActionState, &desc))))
            return E_FAIL;
    }

    tKeyTimer.bCountTime = false;

    // RUNSHORT
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Dodge_To_Run") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNLOOP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNSHORT), CState_RunShort::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // RunLoop
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop_Acc") };
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNLOOP), CState_RunLoop::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JUMP
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_FirstJump_InplaceStart") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); //원래는 fall
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMP), CState_Jump::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JUMPDOUBLE
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_SecondJump_Start") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); //원래는 fall
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPDOUBLE), CState_JumpDouble::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JUMPBULLET
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_BulletJump_Start") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); //원래는 fall
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPBULLET), CState_JumpBullet::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JUMPBACK
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_FirstJump_InplaceStart") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN | CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE); // fall 이 맞는데 land로 변경
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); // fall이 맞는데 우선 idle
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPBACK), CState_JumpBack::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // FALL
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = 0;
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_Jump_FallLoop") };
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN | CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); // 루프 애니메이션이 외부에서 끝났을때
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::FALL), CState_Fall::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Land
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Land_Inplace"), Get_AnimationIndex(L"Animation_PlayerMoon_LandHeavy_Inplace") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 0.3f;
        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::LAND), CState_Land::Create(pActionState, &desc))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects(PLAYER_DESC* pDesc)
{
    {
        CBody::BODY_DESC bodyDesc = {};
        bodyDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
        bodyDesc.iLevelIndex = pDesc->iLevelIndex;
        bodyDesc.wstrModelPrototypeName = pDesc->wstrBodyModelTag;
        if (FAILED(Add_Part(Part::BODY, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Body", &bodyDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_Components(PLAYER_DESC* pDesc)
{
    {
        CPlayerActionState::ACTIONSTATE_DESC desc = {};
        desc.iStateCount = ENUM_TO_UINT(State::END);
        desc.pOwnerModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
        desc.pOwnerAnimECS =static_cast<CComputeShader*>(Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Script_Component(TEXT("ComputeShader_AnimE")));
        if (FAILED(Add_Component<CPlayerActionState>(0, L"Prototype_Component_ActionState_Player", &desc)))
            return E_FAIL;
    }

    if (pDesc->wstrNavigationPrototypeTag.empty() == false)
    {
        CNavigation::NAVIGATION_DESC desc = {};
        desc.iCurrentIndex = pDesc->iNavigationCellIndex;
        desc.vPosition = pDesc->vSpawnPosition;
        if (FAILED(Add_Component<CNavigation>(0 /* static */, pDesc->wstrNavigationPrototypeTag, &desc)))
            return E_FAIL;
    }

    return S_OK;
}

void CPlayer::Free()
{
    Super::Free();
}
