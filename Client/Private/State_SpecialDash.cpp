#include "pch.h"
#include "State_SpecialDash.h"

#include "GameObject.h"
#include "PhysicsCCT.h"
#include "Transform.h"
#include "PlayerActionState.h"

#define Duration 30.f / ANIMTIC
#define OneTrackTime  50.f / ANIMTIC / 4.f
#define RightAcc 5.f
#define LookAcc 4.f

CState_SpecialDash::CState_SpecialDash(CActionState* pOwnerComponent)
    : Super(pOwnerComponent, "SpecialDash")
{
}

HRESULT CState_SpecialDash::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_SpecialDash::Awake(const _uint iLevelIndex)
{
    if (FAILED(Super::Awake(iLevelIndex)))
        return E_FAIL;

    return S_OK;
}

HRESULT CState_SpecialDash::Start(void* pArg, _bool bForce)
{
    if (FAILED(Super::Start(pArg, bForce)))
        return E_FAIL;

    Set_RootMotion_Apply(false);

    // pivot position을 바라 봄

    CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    if (pPlayerTransform == nullptr || pPlayerCCT == nullptr) return E_FAIL;

    m_vPivot = static_cast<CPlayerActionState*>(m_pOwnerStateComp)->Get_PivotPos();
    pPlayerTransform->Look_At_XZ(m_vPivot);
    pPlayerCCT->SetZeroHorizontalVelocity();

    Vec3 vLookDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::LOOK);
    Vec3 vRightDir = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::RIGHT);
    vLookDir.Normalize();
    vRightDir.Normalize();

    m_arrPoses[0] = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

    _float fAcc = 1.f;
    for (size_t i = 1; i < m_arrPoses.size(); i++)
    {
        m_arrPoses[i] = m_arrPoses[i - 1] + vRightDir * RightAcc + vLookDir * (LookAcc * fAcc);

        fAcc += 1.f;
    }

    return S_OK;
}

void CState_SpecialDash::Update(const _float fTimeDelta)
{
    CTransform* pPlayerTransform = Get_OwnerObject()->Get_Component<CTransform>();
    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();

    Vec3 vPrePos = pPlayerTransform->Get_Info(TRANSFORM_INFO_STATE::POS);

    _uint iTrackPos = 0;
    _float fRatio = 0.f;
    for (iTrackPos; iTrackPos < 3; iTrackPos++)
    {
        if (m_fStateElapsed < (iTrackPos + 1) * OneTrackTime)
        {
            _float fLeftTime = OneTrackTime * iTrackPos;
            _float fRightTime = OneTrackTime * (iTrackPos + 1);

            fRatio = (m_fStateElapsed - fLeftTime) / (fRightTime - fLeftTime);

            break;
        }
    }

    // 루프 후 클램프 처리 추가
    if (iTrackPos >= 3)
    {
        iTrackPos = 2;
        fRatio = 1.f;
    }

    Vec3 vResultPos = { Vec3::Zero };
    if (iTrackPos < 2)
        vResultPos = Vec3::CatmullRom(vPrePos, m_arrPoses[iTrackPos], m_arrPoses[iTrackPos + 1], m_arrPoses[iTrackPos + 2], fRatio);
    else
        vResultPos = Vec3::CatmullRom(vPrePos, m_arrPoses[iTrackPos], m_arrPoses[iTrackPos + 1], m_arrPoses[iTrackPos + 1], fRatio);

    pPlayerCCT->SetFootPosition(vResultPos);
    pPlayerTransform->Look_At_XZ(m_vPivot);

    Super::Update(fTimeDelta);
}

HRESULT CState_SpecialDash::End()
{
    if (FAILED(Super::End()))
        return E_FAIL;

    Set_RootMotion_Apply(true);

    CPhysicsCCT* pPlayerCCT = Get_OwnerObject()->Get_Component<CPhysicsCCT>();
    pPlayerCCT->SetZeroHorizontalVelocity();

    return S_OK;
}

_bool CState_SpecialDash::Can_CheckKey(const _float fTimeDelta)
{
    return Is_MainAnimFinished();
}

CState_SpecialDash* CState_SpecialDash::Create(CActionState* pOwnerComponent, void* pArg)
{
    CState_SpecialDash* pInstance = new CState_SpecialDash(pOwnerComponent);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CState_SpecialDash::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CState_SpecialDash::Free()
{
    Super::Free();
}
