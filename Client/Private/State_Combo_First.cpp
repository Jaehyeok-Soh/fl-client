#include "pch.h"
#include "State_Combo_First.h"
#include "Player.h"
#include "GameInstance.h"

CState_Combo_First::CState_Combo_First(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "ComboFirst")
{
}

HRESULT CState_Combo_First::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Combo_First::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    Set_AnimationPlayRate(2.5f);
    return S_OK;
}

HRESULT CState_Combo_First::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    SetupLook_Target_XZ();
    return S_OK;
}

void CState_Combo_First::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    _int iNewWindow = -1;
    _int iNewPart = -1;
    ATTACK_DESC desc = {};

    //if (Is_AnimTrackPositionBetween(0.3f, 0.4f))
    //{
    //    iNewWindow = 0;
    //    iNewPart = ENUM_TO_UINT(CPlayer::Part::LEFTHAND);
    //    desc.iAttackType = ENUM_TO_UINT(EHitType::LEFT);
    //    desc.iDamage = 1;
    //    if (m_bFirst == false)
    //    {
    //        CGameInstance::GetInstance()->Play_RandOneShot(L"HandAttack", 0.2f, 3);
    //        CGameInstance::GetInstance()->Play_RandOneShot(L"Player_Hit", 0.3f, 6);
    //        m_bFirst = true;
    //    }
    //}

    Apply_AttackDesc(iNewWindow, iNewPart, &desc);
}

HRESULT CState_Combo_First::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    m_bFirst = { false };
    //Set_AttackCollider(ENUM_TO_UINT(CPlayer::Part::LEFTHAND), false, nullptr);
    return S_OK;
}

CState_Combo_First* CState_Combo_First::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_Combo_First* pInstance = new CState_Combo_First(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_Combo_First::Create");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_Combo_First::Free()
{
    Super::Free();
}
