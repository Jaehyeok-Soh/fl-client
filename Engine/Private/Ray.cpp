#include "Ray.h"

CRay::CRay(const Vec3& vLocalOrigin, const Vec3& vLocalDir)
	: m_vLocalOrigin(vLocalOrigin)
	, m_vLocalDir(vLocalDir)
{
	m_vLocalDir.Normalize();
}

HRESULT CRay::Initialize()
{
	return S_OK;
}

void CRay::Setup_Ray(const Matrix* pMatParent)
{
	m_vOrigin = Vec3::Transform(m_vLocalOrigin, *pMatParent);
	m_vDir = Vec3::TransformNormal(m_vLocalDir, *pMatParent);
}

_bool CRay::IntersectrayWithAABB_World(BoundingBox* pDesc, OUT Vec3& vOut)
{
	::XMStoreFloat4(&m_vOrigin, ::XMVectorSetW(vOrigin, 1.f));
	::XMStoreFloat3(&m_vDir, vDir);
}

_bool CRay::IntersectrayWithAABB_World(BoundingBox* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(m_vOrigin, m_vDir, fDist))
	{
		vOut = m_vOrigin + m_vDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(m_vLocalOrigin, m_vLocalDir, fDist))
	{
		vOut = m_vLocalOrigin + m_vLocalDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(m_vOrigin, m_vDir, fDist))
	{
		vOut = m_vOrigin + m_vDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(m_vLocalOrigin, m_vLocalDir, fDist))
	{
		vOut = m_vLocalOrigin + m_vLocalDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(m_vOrigin, m_vDir, fDist))
	{
		vOut = m_vOrigin + m_vDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT Vec3& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(m_vLocalOrigin, m_vLocalDir, fDist))
	{
		vOut = m_vLocalOrigin + m_vLocalDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithTriangle_World(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(m_vOrigin, m_vDir, vPointA, vPointB, vPointC, fDist))
	{
		vOut = m_vOrigin + m_vDir * fDist;
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithTriangle_Local(const Vec3& vPointA, const Vec3& vPointB, const Vec3& vPointC, OUT Vec3& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(m_vLocalOrigin, m_vLocalDir, vPointA, vPointB, vPointC, fDist))
	{
		vOut = m_vLocalOrigin + m_vLocalDir * fDist;
		return true;
	}
	return false;
}

CRay* CRay::Create(const Vec3& vLocalOrigin, const Vec3& vLocalDir)
{
	CRay* pInstance = new CRay(vLocalOrigin, vLocalDir);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CRay::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRay::Free()
{
	Super::Free();
}
