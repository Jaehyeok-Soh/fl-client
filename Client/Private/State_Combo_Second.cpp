#include "Client_Defines.h"
#include "Player.h"
#include "GameInstance.h"
#include "State_Combo_Second.h"

CState_Combo_Second::CState_Combo_Second(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "ComboSecond")
{
}

HRESULT CState_Combo_Second::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_Combo_Second::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    Set_AnimationPlayRate(2.5f);
    return S_OK;
}

HRESULT CState_Combo_Second::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    SetupLook_Target_XZ();
    return S_OK;
}

void CState_Combo_Second::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

    _int iNewWindow = -1;
    _int iNewPart = -1;
    ATTACK_DESC desc = {};

    if (Is_AnimTrackPositionBetween(0.32f, 0.4f))
    {
        iNewWindow = 0;
        iNewPart = ENUM_TO_UINT(CPlayer::Part::RIGHTHAND);
        desc.iAttackType = ENUM_TO_UINT(EHitType::RIGHT);
        desc.iDamage = 1;
        if (m_bFirst == false)
        {
            CGameInstance::GetInstance()->Play_RandOneShot(L"HandAttack", 0.2f, 3);
            CGameInstance::GetInstance()->Play_RandOneShot(L"Player_Hit", 0.3f, 6);
            m_bFirst = true;
        }
    }

    Apply_AttackDesc(iNewWindow, iNewPart, &desc);
}

HRESULT CState_Combo_Second::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    m_bFirst = false;
    Set_AttackCollider(ENUM_TO_UINT(CPlayer::Part::RIGHTHAND), false, nullptr);
    return S_OK;
}

CState_Combo_Second* CState_Combo_Second::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_Combo_Second* pInstance = new CState_Combo_Second(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_Combo_Second::Create");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_Combo_Second::Free()
{
    Super::Free();
}
