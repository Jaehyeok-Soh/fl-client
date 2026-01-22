#include "Bounding_AABB.h"
#include "DebugDraw.h"
#include "Ray.h"
#include "Bounding_OBB.h"
#include "Bounding_Sphere.h"
#include "GameInstance.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pInitializeDesc)
{
    if (pInitializeDesc)
    {
        const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(pInitializeDesc);
        
        m_pOriginalDesc = new  BoundingBox(pDesc->vCenter, pDesc->vExtens);
        m_pDesc = new BoundingBox(*m_pOriginalDesc);
    }

    return S_OK;
}

void CBounding_AABB::Update(_fmatrix matWorld)
{
    _matrix matSrc = matWorld;
    
    matSrc.r[0] = ::XMVectorSet(1.f, 0.f, 0.f, 0.f) * ::XMVector3Length(matWorld.r[0]);
    matSrc.r[1] = ::XMVectorSet(0.f, 1.f, 0.f, 0.f) * ::XMVector3Length(matWorld.r[1]);
    matSrc.r[2] = ::XMVectorSet(0.f, 0.f, 1.f, 0.f) * ::XMVector3Length(matWorld.r[2]);

    m_pOriginalDesc->Transform(*m_pDesc, matSrc);
}

_bool CBounding_AABB::Intersect_Bounding(EColliderType eType, CBounding* pOther)
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

_bool CBounding_AABB::IntersectWithRay_World(class CGameInstance* pGameInstance, OUT _float4& vOut)
{
    return pGameInstance->IntersectrayWithAABB_World(m_pDesc, vOut);
}

_bool CBounding_AABB::IntersectWithRay_Local(class CGameInstance* pGameInstance, OUT _float4& vOut)
{
    return pGameInstance->IntersectrayWithAABB_Local(m_pOriginalDesc, vOut);
}

_bool CBounding_AABB::IntersectWithRay_World(CRay* pRay, OUT _float4& vOut)
{
    return pRay->IntersectrayWithAABB_World(m_pDesc, vOut);
}

_bool CBounding_AABB::IntersectWithRay_Local(CRay* pRay, OUT _float4& vOut)
{
    return pRay->IntersectrayWithAABB_Local(m_pOriginalDesc, vOut);
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool bIsColl)
{
    DX::Draw(pBatch, *m_pDesc, bIsColl == true ? ::XMVectorSet(1.f, 0.f, 0.f, 1.f) : ::XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pInitializeDesc)
{
    CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize(pInitializeDesc)))
    {
        MSG_BOX("CBounding_AABB::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBounding_AABB::Free()
{
    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
    Super::Free();
}
