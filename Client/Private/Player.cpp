#include "pch.h"
#include "Player.h"

// has obj
#include "Physics_QueryFilterCallback.h"

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
#include "StatCom_Player.h"
#include "SkillComponent.h"

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

#include "State_JumpWall.h"

#pragma endregion

#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
    m_vecPartObjects.resize(Part::END, nullptr);
}

CPlayer::CPlayer(const CPlayer& rhs)
    : Super(rhs)
    , m_pPhysic_QueryFilter(rhs.m_pPhysic_QueryFilter)
{
    m_vecPartObjects.resize(Part::END, nullptr);
    Safe_AddRef(m_pPhysic_QueryFilter);
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    m_pPhysic_QueryFilter = CPhysics_QueryFilterCallback::Create();
    m_pPhysic_QueryFilter->SetOwner(this);

    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    m_pPhysic_QueryFilter->SetOwner(this);

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

    //float x = vPos.x;
    //float y = vPos.y;
    //float z = vPos.z;

    //char buf[128];
    //sprintf_s(buf, "Position : %.3f, %.3f, %.3f\n", x, y, z);
    //OutputDebugStringA(buf);

    //wchar_t buf2[128];
    //swprintf_s(buf2, L"Position : %.3f, %.3f, %.3f", x, y, z);
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

void CPlayer::Change_Weapon(_uint iPart, _uint iState)
{
    // 우선 다 none으로 바꾼다음
    static_cast<CWeapon*>(Get_Part<CWeapon>(Part::SWORD))->Set_WeaponState(CWeapon::State::NONE);
    static_cast<CWeapon*>(Get_Part<CWeapon>(Part::SKILL))->Set_WeaponState(CWeapon::State::NONE);

    switch (iPart)
    {
    case static_cast<_uint>(Part::SWORD):
        static_cast<CWeapon*>(Get_Part<CWeapon>(Part::SWORD))->Set_WeaponState(iState);
        break;

    case static_cast<_uint>(Part::SKILL):
        static_cast<CWeapon*>(Get_Part<CWeapon>(Part::SKILL))->Set_WeaponState(iState);
        break;
    }

    if(iState == ENUM_TO_UINT(CWeapon::State::NONE))
        static_cast<CWeapon*>(Get_Part<CWeapon>(Part::SWORD))->Set_WeaponState(CWeapon::State::HOLD);
}

_bool CPlayer::Check_OnGround(_float fMaxDist)
{
    return Get_Component<CTransform>()->Is_OnGround(fMaxDist, m_pPhysic_QueryFilter);
}

_bool CPlayer::Check_ColliWithMonster()
{
    // monster와 충돌 여부 반환
    return _bool();
}

void CPlayer::Count_Combo()
{
    static_cast<CStatCom_Player*>(m_pStatComp)->Add_ComboCount();
}

void CPlayer::Count_Dash()
{
    static_cast<CStatCom_Player*>(m_pStatComp)->Sub_DashCount();
}

_bool CPlayer::Start_Attack(State iState)
{
    _bool bChange = { false };
    switch (iState)
    {
    case State::COMBO:
    case State::CHARGE:
    case State::JUMPATTEND:
        bChange = static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::Melee, true);
        break;

    case State::JUMPGUN:
    case State::GUN:
        bChange = static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::Gun, true);
        break;

    case State::SKILL1:
        bChange = static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::E, true);
        if (bChange && m_pSkillEComp)
            m_pSkillEComp->Start_Skill(m_pStatComp);
        break;

    case State::SKILL2:
        bChange = static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::Q, true);
        if (bChange && m_pSkillQComp)
            m_pSkillQComp->Start_Skill(m_pStatComp);
        break;
    }

    return bChange;
}

void CPlayer::End_Attack(State iState)
{
    switch (iState)
    {
    case State::COMBO:
    case State::CHARGE:
    case State::JUMPATTEND:
        static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::Melee, false);
        break;

    case State::JUMPGUN:
    case State::GUN:
        static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::Gun, false);
        break;

    case State::SKILL1:
        static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::E, false);
        m_pSkillEComp->End_Skill(m_pStatComp);
        break;

    case State::SKILL2:
        static_cast<CStatCom_Player*>(m_pStatComp)->Set_AttackState(CStatCom_Player::Attack_State::Q, false);
        m_pSkillQComp->End_Skill(m_pStatComp);
        break;
    }
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
        desc.bLoop          = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(CPlayer::State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(CPlayer::State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(CPlayer::State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(CPlayer::State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(CPlayer::State::CHARGE);

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

        desc.FMoves     = CStateBase_Player::MOVEFLAGS::NORMAL;
        desc.FCollis    = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(CPlayer::State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(CPlayer::State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(CPlayer::State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(CPlayer::State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(CPlayer::State::CHARGE);
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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::CROUCHWALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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
                        {-1, Get_AnimationIndex(L"Animation_PlayerMoon_Slide_Start")}
        };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Slide_Loop") ,Get_AnimationIndex(L"Animation_PlayerMoon_SlideInAir_Loop")};
        desc.bBlend     = true;
        desc.bLoop      = false;


        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::END);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL); // 원래는 fall
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);

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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNLOOP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::CHARGE);
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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::CHARGE);
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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);

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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);

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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::FALL);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); 

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
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

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::CHARGE);
        desc.vecChangeState_ByKey                                                       = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime   = 0.3f;
        desc.tKeyTimer       = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::LAND), CState_Land::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JUMPWALL
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_WallJump_LU") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]            = ENUM_TO_UINT(State::SKILL1);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]            = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        //vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::JUMPGUN);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPWALL), CState_JumpWall::Create(pActionState, &desc))))
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

    Safe_Release(m_pPhysic_QueryFilter);
}
