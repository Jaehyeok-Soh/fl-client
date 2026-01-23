#include "Frustrum.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "CameraMan.h"
#include "GameInstance.h"

CFrustrum::CFrustrum()
    : m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CFrustrum::Initialize()
{
    Matrix matProj = m_pGameInstance->Get_ProjMatrix();
    m_pOriginBounding = new BoundingFrustum;
    m_pWorldBounding = new BoundingFrustum;

    BoundingFrustum::CreateFromMatrix(*m_pOriginBounding, matProj);
    return S_OK;
}

_bool CFrustrum::Culling(CCollider* pCollider)
{
    if (m_pWorldBounding)
    {
        BoundingBox* pBounding = static_cast<CBounding_AABB*>(pCollider->Get_Bounding())->Get_Desc();
        return m_pWorldBounding->Intersects(*pBounding);
    }

    return false;
}

void CFrustrum::Update()
{
    if (m_pWorldBounding)
    {
        m_pOriginBounding->Transform(*m_pWorldBounding, m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_WorldMatrix());
    }
}

void CFrustrum::Clear()
{
    Safe_Delete(m_pOriginBounding);
    m_pOriginBounding = nullptr;
    Safe_Delete(m_pWorldBounding);
    m_pWorldBounding = nullptr;
}

CFrustrum* CFrustrum::Create()
{
    return new CFrustrum();
}

void CFrustrum::Free()
{
    Clear();
    Super::Free();
}
