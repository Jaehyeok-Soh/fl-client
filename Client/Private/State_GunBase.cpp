#include "pch.h"
#include "State_GunBase.h"

#include "Player.h"
#include "ControlContext.h"

#include "Engine_Utils.h"
#include "GameInstance.h"

CState_GunBase::CState_GunBase(CActionState* pOwnerComponent, const string& strName)
    : Super(pOwnerComponent, strName)
{
}

HRESULT CState_GunBase::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
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

    return S_OK;
}

void CState_GunBase::Update(const _float fTimeDelta)
{
    // 만약 r button 눌림이 끝났다면 gun state 탈출
    if (MOUSE_RBUTTON_UP)
    {
        // move 상태에 따라
        GunEnd();
        return;
    }

    Move_Update(fTimeDelta);
}

HRESULT CState_GunBase::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    return S_OK;
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

    if (KEY_BUTTON_HOLD(DIK_SPACE))
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

    case MoveState::LAND:
        Land_Update(fTimeDelta);
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
    // 1. shif, mouse L, skill 키 검사
    if (Check_BaseKey(fTimeDelta))
        return;

    // 2. 움직임키 flag check
    Check_KeyFlag(fTimeDelta);

    // 3. key flag를 기준으로 하체 상태 제어

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
        // 일단 움직이고
        Align_Movement(fTimeDelta);

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
    if (Check_BaseKey(fTimeDelta))
        return;

    // 움직임 그냥 적용
    Align_Movement(fTimeDelta);

    // cool 타임 검사 -> fall
    m_TJumpTime.x += fTimeDelta;
    if (m_TJumpTime.x >= m_TJumpTime.y)
    {
        Change_MoveState(MoveState::FALL);
        return;
    }

    // 바닥 충돌 검사 -> Land
    if (IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_DOWN))
        Change_MoveState(MoveState::LAND);
}

void CState_GunBase::Fall_Update(const _float fTimeDelta)
{
    if (Check_BaseKey(fTimeDelta))
        return;

    // 움직임 그냥 적용
    Align_Movement(fTimeDelta);

    // 바닥 충돌 검사 -> Land
    if (IsOn_CCTFlag(PxControllerCollisionFlag::Enum::eCOLLISION_DOWN))
        Change_MoveState(MoveState::LAND);
}

void CState_GunBase::Land_Update(const _float fTimeDelta)
{
    if (Check_BaseKey(fTimeDelta))
        return;

    // wasd key || cool 타임-> Ground
    m_TLandTime.x += fTimeDelta;
    if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)) ||
        m_TLandTime.x >= m_TLandTime.y)
    {
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
    m_FKeyFlags = 0;
    m_TJumpTime.x = 0.f;
    m_TLandTime.x = 0.f;

    m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::E)] = ENUM_TO_UINT(CPlayer::State::SKILL1);

    switch (eNextState)
    {
    case MoveState::GROUND:
    case MoveState::LAND:
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::SHIFT)]   = ENUM_TO_UINT(CPlayer::State::DASHBACK);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::Q)]       = ENUM_TO_UINT(CPlayer::State::SKILL2);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::LM)]      = ENUM_TO_UINT(CPlayer::State::COMBO);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::CHARGE)]  = ENUM_TO_UINT(CPlayer::State::CHARGE);
        break;

    case MoveState::JUMP:
    case MoveState::FALL:
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::SHIFT)]   = ENUM_TO_UINT(CPlayer::State::DASHSKY);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::Q)]       = m_iEndStateIdx;
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::LM)]      = ENUM_TO_UINT(CPlayer::State::JUMPATTSTART);
        m_vecChangeState_ByKey[ENUM_TO_SZET(STATEKEY::CHARGE)]  = m_iEndStateIdx;
        break;
    }
}

void CState_GunBase::End_MoveState(MoveState ePreState)
{
    switch (ePreState)
    {
    case MoveState::GROUND:
        break;

    case MoveState::LAND:
        break;

    case MoveState::JUMP:
        break;

    case MoveState::FALL:
        // 중력 offset 리셋
        break;


    }
}

void CState_GunBase::GunEnd()
{
    // gun state 빠져 나올때
    switch (m_eMoveState)
    {
     case MoveState::GROUND:
     case MoveState::LAND:
         if (Key_Input(ENUM_TO_UINT(CControlContext::CONTROL_KEY::MOVE)))
         {
             Change_PlayerState(ENUM_TO_UINT(CPlayer::State::WALK));
         }
         else
             Change_PlayerState(ENUM_TO_UINT(CPlayer::State::IDLE));
         break;

     case MoveState::JUMP:
         Change_PlayerState(ENUM_TO_UINT(CPlayer::State::FALL));
         break;

     case MoveState::FALL:
         Change_PlayerState(ENUM_TO_UINT(CPlayer::State::FALL));
         break;
    }
}

void CState_GunBase::Free()
{
    Super::Free();
}
