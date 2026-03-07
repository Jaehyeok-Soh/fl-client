#include "pch.h"
#include "State_GunBase.h"

#include "Player.h"
#include "ControlContext.h"
#include "PhysicsCCT.h"
#include "CameraMan_Targeter.h"

#include "GameInstance.h"

CState_GunBase::CState_GunBase(CActionState* pOwnerComponent, const string& strName)
    : Super(pOwnerComponent, strName)
{
}

HRESULT CState_GunBase::Initialize(void* pArg)
{
    GUN_STATEBASE_DESC* pDesc = static_cast<GUN_STATEBASE_DESC*>(pArg);
    m_MixAnim_Indices = pDesc->arrMixAnims;

    PLAYER_STATEBASE_DESC tSuperDesc    = {};
    tSuperDesc.bBlend                   = true;
    tSuperDesc.bLoop                    = pDesc->bLoop;
    tSuperDesc.pOwnerGun                = pDesc->pOwnerGun;
    tSuperDesc.vecMainAnims             = { pDesc->iMainAnimIdx };


    // todo down도 넣어야하나
    tSuperDesc.FCollis = CStateBase_Player::COLLISIONFLAGS::C_DOWN
        | CStateBase_Player::COLLISIONFLAGS::C_Strong
        | CStateBase_Player::COLLISIONFLAGS::C_Fly;

    vector<_uint> vecChangeState_ByKey{};
    vecChangeState_ByKey.resize(ENUM_TO_SZET(CStateBase_Player::STATEKEY::END), ENUM_TO_UINT(CPlayer::State::END));
    tSuperDesc.vecChangeState_ByKey = vecChangeState_ByKey;

    if (FAILED(Super::Initialize(&tSuperDesc)))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CState_GunBase::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_GunBase::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    GUN_START_DESC* pDesc = static_cast<GUN_START_DESC*>(pArg);

    // desc이 없다면 바닥 충돌 검사후 move state 설정 : 외부 -> Gun base
    if (!pDesc)
    {
        if (Check_OnGround(0.3f))
            Change_MoveState(MoveState::GROUND);

        else
            Change_MoveState(MoveState::FALL);

        CStateBase::SetupLook_CameraLook();
    }

    // desc 정보로 move state 설정
    else
        Change_MoveState(pDesc->eMoveState);

    // cameara state change
    static_cast<CPlayer*>(Get_OwnerObject())->Change_CamState(ENUM_TO_UINT(Client::TargeterState::GUN));

    return S_OK;
}

void CState_GunBase::Update(const _float fTimeDelta)
{
    // 0. 만약 r button 눌림이 끝났다면 gun state 탈출
    if (MOUSE_RBUTTON_UP)
    {
        // move 상태에 따라
        GunEnd();
        return;
    }

    // 1. shif, mouse L, skill 키 검사
    if (Check_BaseKey(fTimeDelta))
    {
        Request_MixAnimation(1, -1);
        return;
    }

    // 2. 움직임키 flag check
    Check_KeyFlag(fTimeDelta);

    // 3. 키값을 받은걸 바탕으로 움직인다 : 8방향
    GunMove(fTimeDelta);

    // 4. move state update
    Move_Update(fTimeDelta);
}

HRESULT CState_GunBase::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    // cameara state change
    static_cast<CPlayer*>(Get_OwnerObject())->Change_CamState(ENUM_TO_UINT(Client::TargeterState::NORMAL));

    return S_OK;
}

void CState_GunBase::Change_PlayerState(STATEKEY eKey)
{
    if (eKey == STATEKEY::SHIFT &&
        (m_eMoveState == MoveState::GROUND) &&
        Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE))
        )
    {
        Request_Change_State(ENUM_TO_UINT(CPlayer::State::RUNSHORT));
    }

    else
        Super::Change_PlayerState(eKey);
}

