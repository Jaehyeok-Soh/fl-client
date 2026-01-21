#include "FileUtils.h"
#include "GameInstance.h"
#include "Cell.h"
#include "Navigation.h"

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CNavigation::CNavigation(const CNavigation& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_vecCells(rhs.m_vecCells)
	, m_iCellCount(rhs.m_iCellCount)
	, m_iCurrentCellIndex(rhs.m_iCurrentCellIndex)
	, m_pParentMatrix(rhs.m_pParentMatrix)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	for (auto& pCell : m_vecCells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::Initialize_Prototype(const POLYGON_SAVEDATA& polygonData)
{
	if (FAILED(SetUp_Cells(polygonData)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (pArg)
	{
		NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);
		if (pDesc->pParentMatrix)
			m_pParentMatrix = pDesc->pParentMatrix;
		if (pDesc->iCurrentIndex != -1)
			m_iCurrentCellIndex = pDesc->iCurrentIndex;
		else
		{
			_float3 vSrc = pDesc->vPosition;
			Sync_Index(::XMVectorSetW(::XMLoadFloat3(&vSrc), 1.f));
		}
	}
	else
		return E_FAIL;

	return S_OK;
}

_vector CNavigation::Get_CellPos()
{
	if (m_pParentMatrix)
		return ::XMVector3TransformCoord(::XMVectorSetW(m_vecCells[m_iCurrentCellIndex]->Get_CenterPos(), 1.f), ::XMLoadFloat4x4(m_pParentMatrix));
	else
		return ::XMVectorSetW(m_vecCells[m_iCurrentCellIndex]->Get_CenterPos(), 1.f);
}

_vector CNavigation::Get_CellNormal()
{
	return m_vecCells[m_iCurrentCellIndex]->Get_UpNormal();
}

_vector CNavigation::Get_CellNormal_World()
{
	_vector vNormalLocal = m_vecCells[m_iCurrentCellIndex]->Get_UpNormal();

	if (m_pParentMatrix)
	{
		_matrix matParent = ::XMLoadFloat4x4(m_pParentMatrix);
		vNormalLocal = ::XMVector3TransformNormal(vNormalLocal, matParent);
		vNormalLocal = ::XMVector3Normalize(vNormalLocal);
	}
	
	return vNormalLocal;
}

_vector CNavigation::SetUp_OnNavigation(_fvector vWorldPos)
{
	if (m_pParentMatrix)
	{
		_vector vCellPos = XMVector3TransformCoord(vWorldPos, XMMatrixInverse(nullptr, ::XMLoadFloat4x4(m_pParentMatrix)));
		vCellPos = XMVectorSetY(vCellPos, m_vecCells[m_iCurrentCellIndex]->Compute_Height(vCellPos));
		return XMVector3TransformCoord(vCellPos, ::XMLoadFloat4x4(m_pParentMatrix));
	}
	else
		return XMVectorSetY(vWorldPos, m_vecCells[m_iCurrentCellIndex]->Compute_Height(vWorldPos));
}

_vector CNavigation::Project_OnNavigation(_fvector vWorldPos)
{
	if (m_iCurrentCellIndex < 0)
		return vWorldPos;

	_vector vLocalPos = vWorldPos;

	if (m_pParentMatrix)
	{
		_matrix matInvParent = ::XMMatrixInverse(nullptr, ::XMLoadFloat4x4(m_pParentMatrix));
		vLocalPos = ::XMVector3TransformCoord(vLocalPos, matInvParent);
	}

	CCell* pCell = m_vecCells[m_iCurrentCellIndex];
	_vector vProjectedLocal = pCell->Project_OnPlane(vLocalPos);

	if (m_pParentMatrix)
	{
		_matrix matParent = ::XMLoadFloat4x4(m_pParentMatrix);
		return ::XMVector3TransformCoord(vProjectedLocal, matParent);
	}

	return vProjectedLocal;
}

void CNavigation::Sync_Index(_fvector vWorldPos)
{
	_vector vCellPos = ::XMVectorSetW(vWorldPos, 1.f);

	if(m_pParentMatrix)
		vCellPos = ::XMVector3TransformCoord(vCellPos, ::XMMatrixInverse(nullptr, ::XMLoadFloat4x4(m_pParentMatrix)));

	m_iCurrentCellIndex = -1;
	_float fBestDist = FLT_MAX;
	for (size_t i = 0; i < m_vecCells.size(); ++i)
	{
		CCell* pCell = m_vecCells[i];

		_vector vA = pCell->Get_Point(EPOINT::A);
		_vector vUpNormal = pCell->Get_UpNormal();

		_float fDist = ::XMVectorGetX(::XMVector3Dot(vCellPos - vA, vUpNormal));

		_vector vProj = vCellPos - vUpNormal * fDist;

		_int iSrc = -1;
		if (pCell->Is_In(vProj, &iSrc) == false)
			continue;

		_float fAbsDist = ::fabsf(fDist);
		if (fAbsDist < fBestDist)
		{
			fBestDist = fAbsDist;
			m_iCurrentCellIndex = pCell->Get_Index();
		}
	}

#pragma region Legacy
	//for (size_t i = 0; i < m_vecCells.size(); ++i)
	//{
	//	_vector vNormal = m_vecCells[i]->Get_UpNormal();
	//	_float fDist = { 0.f };
	//	_bool bHit = { false };
	//	if (true == TriangleTests::Intersects(
	//		vCellPos,
	//		vNormal,
	//		m_vecCells[i]->Get_Point(EPOINT::A),
	//		m_vecCells[i]->Get_Point(EPOINT::B),
	//		m_vecCells[i]->Get_Point(EPOINT::C),
	//		fDist) ||
	//		true == TriangleTests::Intersects(
	//		vCellPos,
	//		vNormal * -1.f,
	//		m_vecCells[i]->Get_Point(EPOINT::A),
	//		m_vecCells[i]->Get_Point(EPOINT::B),
	//		m_vecCells[i]->Get_Point(EPOINT::C),
	//		fDist))
	//	{
	//		bHit = true;
	//	}

	//	if (bHit == false)
	//		continue;

	//	if (fDist < fBestDist)
	//	{
	//		fBestDist = fDist;
	//		m_iCurrentCellIndex = static_cast<_int>(i);
	//	}
	//}
#pragma endregion 
}

_bool CNavigation::Is_Move(_fvector vResultPos)
{
	if (-1 == m_iCurrentCellIndex)
		return false;

	_vector     vCellPos = vResultPos;
	if (m_pParentMatrix)
		vCellPos = XMVector3TransformCoord(vResultPos, XMMatrixInverse(nullptr, ::XMLoadFloat4x4(m_pParentMatrix)));

	_int        iNeighborIndex = { -1 };
	if (m_vecCells[m_iCurrentCellIndex]->Is_In(vCellPos, &iNeighborIndex))
		return true;

	if (iNeighborIndex == -1)
		return false;

	// 무한루프 방지
	const _int iMaxCounting = static_cast<_int>(m_vecCells.size()) + 100;
	_int iCount = { 0 };

	_int iCurrentIndex = iNeighborIndex;
	while (iCount++ < iMaxCounting)
	{
		if (true == m_vecCells[iCurrentIndex]->Is_In(vCellPos, &iNeighborIndex))
		{
			m_iCurrentCellIndex = iCurrentIndex;
			return true;
		}

		if (iNeighborIndex == -1)
			return false;

		iCurrentIndex = iNeighborIndex;
	}

	return false;
}

HRESULT CNavigation::SetUp_Cells(const POLYGON_SAVEDATA& polygonData)
{
	size_t iCellCount = polygonData.vecCells.size();
	m_vecCells.reserve(iCellCount);
	for (size_t i = 0; i < iCellCount; ++i)
	{
		const CELL_SAVEDATA& cellData = polygonData.vecCells[i];
		CCell *pCell = CCell::Create(m_pDevice, m_pDeviceContext,  cellData.arrPoints.data(), cellData.iIndex);
		if (!pCell)
			return E_FAIL;

		pCell->Set_Neighbor(ELINE::AB, cellData.arrNeighbors[ENUM_TO_UINT(ELINE::AB)]);
		pCell->Set_Neighbor(ELINE::BC, cellData.arrNeighbors[ENUM_TO_UINT(ELINE::BC)]);
		pCell->Set_Neighbor(ELINE::CA, cellData.arrNeighbors[ENUM_TO_UINT(ELINE::CA)]);

		pCell->Set_Normal(ELINE::AB, cellData.arrNormals[ENUM_TO_UINT(ELINE::AB)]);
		pCell->Set_Normal(ELINE::BC, cellData.arrNormals[ENUM_TO_UINT(ELINE::BC)]);
		pCell->Set_Normal(ELINE::CA, cellData.arrNormals[ENUM_TO_UINT(ELINE::CA)]);
		m_vecCells.push_back(pCell);
	}

	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const POLYGON_SAVEDATA& polygonData)
{
	CNavigation* pInstance = new CNavigation(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype(polygonData)))
	{
		MSG_BOX("CNavigation::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
	CNavigation* pClone = new CNavigation(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CNavigation::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CNavigation::Free()
{
	for (auto& pCell : m_vecCells)
		Safe_Release(pCell);
	m_vecCells.clear();
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Super::Free();
}
