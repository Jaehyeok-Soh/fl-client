#include "pch.h"
#include "State_Combo_Fourth.h"
#include "Player.h"
#include "GameInstance.h"

CState_Combo_Fourth::CState_Combo_Fourth(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "ComboFourth")
{
}

HRESULT CState_Combo_Fourth::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Combo_Fourth::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    Set_AnimationPlayRate(2.5f);
    return S_OK;
}

HRESULT CState_Combo_Fourth::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    SetupLook_Target_XZ();
    return S_OK;
}

void CState_Combo_Fourth::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    _int iNewWindow = -1;
    _int iNewPart = -1;
    ATTACK_DESC desc = {};

    if (Can_Dash(EDashFlag::FIRST) && Is_AnimTrackPositionAt(0.1f))
        StartForce_ForAnimation(EDashFlag::FIRST, EDir::FRONT, 8.f, 3.f);

    if (Can_Dash(EDashFlag::SECOND) && Is_AnimTrackPositionAt(0.49f))
        StartForce_ForAnimation(EDashFlag::SECOND, EDir::FRONT, 8.f, 3.f);

    //if (Is_AnimTrackPositionBetween(0.23f, 0.35f))
    //{
    //    iNewWindow = 0;
    //    iNewPart = ENUM_TO_UINT(CPlayer::Part::LEFTFOOT);
    //    desc.iAttackType = ENUM_TO_UINT(EHitType::LEFT);
    //    desc.iDamage = 1;
    //    if (m_bFirst == false)
    //    {
    //        CGameInstance::GetInstance()->Play_OneShot(L"WindMeel", 0.15f);
    //        CGameInstance::GetInstance()->Play_RandOneShot(L"Player_Hit", 0.3f, 6);
    //        m_bFirst = true;
    //    }
    //}
    //else if (Is_AnimTrackPositionBetween(0.36f, 0.47f))
    //{
    //    iNewWindow = 1;
    //    iNewPart = ENUM_TO_UINT(CPlayer::Part::RIGHTFOOT);
    //    desc.iAttackType = ENUM_TO_UINT(EHitType::RIGHT);
    //    desc.iDamage = 1;
    //    if (m_bSecond == false)
    //    {
    //        CGameInstance::GetInstance()->Play_OneShot(L"WindMeel", 0.15f);
    //        m_bSecond = true;
    //    }
    //}
    //else if (Is_AnimTrackPositionBetween(0.49f, 0.53f))
    //{
    //    iNewWindow = 2;
    //    iNewPart = ENUM_TO_UINT(CPlayer::Part::RIGHTFOOT);
    //    desc.fForceAbs = 10.f;
    //    desc.fDragK = 2.f;
    //    desc.bImpact = true;
    //    desc.iAttackType = ENUM_TO_UINT(EHitType::SPINBLOWUP);
    //    desc.iDamage = 1;
    //    if (m_bThird == false)
    //    {
    //        CGameInstance::GetInstance()->Play_OneShot(L"WindMeel", 0.15f);
    //        CGameInstance::GetInstance()->Play_RandOneShot(L"Player_Hit", 0.3f, 6);
    //        m_bThird = true;
    //    }
    //}

    Apply_AttackDesc(iNewWindow, iNewPart, &desc);
    Apply_ForceMove(fTimeDelta);
}

HRESULT CState_Combo_Fourth::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    m_bFirst = { false };
    m_bSecond = { false };
    m_bThird = { false };
    Clear_ForceMove();
    //Set_AttackCollider(ENUM_TO_UINT(CPlayer::Part::LEFTFOOT), false, nullptr);
    //Set_AttackCollider(ENUM_TO_UINT(CPlayer::Part::RIGHTFOOT), false, nullptr);
    return S_OK;
}

CState_Combo_Fourth* CState_Combo_Fourth::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_Combo_Fourth* pInstance = new CState_Combo_Fourth(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_Combo_Fourth::Create");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_Combo_Fourth::Free()
{
    Super::Free();
}
