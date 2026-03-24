#include "pch.h"
#include "Player.h"

// has obj
#include "Physics_QueryFilterCallback.h"

// component
#include "Navigation.h"
#include "Model.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "Bone.h"
#include "PlayerActionState.h"
#include "Collider.h"
#include "ComputeShader.h"
#include "StatCom_Player.h"
#include "ActionSkill.h"
#include "EffectHandler.h"
#include "Shader.h"

#include "PhysicsCCT.h"
#include "PhysicsRigidBody.h"
#include "PhysicsCollider.h"

// parts objs
#include "Weapon.h"
#include "Body.h"
#include "Sword.h"
#include "Gun.h"
#include "PartEffect.h"
#include "SocketObject.h"
#include "TriggerCollidePart.h"
#include "BonePart.h"

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

#include "State_HitAdditive.h"
#include "State_HitFlyEnd.h"
#include "State_HitFlyStart.h"
#include "State_HitStrong.h"

#include "State_Condemn.h"
#include "State_SpecialDash.h"

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
    , m_tDoubleJumpCount(rhs.m_tDoubleJumpCount)
    , m_bMainPlayer(rhs.m_bMainPlayer)
    , m_tCBPlayerInfo{rhs.m_tCBPlayerInfo }
    , m_arrWeaponEnum(rhs.m_arrWeaponEnum)
    , m_arrMeleeInfo(rhs.m_arrMeleeInfo)
    , m_arrRangeInfo(rhs.m_arrRangeInfo)
    , m_arrSkillInfo(rhs.m_arrSkillInfo)

