#include "Engine_pch.h"
#include "Bounding_Sphere.h"
#include "Ray.h"
#include "Bounding_AABB.h"
#include "Bounding_OBB.h"
#include "DebugDraw.h"
#include "GameInstance.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CBounding_Sphere::CBounding_Sphere(const CBounding_Sphere& rhs)
    : Super(rhs)
{
    if (rhs.m_pOriginalDesc)
    {
        m_pDesc = new BoundingSphere(*rhs.m_pOriginalDesc);
    }
    if (rhs.m_pDesc)
    {
        m_pDesc = new BoundingSphere(*rhs.m_pDesc);
    }
}


HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pInitialDesc)
{
    const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*>(pInitialDesc);
    if (pDesc->pMinMax == nullptr)
    {
        m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
        m_pDesc = new BoundingSphere(*m_pOriginalDesc);
    }
    else
    {
        m_pOriginalDesc = new BoundingSphere;
        BoundingSphere::CreateFromPoints(*m_pOriginalDesc, 2, pDesc->pMinMax, sizeof(Vec3));
        m_pDesc = new BoundingSphere(*m_pOriginalDesc);
    }

    return S_OK;
}

void CBounding_Sphere::Update(const Matrix &WorldMatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}
_bool CBounding_Sphere::Intersect_Bounding(EColliderType eType, CBounding* pOther)
{
    _bool bIsColl = { false };

    switch (eType)
    {
    case Engine::EColliderType::SPHERE:
        bIsColl = m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pOther)->Get_Desc());
        break;
    case Engine::EColliderType::AABB:
        bIsColl = m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pOther)->Get_Desc());
        break;
    case Engine::EColliderType::OBB:
        bIsColl = m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pOther)->Get_Desc());
        break;
    }

    return bIsColl;
}

_bool CBounding_Sphere::IntersectWithRay_World(CGameInstance* pGameInstance, OUT Vec3& vOut)
{
    return pGameInstance->IntersectrayWithSphere_World(m_pDesc, vOut);
}
_bool CBounding_Sphere::IntersectWithRay_Local(CGameInstance* pGameInstance, OUT Vec3& vOut)
{
    return pGameInstance->IntersectrayWithSphere_Local(m_pOriginalDesc, vOut);
}

_bool CBounding_Sphere::IntersectWithRay_World(CRay* pRay, OUT Vec3& vOut)
{
    return pRay->IntersectrayWithSphere_World(m_pDesc, vOut);
}
_bool CBounding_Sphere::IntersectWithRay_Local(CRay* pRay, OUT Vec3& vOut)
{
    return pRay->IntersectrayWithSphere_Local(m_pOriginalDesc, vOut);
}

#ifdef _DEBUG
HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
    DX::Draw(pBatch, *m_pDesc, true == isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif
CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc)
{
    CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_Sphere");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBounding_Sphere::Free()
{
    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
    Super::Free();
}
