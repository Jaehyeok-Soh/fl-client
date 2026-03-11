#include "pch.h"
#include "Xibi_GateSpawner.h"
#include "GameInstance.h"


CXibi_GateSpawner::CXibi_GateSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Super(pDevice, pContext)
{
}

CXibi_GateSpawner::CXibi_GateSpawner(const CXibi_GateSpawner& rhs)
    : Super(rhs)
    , m_fSpreadYawDeg(rhs.m_fSpreadYawDeg)
    , m_fSpreadPitchDeg(rhs.m_fSpreadPitchDeg)
    , m_pGateOffsets(rhs.m_pGateOffsets)
{
}

HRESULT CXibi_GateSpawner::Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc)
{
    if (FAILED(Super::Initialize_Prototype(pDesc)))
        return E_FAIL;

    m_pGateOffsets = new Vec3[GATE_COUNT]
    {
        Vec3(-6.f, 3.2f, -1.f),
        Vec3(-3.5f, 1.f, -1.f),
        Vec3(-2.f, 4.3f, -2.f),
        Vec3(0.f, 2.f, -1.5f),
        Vec3(0.f, 6.f, -2.f),
        Vec3(3.f, 4.3f, -2.f),
        Vec3(4.f, 1.f, -1.f),
        Vec3(6.f, 3.f, -1.f)
    };

    return S_OK;
}

HRESULT CXibi_GateSpawner::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    XIBIGATE_COPTY_DESC* pDesc = static_cast<XIBIGATE_COPTY_DESC*>(pArg);
    if (pDesc == nullptr)
        return E_FAIL;

    m_fSpreadYawDeg = pDesc->fSpreadYawDeg;
    m_fSpreadPitchDeg = pDesc->fSpreadPitchDeg;
    
    return S_OK;
}

Vec3 CXibi_GateSpawner::Compute_GateWorldPos(_uint iGate, const Vec3& vOrigin, const Vec3& vForwardIn, const Vec3& vUpIn) const
{
    Vec3 vForward = vForwardIn;
    Vec3 vUp = vUpIn;

    if (vForward == Vec3::Zero)
        vForward = Vec3(0.f, 0.f, 1.f);

    if (vUp == Vec3::Zero)
        vUp = Vec3(0.f, 1.f, 0.f);

    vForward.Normalize();
    vUp.Normalize();

    Vec3 vRight = vUp.Cross(vForward);
    if (vRight == Vec3::Zero)
        vRight = Vec3(1.f, 0.f, 0.f);
    vRight.Normalize();

    const Vec3& local = m_pGateOffsets[iGate];
    return vOrigin + vRight * local.x + vUp * local.y + vForward * local.z;
}

Vec3 CXibi_GateSpawner::Apply_RandomSpread(const Vec3& vBaseDirIn, const Vec3& vUpIn) const
{
    Vec3 vBaseDir = vBaseDirIn;
    Vec3 vUp = vUpIn;

    if (vBaseDir == Vec3::Zero)
        return Vec3(0.f, 0.f, 1.f);

    if (vUp == Vec3::Zero)
        vUp = Vec3(0.f, 1.f, 0.f);

    vBaseDir.Normalize();
    vUp.Normalize();

    Vec3 vRight = vUp.Cross(vBaseDir);
    if (vRight == Vec3::Zero)
        vRight = Vec3(1.f, 0.f, 0.f);
    vRight.Normalize();

    const _float fYawDeg = m_pGameInstance->Rand_Float(-m_fSpreadYawDeg, m_fSpreadYawDeg);
    const _float fPitchDeg = m_pGameInstance->Rand_Float(-m_fSpreadPitchDeg, m_fSpreadPitchDeg);

    Matrix matYaw = Matrix::CreateFromAxisAngle(vUp, XMConvertToRadians(fYawDeg));
    Vec3 vSpread = Vec3::TransformNormal(vBaseDir, matYaw);

    Matrix matPitch = Matrix::CreateFromAxisAngle(vRight, XMConvertToRadians(fPitchDeg));
    vSpread = Vec3::TransformNormal(vSpread, matPitch);

    if (vSpread != Vec3::Zero)
        vSpread.Normalize();

    return vSpread;
}

void CXibi_GateSpawner::Emit_One(_uint i, const Vec3& vForward, const Vec3& vUp)
{
    for (_uint iGate = 0; iGate < GATE_COUNT; ++iGate)
    {
        Vec3 vSpawnPos = Compute_GateWorldPos(iGate, m_desc.vOrigin, vForward, vUp);

        Vec3 vBaseDir = vForward;
        if (m_desc.pTarget && m_desc.pTarget->IsDead() == false)
        {
            CTransform* pTargetTransform = m_desc.pTarget->Get_Component<CTransform>();
            if (pTargetTransform)
            {
                Vec3 vTargetPos = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
                vBaseDir = vTargetPos - vSpawnPos;
                if (vBaseDir != Vec3::Zero)
                    vBaseDir.Normalize();
            }
        }

        Vec3 vMoveDir = Apply_RandomSpread(vBaseDir, vUp);
        Vec3 vLookDir = vBaseDir;

        Spawn_SkillObject(vSpawnPos, vMoveDir, vLookDir);
    }
}

CXibi_GateSpawner* CXibi_GateSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc)
{
    CXibi_GateSpawner* pInstance = new CXibi_GateSpawner(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX("CXibi_GateSpawner::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CXibi_GateSpawner::Clone(void* pArg)
{
    CXibi_GateSpawner* pInstance = new CXibi_GateSpawner(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CXibi_GateSpawner::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CXibi_GateSpawner::Free()
{
    if (IsClone() == false)
        Safe_Delete(m_pGateOffsets);

    Super::Free();
}