#include "Engine_pch.h"
#include "ProjectileSpawner_Fan.h"
#include "Engine_Utils.h"
#include "GameInstance.h"
#include "ProjectileSpawner_Radial360.h"

CProjectileSpawner_Fan::CProjectileSpawner_Fan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CProjectileSpawner_Fan::CProjectileSpawner_Fan(const CProjectileSpawner_Fan& rhs)
	: Super(rhs)
{
}

HRESULT CProjectileSpawner_Fan::Initialize_Prototype(SPAWNER_ORIGIN_DESC* pDesc)
{
	if (FAILED(Super::Initialize_Prototype(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CProjectileSpawner_Fan::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

    PR_SPAWNER_FAN_DESC* pDesc = static_cast<PR_SPAWNER_FAN_DESC*>(pArg);
    m_iCount = pDesc->iCount;
    m_fSpreadDeg = pDesc->fSpreadDeg;

	return S_OK;
}

void CProjectileSpawner_Fan::Emit_One(_uint i, const Vec3& vFoward, const Vec3& vUp)
{
    const _uint iCount = (std::max)(1u, m_iCount);
    Vec3 vDir = vFoward;

    if (iCount > 1)
    {
        const _float fA0 = -m_fSpreadDeg * 0.5f;
        const _float fA1 = +m_fSpreadDeg * 0.5f;
        const _float fT = (_float)i / (_float)(iCount - 1);
        const _float fYaw = XMConvertToRadians(std::lerp(fA0, fA1, fT));

        Matrix matResult = Matrix::CreateFromAxisAngle(vUp, fYaw);
        vDir = Vec3::TransformNormal(vFoward, matResult);
        vDir.Normalize();
    }

    Spawn_SkillObject(m_desc.vOrigin, vDir);
}

CProjectileSpawner_Fan* CProjectileSpawner_Fan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, SPAWNER_ORIGIN_DESC* pDesc)
{
    CProjectileSpawner_Fan* pInstance = new CProjectileSpawner_Fan(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype(pDesc)))
    {
        MSG_BOX("CProjectileSpawner_Fan::Create(), Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CProjectileSpawner_Fan::Clone(void* pArg)
{
    CProjectileSpawner_Fan* pInstance = new CProjectileSpawner_Fan(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CProjectileSpawner_Fan::Clone(), Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CProjectileSpawner_Fan::Free()
{
    Super::Free();
}