void CState_GunBase::Check_KeyFlag(const _float fTimeDelta)
{
    // 키 입력을 받아 온다
    m_FKeyFlags = 0;

    if (KEY_BUTTON_HOLD(DIK_W))
        Engine_Utils::Add_Flag(m_FKeyFlags, KeyFlag::W);

    if (KEY_BUTTON_HOLD(DIK_S))
        Engine_Utils::Add_Flag(m_FKeyFlags, KeyFlag::S);

    if (KEY_BUTTON_HOLD(DIK_A))
        Engine_Utils::Add_Flag(m_FKeyFlags, KeyFlag::A);

    if (KEY_BUTTON_HOLD(DIK_D))
        Engine_Utils::Add_Flag(m_FKeyFlags, KeyFlag::D);

    if (KEY_BUTTON_DOWN(DIK_SPACE))
        Engine_Utils::Add_Flag(m_FKeyFlags, KeyFlag::Space);
}

_bool CState_GunBase::Check_BaseKey(const _float fTimeDelta)
{
    if (Check_DashKey(fTimeDelta))
        return true;

    if (Check_MeleeKey(fTimeDelta))
        return true;

    if (Check_SkillKey(fTimeDelta))
        return true;

    return false;
}

void CState_GunBase::Move_Update(const _float fTimeDelta)
{
    switch (m_eMoveState)
    {
    case MoveState::GROUND:
        Ground_Update(fTimeDelta);
        break;

    case MoveState::JUMP:
        Jump_Update(fTimeDelta);
        break;

    case MoveState::FALL:
        Fall_Update(fTimeDelta);
        break;
    }
}

void CState_GunBase::Ground_Update(const _float fTimeDelta)
{
    // 3.1 jump 우선 판정
    if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_Jump))
    {
        Change_MoveState(MoveState::JUMP);
        return;
    }

    // 3.2 움직임 키가 없었다라면
    else if (!Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::MoveKeyOn))
    {
        // 안 섞겠다
        Request_MixAnimation(1, -1);
        return;
    }

    // 3.3 하체 mix anim 판정
    else
    {
        // anim mix 셋팅
        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_LF))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[LF]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_LB))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[LB]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_RF))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[RF]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_RB))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[RB]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_F))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[F]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_R))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[R]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_L))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[L]);
            return;
        }

        if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_B))
        {
            Request_MixAnimation(1, m_MixAnim_Indices[B]);
            return;
        }
    }   
}

void CState_GunBase::Jump_Update(const _float fTimeDelta)
{
    if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyMask::Mask_Jump))
    {
        Request_MixAnimation(1, -1);
        Request_Change_State(ENUM_TO_UINT(CPlayer::State::JUMPDOUBLE));
        return;
    }

    Jump(fTimeDelta);

    // cool 타임 검사 -> fall
    m_TJumpTime.x += fTimeDelta;
    if (m_TJumpTime.x >= m_TJumpTime.y)
    {
        Change_MoveState(MoveState::FALL);
        return;
    }

    // 바닥 충돌 검사 -> Land
    if (m_TJumpTime.x > 0.15f &&
        IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_DOWN))
    {
        Change_MoveState(MoveState::GROUND);
    }

}

void CState_GunBase::Fall_Update(const _float fTimeDelta)
{
    // 바닥 충돌 검사 -> Land
    if (Check_OnGround(0.3f))
    {
        Request_MixAnimation(1, -1);
        Change_MoveState(MoveState::GROUND);
    }

}

_bool CState_GunBase::Change_MoveState(MoveState eState)
{
    // 달라 졌다면 change
    if (m_eMoveState != eState)
    {
        End_MoveState(m_eMoveState);

        m_eMoveState = eState;

        Start_MoveState(m_eMoveState);

        return true;
    }

    return false;
}

