#include "Engine_pch.h"
#include "ProjectileSpawner_Radial360.h"
#include "GameInstance.h"

CProjectileSpawner_Radial360::CProjectileSpawner_Radial360(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{

}

CProjectileSpawner_Radial360::CProjectileSpawner_Radial360(const CProjectileSpawner_Radial360& rhs)
    : Super(rhs)
{

}

HRESULT CProjectileSpawner_Radial360::Initialize_Prototype(SPAWNER_ORIGIN_DESC* pArg)
{
    if (FAILED(Super::Initialize_Prototype(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CProjectileSpawner_Radial360::Initialize(void *pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    PR_SPAWNER_RADIAL_DESC* pDesc = static_cast<PR_SPAWNER_RADIAL_DESC*>(pArg);
    m_iCount = pDesc->iCount;

    return S_OK;
}

void CProjectileSpawner_Radial360::Emit_One(_uint i, const Vec3& vFoward, const Vec3& vUp)
{
    const _uint iCount = (std::max)(1u, m_iCount);
    const _float fYaw = XMConvertToRadians(360.f * ((_float)i / (_float)iCount));

    Matrix matResult = Matrix::CreateFromAxisAngle(vUp, fYaw);
    Vec3 vDir = Vec3::TransformNormal(vFoward, matResult);
    vDir.Normalize();

    Spawn_SkillObject(m_desc.vOrigin, vDir);
}

CProjectileSpawner_Radial360* CProjectileSpawner_Radial360::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc)
{
    CProjectileSpawner_Radial360* pInstance = new CProjectileSpawner_Radial360(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX("CProjectileSpawner_Radial360::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CProjectileSpawner_Radial360::Clone(void* pArg)
{
    CProjectileSpawner_Radial360* pInstance = new CProjectileSpawner_Radial360(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CProjectileSpawner_Radial360::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CProjectileSpawner_Radial360::Free()
{
    Super::Free();
}