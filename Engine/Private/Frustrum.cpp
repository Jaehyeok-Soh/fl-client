#include "Engine_pch.h"
#include "Frustrum.h"
#include "Collider.h"
#include "Bounding_AABB.h"
#include "CameraMan.h"
#include "GameInstance.h"

CFrustrum::CFrustrum()
    : m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CFrustrum::Initialize(const _float fMidStart, const _float fFarStart)
{
    m_fMidStart = fMidStart;
    m_fFarStart = fFarStart;
    Update_WhenMainCameraProjUpdate(m_pGameInstance->Get_ProjMatrix());

    return S_OK;
}

EFrustrumTier CFrustrum::Classify_BySplitFrustrum(const BoundingBox &AABB)
{
    if (m_nearBounding.World.Contains(AABB) != ContainmentType::DISJOINT) return EFrustrumTier::Near;
    if (m_midBounding.World.Contains(AABB) != ContainmentType::DISJOINT) return EFrustrumTier::Mid;
    if (m_farBounding.World.Contains(AABB) != ContainmentType::DISJOINT) return EFrustrumTier::Far;
    return EFrustrumTier::None;
}

EFrustrumTier CFrustrum::Classify_BySplitFrustrum(const BoundingSphere& Sphere)
{
    if (m_nearBounding.World.Contains(Sphere) != ContainmentType::DISJOINT) return EFrustrumTier::Near;
    if (m_midBounding.World.Contains(Sphere) != ContainmentType::DISJOINT) return EFrustrumTier::Mid;
    if (m_farBounding.World.Contains(Sphere) != ContainmentType::DISJOINT) return EFrustrumTier::Far;
    return EFrustrumTier::None;
}

void CFrustrum::Update()
{
    if (m_pGameInstance->Get_MainCamera())
    {
        const Matrix& matCameraWorld = m_pGameInstance->Get_MainCamera()->Get_Component<CTransform>()->Get_WorldMatrix();
        m_Bounds.Origin.Transform(m_Bounds.World, matCameraWorld);
        m_nearBounding.Origin.Transform(m_nearBounding.World, matCameraWorld);
        m_midBounding.Origin.Transform(m_midBounding.World, matCameraWorld);
        m_farBounding.Origin.Transform(m_farBounding.World, matCameraWorld);
    }
}

void CFrustrum::Update_WhenMainCameraProjUpdate(const Matrix& matProj)
{
    BoundingFrustum::CreateFromMatrix(m_Bounds.Origin, matProj);
    m_nearBounding.Origin = BoundingFrustum(m_Bounds.Origin);
    m_midBounding.Origin = BoundingFrustum(m_Bounds.Origin);
    m_farBounding.Origin = BoundingFrustum(m_Bounds.Origin);
    Set_SplitFrustrum(m_fMidStart, m_fFarStart);
    Update();
}

void CFrustrum::Resize_SplitFrustrum(const _float fMidStart, const _float fFarStart)
{
    Set_SplitFrustrum(fMidStart, fFarStart);
    Update();
}

void CFrustrum::Set_SplitFrustrum(const _float fMidStart, const _float fFarStart)
{
    m_fMidStart = fMidStart;
    m_fFarStart = fFarStart;
    const _float fNearZ = m_Bounds.Origin.Near;
    const _float fFarZ = m_Bounds.Origin.Far;
    const _float fMidZ = std::clamp(fMidStart, fNearZ + 0.01f, fFarZ - 0.02f);
    const _float fFarSplitZ = std::clamp(fFarStart, fMidZ + 0.01f, fFarZ - 0.02f);

    m_nearBounding.Origin.Near = fNearZ;
    m_nearBounding.Origin.Far = fMidZ;

    m_midBounding.Origin.Near = fMidZ;
    m_midBounding.Origin.Far = fFarSplitZ;

    m_farBounding.Origin.Near = fFarSplitZ;
    m_farBounding.Origin.Far = fFarZ;
}

CFrustrum* CFrustrum::Create()
{
    return new CFrustrum();
}

void CFrustrum::Free()
{
    Super::Free();
}
