#include "Bounding_OBB.h"
#include "DebugDraw.h"
#include "Ray.h"
#include "Bounding_Sphere.h"
#include "Bounding_AABB.h"
#include "GameInstance.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

HRESULT CBounding_OBB::Initialize(const BOUNDING_DESC* pInitialDesc)
{
    const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(pInitialDesc);

    _float4     vRotation = {};
    ::XMStoreFloat4(&vRotation,
        ::XMQuaternionRotationRollPitchYaw(
            ::XMConvertToRadians(pDesc->vAngles.x),
            ::XMConvertToRadians(pDesc->vAngles.y),
            ::XMConvertToRadians(pDesc->vAngles.z)));

    m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vRotation);
    m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

    return S_OK;
}

void CBounding_OBB::Update(_fmatrix WorldMatrix)
{
    m_pOriginalDesc->Transform(*m_pDesc, WorldMatrix);
}

_bool CBounding_OBB::Intersect_Bounding(EColliderType eType, CBounding* pOther)
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

_bool CBounding_OBB::IntersectWithRay_World(CGameInstance* pGameInstance, OUT _float4& vOut)
{
    return pGameInstance->IntersectrayWithOBB_World(m_pDesc, vOut);
}

_bool CBounding_OBB::IntersectWithRay_Local(CGameInstance* pGameInstance, OUT _float4& vOut)
{
    return pGameInstance->IntersectrayWithOBB_Local(m_pOriginalDesc, vOut);
}

_bool CBounding_OBB::IntersectWithRay_World(CRay* pRay, OUT _float4& vOut)
{
    return pRay->IntersectrayWithOBB_World(m_pDesc, vOut);
}

_bool CBounding_OBB::IntersectWithRay_Local(CRay* pRay, OUT _float4& vOut)
{
    return pRay->IntersectrayWithOBB_Local(m_pOriginalDesc, vOut);
}

#ifdef _DEBUG
HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
    DX::Draw(pBatch, *m_pDesc, true == isColl ? ::XMVectorSet(1.f, 0.f, 0.f, 1.f) : ::XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const BOUNDING_DESC* pDesc)
{
    CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize(pDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_OBB");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBounding_OBB::Free()
{
    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
    Super::Free();
}