void CState_GunBase::Start_MoveState(MoveState eNextState)
{
    // 공통 초기화 값
    m_FKeyFlags = 0;
    m_TJumpTime.x = 0.f;
    m_TLandTime.x = 0.f;

    m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::E)] = ENUM_TO_UINT(CPlayer::State::SKILL1);

    switch (eNextState)
    {
    case MoveState::GROUND:
        Request_MixAnimation(1, -1);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::SHIFT)] = ENUM_TO_UINT(CPlayer::State::DASHBACK);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::Q)] = ENUM_TO_UINT(CPlayer::State::SKILL2);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::LM)] = ENUM_TO_UINT(CPlayer::State::COMBO);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::CHARGE)] = ENUM_TO_UINT(CPlayer::State::CHARGE);
        break;

    case MoveState::JUMP:
        Set_ApplyGravity(false);
        Request_MixAnimation(1, m_MixAnim_Indices[JUMP]);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::SHIFT)]   = ENUM_TO_UINT(CPlayer::State::DASHSKY);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::Q)]       = m_iEndStateIdx;
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::LM)]      = ENUM_TO_UINT(CPlayer::State::JUMPATTSTART);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::CHARGE)]  = m_iEndStateIdx;
        break;

    case MoveState::FALL:
        Request_MixAnimation(1, m_MixAnim_Indices[FALL]);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::SHIFT)]   = ENUM_TO_UINT(CPlayer::State::DASHSKY);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::Q)]       = m_iEndStateIdx;
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::LM)]      = ENUM_TO_UINT(CPlayer::State::JUMPATTSTART);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::CHARGE)]  = m_iEndStateIdx;

        Set_GravityOffset(8.f);
        break;
    }
}

void CState_GunBase::End_MoveState(MoveState ePreState)
{
    Request_MixAnimation(1, -1);

    switch (ePreState)
    {
    case MoveState::GROUND:
        break;

    case MoveState::JUMP:
        Set_ApplyGravity(true);
        break;

    case MoveState::FALL:
        Set_GravityOffset(0.f);
        break;
    }
}

void CState_GunBase::GunEnd()
{
    // 1번 mix 정보를 죽인다
    Request_MixAnimation(1, -1);

    // gun state 빠져 나올때
    switch (m_eMoveState)
    {
     case MoveState::GROUND:
         if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
         {
             Super::Change_PlayerState(ENUM_TO_UINT(CPlayer::State::GUNWALK));
         }
         else
             Super::Change_PlayerState(ENUM_TO_UINT(CPlayer::State::GUNIDLE));

         break;

     case MoveState::JUMP:
         Super::Change_PlayerState(ENUM_TO_UINT(CPlayer::State::FALL));
         break;

     case MoveState::FALL:
         Super::Change_PlayerState(ENUM_TO_UINT(CPlayer::State::FALL));
         break;
    }
}

void CState_GunBase::Jump(const _float fTimeDelta)
{
    CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();
    _float moveps = pPlayerTrans->Get_MovePerSec(); // 속도

    Vec3 vUp = (pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::UP));
    vUp.Normalize();

    Vec3 accelation = vUp;

    Move(accelation);
}

void CState_GunBase::GunMove(const _float fTimeDelta)
{
    CTransform* pPlayerTrans = Get_OwnerObject()->Get_Component<CTransform>();

    Vec3 vRight = pPlayerTrans->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    vRight.y = 0.f;
    vRight.Normalize();

    Vec3 vFront = Vec3(vRight.z, 0.f, -vRight.x);
    vFront.Normalize();

    Vec3 vDir = Vec3::Zero;

    if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyFlag::W))
    {
        vDir -= vFront;
    }

    if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyFlag::S))
    {
        vDir += vFront;
    }

    if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyFlag::A))
    {
        vDir -= vRight;
    }

    if (Engine_Utils::Has_Flag(m_FKeyFlags, KeyFlag::D))
    {
        vDir += vRight;
    }

    if (::XMVector3Equal(vDir, Vec3::Zero) == false)
        vDir.Normalize();
    SetCCTInputDirection(vDir);
}

void CState_GunBase::Free()
{
    Super::Free();
}
