#include "pch.h"
#include "State_Combo_Third.h"
#include "Player.h"
#include "GameInstance.h"

CState_Combo_Third::CState_Combo_Third(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "ComboThird")
{
}

HRESULT CState_Combo_Third::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Combo_Third::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    Set_AnimationPlayRate(1.5f);
    return S_OK;
}

HRESULT CState_Combo_Third::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    SetupLook_Target_XZ();
    return S_OK;
}

void CState_Combo_Third::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    _int iNewWindow = -1;
    _int iNewPart = -1;
    ATTACK_DESC desc = {};

    if (Can_Dash(EDashFlag::FIRST) && Is_AnimTrackPositionAt(0.1f))
        StartForce_ForAnimation(EDashFlag::FIRST, EDir::FRONT, 4.f, 7.f);

    //if (Is_AnimTrackPositionBetween(0.29f, 0.37f))
    //{
    //    iNewWindow = 0;
    //    iNewPart = ENUM_TO_UINT(CPlayer::Part::LEFTFOOT);
    //    desc.fForceAbs = 4.f;
    //    desc.fDragK = 7.f;
    //    desc.iAttackType = ENUM_TO_UINT(EHitType::FRONT);
    //    desc.iDamage = 1;
    //    if (m_bFirst == false)
    //    {
    //        CGameInstance::GetInstance()->Play_RandOneShot(L"HandAttack", 0.2f, 3);
    //        CGameInstance::GetInstance()->Play_RandOneShot(L"Player_Hit", 0.3f, 6);
    //        m_bFirst = true;
    //    }
    //}

    Apply_AttackDesc(iNewWindow, iNewPart, &desc);
    Apply_ForceMove(fTimeDelta);
}

HRESULT CState_Combo_Third::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    m_bFirst = false;
    Clear_ForceMove();
    //Set_AttackCollider(ENUM_TO_UINT(CPlayer::Part::LEFTFOOT), false, nullptr);
    return S_OK;
}

CState_Combo_Third* CState_Combo_Third::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_Combo_Third* pInstance = new CState_Combo_Third(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_Combo_Third::Create");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_Combo_Third::Free()
{
    Super::Free();
}