{
    m_vecPartObjects.resize(Part::END, nullptr);
    Safe_AddRef(m_pPhysic_QueryFilter);
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    m_tDoubleJumpCount.bCountTime = false;
    m_tDoubleJumpCount.bTimeReset = true;
    m_tDoubleJumpCount.fMaxTime = 5.f;
    m_tDoubleJumpCount.fTimeAcc = 0.f;

    m_pPhysic_QueryFilter = CPhysics_QueryFilterCallback::Create();
    m_pPhysic_QueryFilter->SetOwner(this);

    Ready_WeaponInfo();

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

    if (FAILED(Ready_PartWeapon(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_BaseStates()))
        return E_FAIL;

    if (FAILED(Ready_HitStates()))
        return E_FAIL;

    if (FAILED(Ready_PartCollider()))
        return E_FAIL;
    
    if (FAILED(Ready_Interact_PartCollider()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    CGameInstance::GetInstance()->Add_Actor_Object(this);
    CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>();
    if (pPlayerState)
    {
        if (FAILED(pPlayerState->Awake(iCurrentLevelID)))
            return E_FAIL;
    }



    // level 별 관리 : 주로 테스트용
    switch (iCurrentLevelID)
    {
    case ENUM_TO_UINT(ELevelType::TEST):
        Change_WeaponState(ENUM_TO_UINT(EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::NONE));
        //pPlayerState->Set_SpecialDashOn(true);
        break;

    case ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS):
        Set_FKeyEvent(0, true);
       // pPlayerState->Set_SpecialDashOn(true);

    default:
        Change_WeaponState(ENUM_TO_UINT(EWEAPON::MELEE), ENUM_TO_UINT(CWeapon::State::HOLD));
    }

    Start_Attack(CPlayer::State::COMBO);

    Get_Component<CActionSkill>()->Awake(iCurrentLevelID);


    return S_OK;
}

void CPlayer::Update_Priority(const _float fTimeDelta)
{
    Count_DoubleJump(fTimeDelta);

    Super::Update_Priority(fTimeDelta);

    CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>();

    // special dash on일때만 pivot 넘겨줌 : 보스전에만 가능
    if (pPlayerState->Get_SpecialDashOn())
    {
        CGameObject* pBoss = m_pGameInstance->Get_GameObject_Front(m_pGameInstance->Get_CurrentLevelIndex(), g_wszBossLayer);
        if (pBoss)
            pPlayerState->Set_PivotPos(pBoss->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS));
    }
}

void CPlayer::Update(const _float fTimeDelta)
{
    if (CPlayerActionState* pPlayerState = Get_Component<CPlayerActionState>())
    {
        pPlayerState->Update(fTimeDelta);

        // attack action desc 정리 : state update 시점에서 change를 다루어야 하기 때문에
        // update 이후 desc을 정리해준다
        pPlayerState->Reset_HitDesc();
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

    /* Main Player 일때만 Setting */
    //if (m_bMainPlayer)
    //{
    //    CTransform* pTs = Get_Component<CTransform>();
    //    if (pTs == nullptr) return;
    //    m_tCBPlayerInfo.PlayerMatrix = pTs->Get_WorldMatrix();  /* Matrix */

    //    CPhysicsCCT* pCCT = Get_Component<CPhysicsCCT>();
    //    if (pCCT == nullptr) return;
    //    const PHYSICSCCT_DESC& tDesc = pCCT->GetDesc();
    //    m_tCBPlayerInfo.fCollisionHeight    = tDesc.fHeight;
    //    m_tCBPlayerInfo.fCollisionRange     = tDesc.fRadius;

    //    CBody* pBody = Get_Part<CBody>(ENUM_TO_UINT(BODY));
    //    if (pBody == nullptr) return;
    //    CShader* pShader = pBody->Get_Component<CShader>();
    //    if (pShader == nullptr) return;
    //    pShader->Bind_PlayerInfo(m_tCBPlayerInfo);
    //}

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

HRESULT CPlayer::Change_IdleForce()
{
    CStateBase::STATE_START_DESC tDesc = {};
    tDesc.bCheckPre = false;

    if (FAILED(Get_Component<CPlayerActionState>()->Change_State(ENUM_TO_UINT(State::IDLE), false, &tDesc)))
        return E_FAIL;

    return S_OK;
}

void CPlayer::Set_WepaponOn(_uint iWeaponType, _uint iIdx, _bool bOn)
{
    switch (iWeaponType)
    {
    case ENUM_TO_UINT(EWEAPON::MELEE):
        if (iIdx >= ENUM_TO_UINT(MELEE::END))
            return;
        m_arrMeleeInfo[size_t(iIdx)].bHave = bOn;
        break;

    case ENUM_TO_UINT(EWEAPON::RANGE):
        if (iIdx >= ENUM_TO_UINT(RANGE::END))
            return;

        m_arrRangeInfo[size_t(iIdx)].bHave = bOn;
        break;

    case ENUM_TO_UINT(EWEAPON::SKILL):
        if (iIdx >= ENUM_TO_UINT(SKILL::END))
            return;

        m_arrSkillInfo[size_t(iIdx)].bHave = bOn;
        break;
    }
}

void CPlayer::SetWepaponOn_SetState(_uint iWeaponType, _uint iIdx, _bool bOn, _uint iState)
{
    switch (iWeaponType)
    {
    case ENUM_TO_UINT(EWEAPON::MELEE):
        if (iIdx >= ENUM_TO_UINT(MELEE::END))
            return;
        m_arrMeleeInfo[size_t(iIdx)].bHave = bOn;
        break;

    case ENUM_TO_UINT(EWEAPON::RANGE):
        if (iIdx >= ENUM_TO_UINT(RANGE::END))
            return;

        m_arrRangeInfo[size_t(iIdx)].bHave = bOn;
        break;

    case ENUM_TO_UINT(EWEAPON::SKILL):
        if (iIdx >= ENUM_TO_UINT(SKILL::END))
            return;

        m_arrSkillInfo[size_t(iIdx)].bHave = bOn;
        break;
    }

    Change_WeaponState(iWeaponType, iState);
}

_bool CPlayer::Change_MainWeapon(_uint iWeaponType, _uint iIdx)
{
    _bool bOn = { false };
    switch (iWeaponType)
    {
    case ENUM_TO_UINT(EWEAPON::MELEE):
        if (iIdx >= ENUM_TO_UINT(MELEE::END))
            return false;
        bOn = m_arrMeleeInfo[size_t(iIdx)].bHave;
        break;

    case ENUM_TO_UINT(EWEAPON::RANGE):
        if (iIdx >= ENUM_TO_UINT(RANGE::END))
            return false;

        bOn = m_arrRangeInfo[size_t(iIdx)].bHave;
        break;

    case ENUM_TO_UINT(EWEAPON::SKILL):
        if (iIdx >= ENUM_TO_UINT(SKILL::END))
            return false;

        bOn = m_arrSkillInfo[size_t(iIdx)].bHave;
        break;
    }

    if (bOn)
    {
        // 기존에 있던거 꺼주기
        Set_CurPartWeapon_State(static_cast<EWEAPON>(iWeaponType), ENUM_TO_UINT(CWeapon::State::NONE));

        // 인덱스 change
        m_arrWeaponEnum[size_t(iWeaponType)] = iIdx;
    }

    return bOn;
}

_bool CPlayer::Change_MainWeaponNext(_uint iWeaponType)
{
    _uint iNextIdx = Get_CurWeaponIdx(iWeaponType) + 1;
    
    // 만약 다음 인덱스가 마지막이 되었을때는 0으로 리셋
    switch (iWeaponType)
    {
    case ENUM_TO_UINT(EWEAPON::MELEE):
        if (iNextIdx == ENUM_TO_UINT(MELEE::END))
            iNextIdx = 0;
        break;

    case ENUM_TO_UINT(EWEAPON::RANGE):
        if (iNextIdx == ENUM_TO_UINT(RANGE::END))
            iNextIdx = 0;
        break;

    case ENUM_TO_UINT(EWEAPON::SKILL):
        if (iNextIdx == ENUM_TO_UINT(SKILL::END))
            iNextIdx = 0;
        break;
    }

    return Change_MainWeapon(iWeaponType, iNextIdx);
}

void CPlayer::Change_WeaponState(_uint iWeaponType, _uint iState)
{
    // 우선 다 none으로 바꾼다음
    Set_CurPartWeapon_State(EWEAPON::MELEE, ENUM_TO_UINT(CWeapon::State::NONE));
    Set_CurPartWeapon_State(EWEAPON::RANGE, ENUM_TO_UINT(CWeapon::State::NONE));
    Set_CurPartWeapon_State(EWEAPON::SKILL, ENUM_TO_UINT(CWeapon::State::NONE));

    switch (iWeaponType)
    {
    case ENUM_TO_UINT(EWEAPON::MELEE):
        Set_CurPartWeapon_State(EWEAPON::MELEE, iState);
        break;

    case ENUM_TO_UINT(EWEAPON::RANGE):
        Set_CurPartWeapon_State(EWEAPON::RANGE, iState);
        break;

    case ENUM_TO_UINT(EWEAPON::SKILL):
        Set_CurPartWeapon_State(EWEAPON::SKILL, iState);
        break;
    }

    if (iState == ENUM_TO_UINT(CWeapon::State::NONE) && iWeaponType != ENUM_TO_UINT(EWEAPON::MELEE))
    {
        Set_CurPartWeapon_State(EWEAPON::MELEE, ENUM_TO_UINT(CWeapon::State::HOLD));
    }
}

_int CPlayer::Get_CurWeaponIdx(_uint iWeaponType)
{
    return m_arrWeaponEnum[size_t(iWeaponType)];
}

_bool CPlayer::Can_UseWeapon(_uint iWeaponType)
{
    _uint iCurWeapon{};
    switch (iWeaponType)
    {
    case ENUM_TO_UINT(EWEAPON::MELEE):
        // melee중에 현재 weapon
        iCurWeapon = m_arrWeaponEnum[ENUM_TO_SZET(EWEAPON::MELEE)];
        return      m_arrMeleeInfo[size_t(iCurWeapon)].bHave;

    case ENUM_TO_UINT(EWEAPON::RANGE):
        // melee중에 현재 weapon
        iCurWeapon = m_arrWeaponEnum[ENUM_TO_SZET(EWEAPON::RANGE)];
        return          m_arrRangeInfo[size_t(iCurWeapon)].bHave;

    case ENUM_TO_UINT(EWEAPON::SKILL):
        // melee중에 현재 weapon
        iCurWeapon = m_arrWeaponEnum[ENUM_TO_SZET(EWEAPON::SKILL)];
        return       m_arrSkillInfo[size_t(iCurWeapon)].bHave;
    }

    return false;
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
    static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Add_ComboCount();
}

void CPlayer::Count_Dash()
{
    static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Sub_DashCount();
}

void CPlayer::Set_RootMotion_Apply(_bool bApply)
{
    Get_Part<CBody>(Part::BODY)->Get_Component<CModel>()->Set_CurAnimation_RootApply(bApply);
}

_bool CPlayer::Check_DoubleJump()
{
    return !(m_tDoubleJumpCount.bCountTime);
}

void CPlayer::Change_CamState(_uint iCamState)
{
    if (m_pTargeter)
    {
        static_cast<CCameraMan_Targeter*>(m_pTargeter)->Change_CamState(iCamState);
    }
}

_float CPlayer::Get_CamPitch() const
{
    if (m_pTargeter)
    {
        return static_cast<CCameraMan_Targeter*>(m_pTargeter)->Get_Pitch();
    }

    return -1000.f;
}

void CPlayer::Set_FKeyEvent(_uint iEvenet, _bool bOn)
{
    CPlayerActionState* pAction = static_cast<CPlayerActionState*>(Get_Component<CActionState>());

    if (pAction)
    {
        pAction->Set_FKeyEvent(iEvenet, bOn);
    }
}

const Vec3& CPlayer::Get_CollidedMonster_Position()
{
    if (CTriggerCollidePart* pCollider = Get_Part<CTriggerCollidePart>(Part::DETECTCOLLIDER))
    {
        return pCollider->Get_Collided_ObjPos();
    }

    return Vec3::Zero;
}

HRESULT CPlayer::Bind_PlayerInfo(class CShader* pShader)
{
    if (!pShader) return E_FAIL;

    CTransform* pTs = Get_Component<CTransform>();
    if (pTs == nullptr) return E_FAIL;
    m_tCBPlayerInfo.PlayerMatrix = pTs->Get_WorldMatrix();  /* Matrix */

    CPhysicsCCT* pCCT = Get_Component<CPhysicsCCT>();
    if (pCCT == nullptr) return E_FAIL;
    const PHYSICSCCT_DESC& tDesc = pCCT->Get_Desc();
    CPhysicsCCT::CCTMOVEMENTSTATE* tCCTMovementState = pCCT->GetMoveState();
    m_tCBPlayerInfo.fCurSpeed = tCCTMovementState->vVelocity.magnitude();
    m_tCBPlayerInfo.fCollisionHeight    = tDesc.fHeight;
    m_tCBPlayerInfo.fCollisionRange     = tDesc.fRadius;


    CBody* pBody = Get_Part<CBody>(ENUM_TO_UINT(BODY));
    if (pBody == nullptr) return E_FAIL;

    CShader* pPlayerShader = pShader == nullptr ? pBody->Get_Component<CShader>() : pShader;
    if (pPlayerShader == nullptr) return E_FAIL;;

    if (FAILED(pPlayerShader->Bind_PlayerInfo(m_tCBPlayerInfo)))
        return E_FAIL;

    return S_OK;
}

_bool CPlayer::Start_Attack(State iState)
{
    _bool bChange = { false };
    switch (iState)
    {
    case State::COMBO:
    case State::COMBO_DUAL:
    case State::CHARGE:
    case State::JUMPATTEND:
        bChange = static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Set_AttackState(CStatCom_Player::Attack_State::Melee, true);
        break;

    case State::GUNIDLE:
    case State::GUNWALK:
    case State::GUNATTACK:
    case State::GUNRELOAD:
        bChange = static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Set_AttackState(CStatCom_Player::Attack_State::Gun, true);
        break;

    case State::SKILL1:
        bChange = Get_Component<CActionSkill>()->Start_Skill(MoonE);
        break;

    case State::SKILL2:
        bChange = Get_Component<CActionSkill>()->Start_Skill(MoonQ);

        if (bChange)
        {
            if (CPartEffect* pEff = Get_Part<CPartEffect>(Part::EFFECT))
            {
                pEff->Change_State(CPartEffect::CPartEff_State::SPAWN);
            }
        }

        break;
    }

    return bChange;
}

void CPlayer::End_Attack(State iState)
{
    switch (iState)
    {
        // combo timer 시작
    case State::COMBO:
    case State::COMBO_DUAL:
    case State::JUMPATTEND:
        static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Set_Timer(CStatCom_Player::TIMER_TYPE::COMBO, true);
    case State::CHARGE:
        static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Set_AttackState(CStatCom_Player::Attack_State::Melee, false);
        break;

    case State::GUNIDLE:
    case State::GUNWALK:
    case State::GUNATTACK:
    case State::GUNRELOAD:
        static_cast<CStatCom_Player*>(Get_Component<CMyStat>())->Set_AttackState(CStatCom_Player::Attack_State::Gun, false);
        break;

    case State::SKILL1:
        Get_Component<CActionSkill>()->End_Skill(MoonE);
        break;

    case State::SKILL2:
        Get_Component<CActionSkill>()->End_Skill(MoonE);
        break;
    }
}

CPlayer::State CPlayer::Get_CurState()
{
    CActionState* pAction = Get_Component<CActionState>();

    return static_cast<CPlayer::State>(pAction->Get_CurrentStateIndex());
}

ICameraAnchorProvider* CPlayer::Get_CameraAnchorProvider(_int iPartIndex)
{
    if (iPartIndex < 0 || iPartIndex >= Part::END)
        return nullptr;

    switch (iPartIndex)
    {
    case Part::BODY:
        return Get_Part<CBody>(Part::BODY);
    //case Part::SWORD:
    //    return Get_Part<CSword>(Part::SWORD);
    //case Part::GUN:
    //    return Get_Part<CGun>(Part::GUN);
    //case Part::Dual_R:
    //    return Get_Part<CSword>(Part::Dual_R);
    //case Part::Dual_L:
    //    return Get_Part<CSword>(Part::Dual_L);
    default:
        nullptr;
    }
}

CTransform* CPlayer::Get_CameraAnchorOwnerTransform()
{
    return Get_Component<CTransform>();
}

HRESULT CPlayer::Ready_BaseStates()
{
    CPlayerActionState* pActionState = { nullptr };
    CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
    if (!pModel)
        return E_FAIL;

    if (!(pActionState = Get_Component<CPlayerActionState>()))
        return E_FAIL;

    CGun* pMyGun = static_cast<CGun*>(Get_Part<CWeapon>(ENUM_TO_UINT(Part::GUN)));

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
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_Idle") }; //Animation_PlayerMoon_Idle // Animation_PlayerMoon_Turn_L45
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        desc.FCollis =    CStateBase_Player::COLLISIONFLAGS::C_DOWN
                        | CStateBase_Player::COLLISIONFLAGS::C_Addtive 
                        | CStateBase_Player::COLLISIONFLAGS::C_Strong
                        | CStateBase_Player::COLLISIONFLAGS::C_Fly
                        | CStateBase_Player::COLLISIONFLAGS::C_CheckF
                        ;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(CPlayer::State::CHARGE);

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime    = true;
        tKeyTimer.fMaxTime      = 0.07f;
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::IDLE), CState_Idle::Create(pActionState, &desc))))
            return E_FAIL;
    }

    vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(CPlayer::State::END);

    // Walk
    {
        CStateBase_Player::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;// | CStateBase::STATEANI_FLAG::SA_PreNonEvent;
        //desc.FAniFlags = 0;
        desc.vecPreAnims    = {
                        {ENUM_TO_UINT(State::SLIDE), Get_AnimationIndex(L"Animation_PlayerMoon_Slide_To_Run")}
                        //,{ENUM_TO_UINT(State::LAND), Get_AnimationIndex(L"Animation_PlayerMoon_Land_To_Running")}
                        //,{-1, Get_AnimationIndex(L"Animation_PlayerMoon_Run_Start_L")}
        };
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop") }; // Animation_PlayerMoon_Run_Loop
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.FMoves     = CStateBase_Player::MOVEFLAGS::NORMAL;
        desc.FCollis    = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF
            ;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(CPlayer::State::CHARGE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime    = false;
        desc.tKeyTimer          = tKeyTimer;
        desc.pOwnerGun          = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
                     | CStateBase_Player::COLLISIONFLAGS::C_Strong
                     | CStateBase_Player::COLLISIONFLAGS::C_Fly
                     | CStateBase_Player::COLLISIONFLAGS::C_CheckF
                     ;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::CROUCHWALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = false;
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly
            | CStateBase_Player::COLLISIONFLAGS::C_CheckF
            ;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = false;
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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


        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN | CStateBase_Player::MOVEFLAGS::LOOP_DONE;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBULLET);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::RUNSHORT);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 0.32f;
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = (3.f / 5.f / 1.2f);
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);

        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        tKeyTimer.bCountTime    = false;
        tKeyTimer.fMaxTime      = 15.f / (24.f * 1.2f);//0.4f;

        desc.tKeyTimer          = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FMoves = CStateBase_Player::MOVEFLAGS::UP_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONEMOVEKEY)] = ENUM_TO_UINT(State::WALK);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 15.f/( 24.f * 1.2f);// (0.7f);
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::RUNSHORT), CState_RunShort::Create(pActionState, &desc))))
            return E_FAIL;

        tKeyTimer.bCountTime = false;
    }

    // RunLoop
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Run_Loop_Acc") };
        desc.bBlend         = true;
        desc.bLoop          = true;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONEMOVEKEY)] = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FCollis = 0;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);
        
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);

        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

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

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN | CStateBase_Player::MOVEFLAGS::PRESS_CHANGE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::FALL);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPBACK), CState_JumpBack::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // FALL
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags      = 0;
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_Jump_FallLoop") };
        desc.bBlend         = false;
        desc.bLoop          = true;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::OWN | CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMPDOUBLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::SLIDE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::LAND); 

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::FALL), CState_Fall::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // Land
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Land_Inplace"), Get_AnimationIndex(L"Animation_PlayerMoon_LandHeavy_Inplace") };
        desc.bBlend = false;
        desc.bLoop = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
            | CStateBase_Player::COLLISIONFLAGS::C_Strong
            | CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::IDLE);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]           = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::CHARGE);
        desc.vecChangeState_ByKey                                                       = vecChangeState_ByKey;

        tKeyTimer.bCountTime    = true;
        tKeyTimer.fMaxTime      = 0.1f; // (24.f / 36.f) / (36.f / 24.f) / 1.2f;
        desc.tKeyTimer          = tKeyTimer;
        desc.pOwnerGun          = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::LAND), CState_Land::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // JUMPWALL
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_WallJump_LU") };
        desc.bBlend = true;
        desc.bLoop  = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_Fly;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::NORMAL;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]           = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]          = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]          = ENUM_TO_UINT(State::DASHSKY);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)]    = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]       = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)]       = ENUM_TO_UINT(State::FALL);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)]              = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)]              = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)]             = ENUM_TO_UINT(State::JUMPATTSTART);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)]             = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)]         = ENUM_TO_UINT(State::END);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 0.1f;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_NextFrame;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::JUMPWALL), CState_JumpWall::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // SPECIALDASH
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims   = { Get_AnimationIndex(L"Animation_PlayerMoon_DodgeBack") };
        desc.bBlend         = false;
        desc.bLoop          = false;

        desc.FCollis = 0;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)] = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)] = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)] = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)] = ENUM_TO_UINT(State::END);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)] = ENUM_TO_UINT(State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)] = ENUM_TO_UINT(State::CHARGE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime    = true;
        tKeyTimer.fMaxTime      = 20.f / ANIMTIC;

        desc.tKeyTimer          = tKeyTimer;
        desc.pOwnerGun          = pMyGun;

        desc.FWeaponChanges = CStateBase_Player::WEAPONCHANGEFLAGS::Change_Check | CStateBase_Player::WEAPONCHANGEFLAGS::Change_End;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::SPECIALDASH), CState_SpecialDash::Create(pActionState, &desc))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_HitStates()
{
    CPlayerActionState* pActionState = { nullptr };
    CModel* pModel = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
    if (!pModel)
        return E_FAIL;

    if (!(pActionState = Get_Component<CPlayerActionState>()))
        return E_FAIL;

    CGun* pMyGun = static_cast<CGun*>(Get_Part<CWeapon>(ENUM_TO_UINT(Part::GUN)));

    vector<_uint> vecChangeState_ByKey{};
    vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), ENUM_TO_UINT(State::END));

    TIME_COUNTER tKeyTimer = {};
    tKeyTimer.bCountTime = false;
    tKeyTimer.bTimeReset = false;

    // HITFLYSTART
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = CStateBase::STATEANI_FLAG::SA_HasPreAni;
        desc.vecPreAnims = { {-1, Get_AnimationIndex(L"Animation_PlayerMoon_HitFly_Start")} };
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_HitFly_Loop") };
        desc.bBlend = true;
        desc.bLoop = true;

        desc.FMoves = 0;
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::HITFLYSTART), CState_HitFlyStart::Create(pActionState, &desc))))
            return E_FAIL;
    }



    // HITADDTIVE
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_HitAdditive") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::MOVE)]       = ENUM_TO_UINT(State::WALK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SPACE)]      = ENUM_TO_UINT(State::JUMP);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::SHIFT)]      = ENUM_TO_UINT(State::DASHBACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_PRESS)] = ENUM_TO_UINT(State::CROUCH);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LCRTL_UP)]   = ENUM_TO_UINT(State::END);

        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::E)] = ENUM_TO_UINT(State::SKILL1);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::Q)] = ENUM_TO_UINT(State::SKILL2);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LM)] = ENUM_TO_UINT(CPlayer::State::COMBO);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::RM)] = ENUM_TO_UINT(CPlayer::State::GUNATTACK);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::CHARGE)] = ENUM_TO_UINT(CPlayer::State::CHARGE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;


        desc.tKeyTimer = tKeyTimer;

        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::HITADDTIVE), CState_HitAdditive::Create(pActionState, &desc))))
            return E_FAIL;
    }

    //CONDEMN
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_Condemn_End_01") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis = 0;
        desc.FMoves = CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONEMOVEKEY)] = ENUM_TO_UINT(State::WALK);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 4.f;
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::CONDEMN), CState_Condemn::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // HITSTRONG
    {
        CState_HitStrong::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        vector<_int> vecHitStrong;
        vecHitStrong.resize(ENUM_TO_UINT(CState_HitStrong::HitStrong_AnimIdx::END));

        desc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        vecHitStrong[ENUM_TO_SZET(CState_HitStrong::HitStrong_AnimIdx::FRONT)] = Get_AnimationIndex(L"Animation_PlayerMoon_HitLight_01");
        vecHitStrong[ENUM_TO_SZET(CState_HitStrong::HitStrong_AnimIdx::BACK)] = Get_AnimationIndex(L"Animation_PlayerMoon_HitLight_B");
        vecHitStrong[ENUM_TO_SZET(CState_HitStrong::HitStrong_AnimIdx::RIGHT)] = Get_AnimationIndex(L"Animation_PlayerMoon_HitLight_R");
        vecHitStrong[ENUM_TO_SZET(CState_HitStrong::HitStrong_AnimIdx::LEFT)] = Get_AnimationIndex(L"Animation_PlayerMoon_HitLight_L");

        desc.vecMainAnims = vecHitStrong;
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FMoves = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        // key additive와 동일
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONEMOVEKEY)] = ENUM_TO_UINT(State::WALK);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 25.f / (24.f * 1.2f);
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::HITSTRONG), CState_HitStrong::Create(pActionState, &desc))))
            return E_FAIL;
    }

    // HITFLYEND
    {
        CState_RunLoop::PLAYER_STATEBASE_DESC  desc = {};
        desc.FAniFlags = 0;
        desc.vecMainAnims = { Get_AnimationIndex(L"Animation_PlayerMoon_HitFly_End") };
        desc.bBlend = true;
        desc.bLoop = false;

        desc.FCollis    = CStateBase_Player::COLLISIONFLAGS::C_DOWN;
        desc.FMoves     = CStateBase_Player::MOVEFLAGS::PRESS_CHANGE | CStateBase_Player::MOVEFLAGS::LOOP_DONE;
        // key additive와 동일
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONE)] = ENUM_TO_UINT(State::IDLE);
        vecChangeState_ByKey[ENUM_TO_SZET(CStateBase_Player::STATEKEY::LOOPDONEMOVEKEY)] = ENUM_TO_UINT(State::WALK);
        desc.vecChangeState_ByKey = vecChangeState_ByKey;

        tKeyTimer.bCountTime = true;
        tKeyTimer.fMaxTime = 30.f / (24.f * 1.2f);//2.f;
        desc.tKeyTimer = tKeyTimer;
        desc.pOwnerGun = pMyGun;

        if (FAILED(pActionState->Add_State(ENUM_TO_UINT(State::HITFLYEND), CState_HitFlyEnd::Create(pActionState, &desc))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_WeaponInfo()
{
    // weapon enum
    {
        m_arrWeaponEnum.fill(0); //원래는 -1로 해야하는게 맞는데 0으로 해서 다 쓸 수 있도록
    }

    /* Melee */
    {
        {
            WEAPON_INFO tInfo = {};

            tInfo.iPartStartIdx = Part::SWORD;
            tInfo.iPartSize = 1;
            tInfo.bHave = false;
            tInfo.iWeaponState = ENUM_TO_UINT(CWeapon::State::HOLD);

            m_arrMeleeInfo[ENUM_TO_SZET(MELEE::SWORD)] = tInfo;
        }

        {
            WEAPON_INFO tInfo = {};

            tInfo.iPartStartIdx = Part::Dual_R;
            tInfo.iPartSize = 2;
            tInfo.bHave = false;
            tInfo.iWeaponState = ENUM_TO_UINT(CWeapon::State::NONE);

            m_arrMeleeInfo[ENUM_TO_SZET(MELEE::DUAL)] = tInfo;
        }

    }

    /* Range */
    {
        // MACHINE
        {
            WEAPON_INFO tInfo = {};

            tInfo.iPartStartIdx = Part::GUN;
            tInfo.iPartSize = 1;
            tInfo.bHave = false;
            tInfo.iWeaponState = ENUM_TO_UINT(CWeapon::State::NONE);

            m_arrRangeInfo[ENUM_TO_SZET(RANGE::MACHINE)] = tInfo;
        }
    }

    /* Skill */
    {
        {
            WEAPON_INFO tInfo = {};

            tInfo.iPartStartIdx = Part::SKILL;
            tInfo.iPartSize = 1;
            tInfo.bHave = false;
            tInfo.iWeaponState = ENUM_TO_UINT(CWeapon::State::NONE);

            m_arrSkillInfo[ENUM_TO_SZET(SKILL::MOON)] = tInfo;
        }
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

    // part eff
    {
        {
            CPartEffect::PART_EFFECT_DESC tDesc;
            tDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
            tDesc.arrState_DurationTimes = { 0.f,12.5f,0.f };
            tDesc.arrState_DelayTimes = { 0.f,0.5f,0.f };
            tDesc.FPartEff_Flags = CPartEffect::PartEff_Flag::Spawn_Again_AfterDespawn;
            tDesc.iLevelIndex = pDesc->iLevelIndex;

            vector<CPartEffect::DATA_EFFHANDLER> tEffectHandlerDesc;
            tEffectHandlerDesc.reserve(2);

            CPartEffect::DATA_EFFHANDLER tData0 = {};
            {
                tData0.eEffState = CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_SPAWN;
                tData0.eHandlerType = CEffectHandler::E_HANDLER_TYPE::SKILL_OBJ;

                CEffectHandler::STATE_VFX_DESC SkillDesc{};
                {
                    SkillDesc.EffectPrefabTag = "Player_Moon_QSkill_Barrior";
                    SkillDesc.pParentTransformMatrix = nullptr;
                    SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_LOCAL };
                    SkillDesc.bFollowBone = { false };
                    SkillDesc.iBoneIndex = -1;
                    SkillDesc.vOffSet = { 0.f,1.f,0.f };
                    SkillDesc.vRotation = { Vec3::Zero };
                }
                tData0.tSkillDesc = SkillDesc;
            }
            tEffectHandlerDesc.push_back(tData0);

            CPartEffect::DATA_EFFHANDLER tData1 = {};
            {
                tData1.eEffState = CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_DESTROY;
                tData1.eHandlerType = CEffectHandler::E_HANDLER_TYPE::SKILL_OBJ;

                CEffectHandler::STATE_VFX_DESC SkillDesc{};
                {
                    SkillDesc.EffectPrefabTag = "";
                    SkillDesc.pParentTransformMatrix = nullptr;
                    SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_LOCAL };
                    SkillDesc.bFollowBone = { false };
                    SkillDesc.iBoneIndex = -1;
                    SkillDesc.vOffSet = { Vec3::Zero };
                    SkillDesc.vRotation = { Vec3::Zero };
                }
                tData1.tSkillDesc = SkillDesc;
            }
            tEffectHandlerDesc.push_back(tData1);

            tDesc.tEffectHandlerDesc = tEffectHandlerDesc;

            if (FAILED(Add_Part(Part::EFFECT, ENUM_TO_UINT(ELevelType::STATIC), g_wszPartObj_Effect_Prototype_Tag, &tDesc)))
                return E_FAIL;
        }
    }

    // CLOAK
    {
        CBody* pBody = Get_Part<CBody>(ENUM_TO_UINT(Part::BODY));
        CBonePart::BONEPART_DESC desc = {};
        desc.wstrModelPrototypeName = L"Prototype_Component_Model_MoonClock";
        desc.pMatParent             = &Get_Component<CTransform>()->Get_WorldMatrix();
        desc.pParentBoneCombineCS   = pBody->Get_BoneCombineCS();
        desc.pParentModel           = pBody->Get_Component<CModel>();
        desc.FFlags                 = ENUM_TO_UINT(CBonePart::BonePartFlag::VSShakeOn);

        if (FAILED(Add_Part(Part::CLOAK, ENUM_TO_UINT(ELevelType::STATIC), g_wszPartObj_Bone_Prototype_Tag, &desc)))
            return E_FAIL;
    }


    return S_OK;
}

HRESULT CPlayer::Ready_PartWeapon(PLAYER_DESC* pDesc)
{
    // Weapons : Sword
    {
        CWeapon::WEAPON_DESC weaponDesc = {};
        weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_MoonSword";
        weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
        weaponDesc.pMatHandSocket = &Get_Part<CBody>(Part::BODY)->Get_RightHandSocket()->Get_CombinedTransformMatrix();
        weaponDesc.pMatSocket = &Get_Part<CBody>(Part::BODY)->Get_WeaponSocket()->Get_BindPoseTransformMatrix();
        weaponDesc.eModel = CWeapon::Weapon_ModelType::STATIC;
        weaponDesc.eState = CWeapon::State::HOLD;

        weaponDesc.bMianWeapon = false;
        weaponDesc.FDescFlag = CWeapon::WeaponDescFlag::WF_RGBMappingOn;
        weaponDesc.vColorR = Vec4(0.119538f, 0.119538f, 0.119538f, 1.f);
        weaponDesc.vColorG = Vec4(1.f, 0.751839f, 0.182292f, 1.f);
        weaponDesc.vColorB = Vec4(0.458824f, 0.435294f, 0.45098f, 1.f);


        weaponDesc.matHandOffsetMatrix = Matrix::CreateRotationX(XMConvertToRadians(-90.f));
        //weaponDesc.matHoldOffsetMatrix  = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(10.f), XMConvertToRadians(0.f), XMConvertToRadians(-45.f));
        weaponDesc.matConOffsetMatrix = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(-70.f), XMConvertToRadians(180.f), 0.f);

        if (FAILED(Add_Part(Part::SWORD, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Sword", &weaponDesc)))
            return E_FAIL;
    }

    // Weapons : Skill
    {
        CWeapon::WEAPON_DESC weaponDesc = {};
        weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_MoonSkillWeap";
        weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
        weaponDesc.pMatHandSocket = &Get_Part<CBody>(Part::BODY)->Get_RightHandSocket()->Get_CombinedTransformMatrix();
        weaponDesc.pMatSocket = &Get_Part<CBody>(Part::BODY)->Get_WeaponSocket()->Get_BindPoseTransformMatrix();
        weaponDesc.eModel = CWeapon::Weapon_ModelType::STATIC;
        weaponDesc.bMianWeapon = false;
        weaponDesc.FDescFlag = CWeapon::WeaponDescFlag::WF_RGBMappingOn;
        weaponDesc.vColorR = Vec4(0.84375f, 0.84375f, 0.84375f, 1.f);
        weaponDesc.vColorB = Vec4(0.234375f, 0.234375f, 0.234375f, 1.f);
        weaponDesc.vColorG = Vec4(0.686686f, 0.686686f, 0.686686f, 1.f);


        weaponDesc.matHandOffsetMatrix = Matrix::CreateRotationX(XMConvertToRadians(-90.f));
        weaponDesc.matHoldOffsetMatrix = Matrix::CreateRotationX(XMConvertToRadians(-90.f));
        if (FAILED(Add_Part(Part::SKILL, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Sword", &weaponDesc)))
            return E_FAIL;
    }

    //Weapons : Gun
    {
        CGun::GUN_DESC weaponDesc = {};
        weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_MoonGun"; //Prototype_Component_Model_XibiWeapon //Prototype_Component_Model_MoonGun
        weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
        weaponDesc.pMatHandSocket = &Get_Part<CBody>(Part::BODY)->Get_RightHandSocket()->Get_CombinedTransformMatrix();
        weaponDesc.pMatSocket = &Get_Part<CBody>(Part::BODY)->Get_WeaponSocket()->Get_BindPoseTransformMatrix();
        weaponDesc.eModel = CWeapon::Weapon_ModelType::ANIM;
        weaponDesc.eAnimState = CWeapon::AnimState::PLAY_ONCE;
        weaponDesc.bMianWeapon = false;
        weaponDesc.eState = CWeapon::State::HOLD;
        weaponDesc.FDescFlag = CWeapon::WeaponDescFlag::WF_RGBMappingOn;
        weaponDesc.vColorR = Vec4(0.947917f, 0.947917f, 0.947917f, 1.f);
        weaponDesc.vColorG = Vec4(0.364583f, 0.355613f, 0.351292f, 1.f);
        weaponDesc.vColorB = Vec4(0.03954f, 0.035601f, 0.03434f, 1.f);

        weaponDesc.fAllBullet = 1000.f;
        weaponDesc.fCurBullet = 500.f;
        weaponDesc.fAttackCoolTime = 0.2f; // 0.15 넘 빠름 // 0.3 너무 느림

        weaponDesc.matHandOffsetMatrix = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(180.f), XMConvertToRadians(90.f), XMConvertToRadians(0.f));
        weaponDesc.matHoldOffsetMatrix = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(0.f), XMConvertToRadians(0.f), XMConvertToRadians(90.f));

        if (FAILED(Add_Part(Part::GUN, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Gun", &weaponDesc)))
            return E_FAIL;
    }

    // Weapons : Dual
    {
        CWeapon::WEAPON_DESC weaponDesc = {};
        weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_DualR";
        weaponDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
        weaponDesc.pMatHandSocket = &Get_Part<CBody>(Part::BODY)->Get_RightHandSocket()->Get_CombinedTransformMatrix();
        weaponDesc.pMatSocket = &Get_Part<CBody>(Part::BODY)->Get_WeaponSocket()->Get_BindPoseTransformMatrix();
        weaponDesc.eModel = CWeapon::Weapon_ModelType::STATIC;
        weaponDesc.eState = CWeapon::State::HOLD;

        weaponDesc.bMianWeapon = false;
        weaponDesc.FDescFlag = CWeapon::WeaponDescFlag::WF_RGBMappingOn;
        weaponDesc.vColorR = Vec4(0.309524f, 0.309524f, 0.309524f, 1.f);
        weaponDesc.vColorG = Vec4(0.10119f, 0.10119f, 0.10119f, 1.f);
        weaponDesc.vColorB = Vec4(0.125f, 0.055804f, 0.055804f, 1.f);

        weaponDesc.matHoldOffsetMatrix  = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(0.f), XMConvertToRadians(45.f), XMConvertToRadians(-10.f));

        if (FAILED(Add_Part(Part::Dual_R, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Sword", &weaponDesc)))
            return E_FAIL;

        weaponDesc.wstrModelPrototypeName = L"Prototype_Component_Model_DualL";
        weaponDesc.pMatHandSocket = Get_Part<CBody>(Part::BODY)->Get_SocketMatrix(415);
        weaponDesc.matHoldOffsetMatrix = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(10.f), XMConvertToRadians(0.f), XMConvertToRadians(-90.f));

        if (FAILED(Add_Part(Part::Dual_L, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Sword", &weaponDesc)))
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

HRESULT CPlayer::Ready_PartCollider()
{
    CTriggerCollidePart::TRIGGER_COLLIDEPART_DESC tPartColliDesc;
    {
        PHYSICSRIGIDBODY_DESC tRigiDesc = {};
        {
            tRigiDesc.eType = EPhysicsActorType::KINEMATIC;
            tRigiDesc.bUseGravity = false;
            tRigiDesc.pOwnerMatrix = nullptr;

            tPartColliDesc.pRigidbodyDesc = &tRigiDesc;
        }

        PHYSICSCOLLIDER_DESC tPColliDesc = {};
        {
            tPColliDesc.eShape  = EPhysicsShape::BOX;
            //tPColliDesc.fHeight = 100.f;
            tPColliDesc.vCenter = { 0.f,0.f,0.f };
            tPColliDesc.vExtents = {40.f, 100.f, 40.f};

            //tPColliDesc.fRadius = { 20.f };
            tPColliDesc.bIsTrigger = { true };
            tPColliDesc.eFilterLayer = PHYSICSFILTERGROUP::DETECT_MONSTER;
            tPColliDesc.iFilterMask =
            {
                PHYSICSFILTERGROUP::Enum::MONSTER
                | PHYSICSFILTERGROUP::Enum::OBJECT1
                | PHYSICSFILTERGROUP::Enum::OBJECT2
            };

           tPartColliDesc.pColliderDesc = &tPColliDesc;
        }

        tPartColliDesc.FUpdate_Flags = ENUM_TO_UINT(CTriggerCollidePart::UPDATEFLAGS::Only_TriggerCall);
        tPartColliDesc.pMatParent = Get_Component<CTransform>()->Get_WorldMatrixPtr();

        // mini map에게 감지할 part ui
        if (FAILED(Add_Part(Part::DETECTCOLLIDER_UI, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Collider", &tPartColliDesc)))
            return E_FAIL;

        {
            tPColliDesc.eShape = EPhysicsShape::SPHERE;
            tPColliDesc.fRadius = { 4.5f };
            tPartColliDesc.pColliderDesc = &tPColliDesc;
        }

        tPartColliDesc.FUpdate_Flags = ENUM_TO_UINT(CTriggerCollidePart::UPDATEFLAGS::Only_ObjChache) | ENUM_TO_UINT(CTriggerCollidePart::UPDATEFLAGS::Update_MinDistance);

        // player가 감지할 part ui
        if (FAILED(Add_Part(Part::DETECTCOLLIDER, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Collider", &tPartColliDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CPlayer::Ready_Interact_PartCollider()
{
    CTriggerCollidePart::TRIGGER_COLLIDEPART_DESC tPartColliDesc;
    {
        PHYSICSRIGIDBODY_DESC tRigiDesc = {};
        {
            tRigiDesc.eType = EPhysicsActorType::KINEMATIC;
            tRigiDesc.bUseGravity = false;
            tRigiDesc.pOwnerMatrix = nullptr;

            tPartColliDesc.pRigidbodyDesc = &tRigiDesc;
        }

        PHYSICSCOLLIDER_DESC tPColliDesc = {};
        {
            tPColliDesc.eShape = EPhysicsShape::BOX;
            //tPColliDesc.fHeight = 100.f;
            tPColliDesc.vCenter = { 0.f, 0.75f, 0.7f };
            tPColliDesc.vExtents = { 2.f, 1.5f, 4.f };

            //tPColliDesc.fRadius = { 20.f };
            tPColliDesc.bIsTrigger = { true };
            tPColliDesc.eFilterLayer = PHYSICSFILTERGROUP::DETECT_INTERACT;
            tPColliDesc.iFilterMask =
            {
                PHYSICSFILTERGROUP::Enum::NPC
                | PHYSICSFILTERGROUP::Enum::OBJECT1
                | PHYSICSFILTERGROUP::Enum::OBJECT2
            };

            tPartColliDesc.pColliderDesc = &tPColliDesc;
        }

        tPartColliDesc.FUpdate_Flags = ENUM_TO_UINT(CTriggerCollidePart::UPDATEFLAGS::Only_Detect);
        tPartColliDesc.pMatParent = Get_Component<CTransform>()->Get_WorldMatrixPtr();

        if (FAILED(Add_Part(Part::DETECTCOLLIDER_INTERACT, ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Collider", &tPartColliDesc)))
            return E_FAIL;
    }

    return S_OK;
}

void CPlayer::Set_CurPartWeapon_State(EWEAPON eWeaponType, _uint iState)
{
    _uint iCurWeapon{}, iStartPartIdx{}, iPartSize{};

    switch (eWeaponType)
    {
    case EWEAPON::MELEE:
        // melee중에 현재 weapon
        iCurWeapon      = m_arrWeaponEnum[ENUM_TO_SZET(EWEAPON::MELEE)];
        iStartPartIdx   = m_arrMeleeInfo[size_t(iCurWeapon)].iPartStartIdx;
        iPartSize       = m_arrMeleeInfo[size_t(iCurWeapon)].iPartSize;

        if (!m_arrMeleeInfo[size_t(iCurWeapon)].bHave)
            return;

        m_arrMeleeInfo[size_t(iCurWeapon)].iWeaponState = iState;
        break;

    case EWEAPON::RANGE:
        // melee중에 현재 weapon
        iCurWeapon      = m_arrWeaponEnum[ENUM_TO_SZET(EWEAPON::RANGE)];
        iStartPartIdx   = m_arrRangeInfo[size_t(iCurWeapon)].iPartStartIdx;
        iPartSize       = m_arrRangeInfo[size_t(iCurWeapon)].iPartSize;

        if (!m_arrRangeInfo[size_t(iCurWeapon)].bHave)
            return;

        m_arrRangeInfo[size_t(iCurWeapon)].iWeaponState = iState;
        break;

    case EWEAPON::SKILL:
        // melee중에 현재 weapon
        iCurWeapon      = m_arrWeaponEnum[ENUM_TO_SZET(EWEAPON::SKILL)];
        iStartPartIdx   = m_arrSkillInfo[size_t(iCurWeapon)].iPartStartIdx;
        iPartSize       = m_arrSkillInfo[size_t(iCurWeapon)].iPartSize;

        if (!m_arrSkillInfo[size_t(iCurWeapon)].bHave)
            return;

        m_arrSkillInfo[size_t(iCurWeapon)].iWeaponState = iState;
        break;
    }

    for (_int i = 0; i < (_int)iPartSize; i++)
    {
        CWeapon* pWeapon = static_cast<CWeapon*>(Get_Part<CWeapon>(iStartPartIdx + i));

        if (pWeapon)
            pWeapon->Set_WeaponState(iState);
    }
}

void CPlayer::Count_DoubleJump(const _float fTimeDelta)
{
    // count time이 다 찼다면
    if (m_tDoubleJumpCount.CountTime(fTimeDelta) ==1.f)
    {
        // false로 바꿔줌
        m_tDoubleJumpCount.bCountTime = false;
    }
}

void CPlayer::Free()
{
    Safe_Release(m_pPhysic_QueryFilter);

    Super::Free();
}
