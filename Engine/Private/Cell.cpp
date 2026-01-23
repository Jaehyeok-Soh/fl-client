#include "Cell.h"

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::Initialize(const Vec3* pPoints, _int iIndex)
{
	::memcpy(m_vPoints, pPoints, sizeof(Vec3) * ENUM_TO_UINT(EPOINT::END));

	m_iIndex = iIndex;

	Vec3 vAB = (m_vPoints[ENUM_TO_UINT(EPOINT::B)] - m_vPoints[ENUM_TO_UINT(EPOINT::A)]);
	Vec3 vAC = (m_vPoints[ENUM_TO_UINT(EPOINT::C)] - m_vPoints[ENUM_TO_UINT(EPOINT::A)]);
	vAB.Normalize();
	vAC.Normalize();

	Vec3 vUp = vAB.Cross(vAC);
	vUp.Normalize();
	m_vUpNormal = vUp;
	return S_OK;
}

Vec3 CCell::Project_OnPlane(const Vec3 &vPos) const
{
	Vec3 vNormal = Get_UpNormal();
	Vec3 vA_Pos = vPos - Get_Point(EPOINT::A);
	_float fDist = vA_Pos.Dot(vNormal);
	return vPos - vNormal * fDist;
}

Vec3 CCell::Get_CenterPos()
{
	Vec3 vPosition = {};
	for (size_t i = 0; i < 3; i++)
		vPosition += m_vPoints[i];

	return vPosition / 3.f;
}

_bool CCell::Is_In(Vec3 vResultPos, _int* pNeighborIndex)
{
	for (size_t i = 0; i < ENUM_TO_UINT(ELINE::END); ++i)
	{
		Vec3 vDir = vResultPos - m_vPoints[i];
		_float fDot = vDir.Dot(m_vNormals[i]);
		if (g_XMEpsilon.f[0] < fDot)
		{
			*pNeighborIndex = m_iNeighbors[i];
			return false;
		}
	}

	return true;
}

_bool CCell::Compare(Vec3 vSourPoint, Vec3 vDestPoint)
{
	if (::XMVector3Equal(vSourPoint, m_vPoints[ENUM_TO_UINT(EPOINT::A)]) == true)
	{
		if (::XMVector3Equal(vDestPoint, m_vPoints[ENUM_TO_UINT(EPOINT::B)]) == true)
			return true;
		if (::XMVector3Equal(vDestPoint, m_vPoints[ENUM_TO_UINT(EPOINT::C)]) == true)
			return true;
	}

	if (::XMVector3Equal(vSourPoint, m_vPoints[ENUM_TO_UINT(EPOINT::B)]) == true)
	{
		if (::XMVector3Equal(vDestPoint, m_vPoints[ENUM_TO_UINT(EPOINT::C)]) == true)
			return true;
		if (::XMVector3Equal(vDestPoint, m_vPoints[ENUM_TO_UINT(EPOINT::A)]) == true)
			return true;
	}

	if (::XMVector3Equal(vSourPoint, m_vPoints[ENUM_TO_UINT(EPOINT::C)]) == true)
	{
		if (::XMVector3Equal(vDestPoint, m_vPoints[ENUM_TO_UINT(EPOINT::A)]) == true)
			return true;
		if (::XMVector3Equal(vDestPoint, m_vPoints[ENUM_TO_UINT(EPOINT::B)]) == true)
			return true;
	}

	return false;
}

_float CCell::Compute_Height(const Vec3& vCellPos)
{
	Vec4 vPlane = ::XMPlaneFromPoints(
		::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ENUM_TO_UINT(EPOINT::A))]),
		::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ENUM_TO_UINT(EPOINT::B))]),
		::XMLoadFloat3(&m_vPoints[ENUM_TO_UINT(ENUM_TO_UINT(EPOINT::C))]));

	// ax + by + cz + d  = 0
	// y = (-ax - cz - d) / b
	_float x = vCellPos.x;
	_float y = vCellPos.y;
	_float z = vCellPos.z;

	_float a = vPlane.x;
	_float b = vPlane.y;
	_float c = vPlane.z;
	_float d = vPlane.w;

	return ((-a * x) - (c * z) - d) / b;
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const Vec3* pPoints, _int iIndex)
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
