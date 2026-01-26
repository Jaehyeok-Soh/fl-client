#include "pch.h"
#include "Player.h"

#include "Navigation.h"
#include "StatComponent.h"
#include "MainPlayer.h"
#include "Model.h"
#include "Weapon.h"
#include "ColliderPart.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "CameraMan_Targeter.h"
#include "Collider.h"
#include "Bone.h"
#include "Body.h"
#include "PlayerActionState.h"

#pragma region States
#include "State_Idle.h"
#include "State_Walk.h"
#include "State_Crouch.h"
#include "State_CrouchWalk.h"
#include "State_Slide.h"

#include "State_DashBack.h"
#include "State_DashFront.h"
#include "State_DashSky.h"

#include "State_RunShort.h"
#include "State_RunLoop.h"

#include "State_Jump.h"
#include "State_JumpDouble.h"
#include "State_JumpBullet.h"
#include "State_JumpBack.h"

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
        desc.vecPreAnims    = { 
                                {ENUM_TO_UINT(State::RUNLOOP), Get_AnimationIndex(L"Animation_PlayerMoon_Run_Stop_L")} 
                                ,{ENUM_TO_UINT(State::WALK), Get_AnimationIndex(L"Animation_PlayerMoon_Walk_Stop_L")} 
                                ,{ENUM_TO_UINT(State::CROUCH), Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_To_Stand")}
                                ,{ENUM_TO_UINT(State::CROUCHWALK), Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_To_Stand")}
                                ,{ENUM_TO_UINT(State::SLIDE), Get_AnimationIndex(L"Animation_PlayerMoon_Slide_End")}
                                ,{ENUM_TO_UINT(State::RUNLOOP), Get_AnimationIndex(L"Animation_PlayerMoon_Run_Stop_L_Acc")}
        };
        desc.iAnimIndex     = { Get_AnimationIndex(L"Animation_PlayerMoon_Idle") };
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.eMoveType = CStateBase_Player::MOVETYPE::CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::WALK);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::CROUCH);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::IDLE), CState_Idle::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Walk
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.vecPreAnims = {
                        {ENUM_TO_UINT(State::SLIDE), Get_AnimationIndex(L"Animation_PlayerMoon_Slide_End")}
        };
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_Walk_Loop") };
        desc.bBlend = true;
        desc.bLoop = true;

        desc.eMoveType = CStateBase_Player::MOVETYPE::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHFRONT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::SLIDE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::WALK), CState_Walk::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Crouch
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_Idle") };
        desc.bBlend = true;
        desc.bLoop = true;

        desc.eMoveType = CStateBase_Player::MOVETYPE::CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::CROUCHWALK);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CROUCH), CState_Crouch::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // CrouchWalk
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_Crouch_Loop") };
        desc.bBlend = true;
        desc.bLoop = true;

        desc.eMoveType = CStateBase_Player::MOVETYPE::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CROUCHWALK), CState_CrouchWalk::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Slide
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_Slide_Loop") };
        desc.bBlend = true;
        desc.bLoop = false;
        desc.iNextState = ENUM_TO_UINT(State::IDLE);

        desc.eMoveType = CStateBase_Player::MOVETYPE::OWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::END);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHFRONT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SLIDE), CState_Slide::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // DASHBACK
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_DodgeBack") };
        desc.bBlend = true;
        desc.bLoop = false;
        desc.iNextState = ENUM_TO_UINT(State::IDLE);

        desc.eMoveType = CStateBase_Player::MOVETYPE::CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMPBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::DASHBACK), CState_DashBack::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // DASHFRONT
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_Dodge") };
        desc.bBlend = true;
        desc.bLoop = false;
        desc.iNextState = ENUM_TO_UINT(State::IDLE);

        desc.eMoveType = CStateBase_Player::MOVETYPE::CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::RUNSHORT);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMPBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 0.3f;
        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::DASHFRONT), CState_DashFront::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // RUNSHORT
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.vecPreAnims = { {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Run_Start_L")} };
        desc.iAnimIndex = { Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop") };
        desc.bBlend = true;
        desc.bLoop = false;
        desc.iNextState = ENUM_TO_UINT(State::IDLE);

        desc.eMoveType = CStateBase_Player::MOVETYPE::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::RUNLOOP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNSHORT), CState_RunShort::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // RunLoop
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = 0;
        desc.iAnimIndex     = { Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop_Acc") };
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.eMoveType = CStateBase_Player::MOVETYPE::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHFRONT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNLOOP), CState_RunLoop::Create(pActionState, &desc))))
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
