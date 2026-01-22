#include "Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
	::memcpy(m_vPoints, pPoints, sizeof(_float3) * ENUM_TO_UINT(EPOINT::END));

	m_iIndex = iIndex;

	_vector vPointA = ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::A)]);
	_vector vPointB = ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::B)]);
	_vector vPointC = ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::C)]);

	_vector vAB = ::XMVector3Normalize(vPointB - vPointA);
	_vector vAC = ::XMVector3Normalize(vPointC - vPointA);

	_vector vUp = ::XMVector3Normalize(::XMVector3Cross(vAB, vAC));

	::XMStoreFloat3(&m_vUpNormal, vUp);
	return S_OK;
}

_vector CCell::Project_OnPlane(_fvector vPos) const
{
	_vector vA = Get_Point(EPOINT::A);
	_vector vB = Get_Point(EPOINT::B);
	_vector vC = Get_Point(EPOINT::C);

	_vector vNormal = Get_UpNormal();

	_vector vA_Pos = vPos - vA;
	_float fDist = ::XMVectorGetX(::XMVector3Dot(vA_Pos, vNormal));
	return vPos - vNormal * fDist;
}

_vector CCell::Get_CenterPos()
{
	_vector		vPosition = {};

	for (size_t i = 0; i < 3; i++)
	{
		vPosition += ::XMLoadFloat3(&m_vPoints[i]);
	}
	return vPosition / 3.f;
}

_bool CCell::Is_In(_fvector vResultPos, _int* pNeighborIndex)
{
	for (size_t i = 0; i < ENUM_TO_UINT(ELINE::END); ++i)
	{
		_vector vDir = vResultPos - ::XMLoadFloat3(&m_vPoints[i]);
		_vector vDist450 = ::XMVector3Dot(::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::A)]) - vResultPos, ::XMLoadFloat3(&m_vNormals[ENUM_TO_UINT(ELINE::AB)]));
		if (g_XMEpsilon.f[0] < ::XMVectorGetX(::XMVector3Dot(vDir, ::XMLoadFloat3(&m_vNormals[i]))))
		{
			*pNeighborIndex = m_iNeighbors[i];
			return false;
		}
	}

	return true;
}

_bool CCell::Compare(_fvector vSourPoint, _fvector vDestPoint)
{
	if (::XMVector3Equal(vSourPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::A)])) == true)
	{
		if (::XMVector3Equal(vDestPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::B)])) == true)
			return true;
		if (::XMVector3Equal(vDestPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::C)])) == true)
			return true;
	}

	if (::XMVector3Equal(vSourPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::B)])) == true)
	{
		if (::XMVector3Equal(vDestPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::C)])) == true)
			return true;
		if (::XMVector3Equal(vDestPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::A)])) == true)
			return true;
	}

	if (::XMVector3Equal(vSourPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::C)])) == true)
	{
		if (::XMVector3Equal(vDestPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::A)])) == true)
			return true;
		if (::XMVector3Equal(vDestPoint, ::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(EPOINT::B)])) == true)
			return true;
	}

	return false;
}

_float CCell::Compute_Height(_vector vCellPos)
{
	_vector vPlane = ::XMPlaneFromPoints(
		::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ENUM_TO_UINT(EPOINT::A))]),
		::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ENUM_TO_UINT(EPOINT::B))]),
		::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ENUM_TO_UINT(EPOINT::C))]));

	// ax + by + cz + d  = 0
	// y = (-ax - cz - d) / b
	_float x = ::XMVectorGetX(vCellPos);
	_float y = ::XMVectorGetY(vCellPos);
	_float z = ::XMVectorGetZ(vCellPos);

	_float a = ::XMVectorGetX(vPlane);
	_float b = ::XMVectorGetY(vPlane);
	_float c = ::XMVectorGetZ(vPlane);
	_float d = ::XMVectorGetW(vPlane);

	return ((-a * x) - (c * z) - d) / b;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _float3* pPoints, _int iIndex)
{
	CCell* pInstance = new CCell(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize(pPoints, iIndex)))
	{
		MSG_BOX("CCell::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
