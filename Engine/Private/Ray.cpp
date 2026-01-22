#include "Ray.h"

CRay::CRay(const _float4& vLocalOrigin, const _float3& vLocalDir)
	: m_vLocalOrigin(vLocalOrigin)
	, m_vLocalDir(vLocalDir)
{
	::XMStoreFloat3(&m_vLocalDir, ::XMVector3Normalize(::XMLoadFloat3(&m_vLocalDir)));
}

HRESULT CRay::Initialize()
{
	return S_OK;
}

void CRay::Setup_Ray(const _float4x4* pMatParent)
{
	::XMStoreFloat4(&m_vOrigin, ::XMVector3TransformCoord(::XMVectorSetW(::XMLoadFloat4(&m_vLocalOrigin), 1.f), ::XMLoadFloat4x4(pMatParent)));
	::XMStoreFloat3(&m_vDir, ::XMVector3TransformNormal(::XMLoadFloat3(&m_vLocalDir), ::XMLoadFloat4x4(pMatParent)));
}

void CRay::Setup_Ray(_fvector vOrigin, _fvector vDir)
{
	::XMStoreFloat4(&m_vOrigin, ::XMVectorSetW(vOrigin, 1.f));
	::XMStoreFloat3(&m_vDir, vDir);
}

_bool CRay::IntersectrayWithAABB_World(BoundingBox* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(::XMLoadFloat4(&m_vOrigin), ::XMLoadFloat3(&m_vDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vOrigin) + ::XMLoadFloat3(&m_vDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithAABB_Local(BoundingBox* pOriginDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(::XMLoadFloat4(&m_vLocalOrigin), ::XMLoadFloat3(&m_vLocalDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vLocalOrigin) + ::XMLoadFloat3(&m_vLocalDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithOBB_World(BoundingOrientedBox* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(::XMLoadFloat4(&m_vOrigin), ::XMLoadFloat3(&m_vDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vOrigin) + ::XMLoadFloat3(&m_vDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithOBB_Local(BoundingOrientedBox* pOriginDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(::XMLoadFloat4(&m_vLocalOrigin), ::XMLoadFloat3(&m_vLocalDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vLocalOrigin) + ::XMLoadFloat3(&m_vLocalDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithSphere_World(BoundingSphere* pDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pDesc->Intersects(::XMLoadFloat4(&m_vOrigin), ::XMLoadFloat3(&m_vDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vOrigin) + ::XMLoadFloat3(&m_vDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithSphere_Local(BoundingSphere* pOriginDesc, OUT _float4& vOut)
{
	_float fDist = {};
	if (pOriginDesc->Intersects(::XMLoadFloat4(&m_vLocalOrigin), ::XMLoadFloat3(&m_vLocalDir), fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vLocalOrigin) + ::XMLoadFloat3(&m_vLocalDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithTriangle_World(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(::XMLoadFloat4(&m_vOrigin), ::XMLoadFloat3(&m_vDir),
		::XMLoadFloat3(&vPointA), ::XMLoadFloat3(&vPointB), ::XMLoadFloat3(&vPointC),
		fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vOrigin) + ::XMLoadFloat3(&m_vDir) * fDist);
		return true;
	}
	return false;
}

_bool CRay::IntersectrayWithTriangle_Local(const _float3& vPointA, const _float3& vPointB, const _float3& vPointC, OUT _float4& vOut)
{
	_float fDist = {};
	if (true == TriangleTests::Intersects(::XMLoadFloat4(&m_vLocalOrigin), ::XMLoadFloat3(&m_vLocalDir),
		::XMLoadFloat3(&vPointA), ::XMLoadFloat3(&vPointB), ::XMLoadFloat3(&vPointC),
		fDist))
	{
		::XMStoreFloat4(&vOut, ::XMLoadFloat4(&m_vLocalOrigin) + ::XMLoadFloat3(&m_vLocalDir) * fDist);
		return true;
	}
	return false;
}

CRay* CRay::Create(const _float4& vLocalOrigin, const _float3& vLocalDir)
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
