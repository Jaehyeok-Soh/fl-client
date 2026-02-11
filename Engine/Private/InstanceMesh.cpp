#include "Engine_pch.h"
#include "InstanceMesh.h"
#include "GameInstance.h"

CInstanceMesh::CInstanceMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext), m_pInstanceWorldMinMax{ nullptr } 
{
}

CInstanceMesh::CInstanceMesh(const CInstanceMesh& rhs)
	: CVIBuffer(rhs), m_pInstanceWorldMinMax(rhs.m_pInstanceWorldMinMax)
{
}


HRESULT CInstanceMesh::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInstanceMesh::Initialize(void* pArg)
{
	CInstanceMesh::INSTANCEMESH_DESC* pDesc = static_cast<CInstanceMesh::tagInstanceMesh_Desc*>(pArg);

	if (pDesc->vecInstanceMatrixPointer == nullptr) return E_FAIL;

	CVIBuffer::VIBUFFER_ORIGIN_DESC tVIBufferDesc{};
	tVIBufferDesc.VB_Usage = pDesc->VB_Usage;
	tVIBufferDesc.IB_Usage = pDesc->IB_Usage;

	/* Usage Setting */
	if (FAILED(Super::Initialize_Prototype(&tVIBufferDesc)))
		return E_FAIL;

	m_iVertexBufferCount = 1;
	m_iVertexStride = sizeof(VTX_INSTANCE);
	m_iVertexCount = 1;
	m_iInstanceCount = ENUM_TO_UINT(pDesc->vecInstanceMatrixPointer->size());

	m_tInstanceVertexBufferDesc.ByteWidth = sizeof(VTX_INSTANCE) * static_cast<_uint>(pDesc->vecInstanceMatrixPointer->size());
	m_tInstanceVertexBufferDesc.StructureByteStride = m_iVertexStride;
	m_tInstanceVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tInstanceVertexBufferDesc.Usage = m_VB_Usage;
	m_tInstanceVertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
	m_tInstanceVertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
	InstanceInitialData.pSysMem = pDesc->vecInstanceMatrixPointer->data();

	if (FAILED(m_pDevice->CreateBuffer(&m_tInstanceVertexBufferDesc, &InstanceInitialData, &m_pVB)))
		return E_FAIL;


	if (FAILED(Update_Instance_WorldMinMax(pDesc->pModelMinMax, pDesc->vecInstanceMatrixPointer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CInstanceMesh::Update_Instance_WorldMinMax(const Vec3* pModelMinMax,const vector<Matrix>* vecInstanceMatrixPointer)
{
	if (pModelMinMax == nullptr) return S_OK;
	if (vecInstanceMatrixPointer == nullptr) return E_FAIL;

	if(!m_pInstanceWorldMinMax)
		m_pInstanceWorldMinMax = new Vec3[2]{ Vec3(FLT_MAX,FLT_MAX,FLT_MAX) , Vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX)};

	const Vec3& vLocalMin = pModelMinMax[MIN];
	const Vec3& vLocalMax = pModelMinMax[MAX];

	BoundingBox localBox = Engine_Utils::MakeAABB_FromMinMax(vLocalMin, vLocalMax);
	BoundingBox groupBox;

	// 인스턴스 모델중 첫놈으로 기준 잡기
	localBox.Transform(groupBox , (*vecInstanceMatrixPointer)[0]);

	Vec3& vOutMin = m_pInstanceWorldMinMax[MIN];
	Vec3& vOutMax = m_pInstanceWorldMinMax[MAX];

	vOutMin = groupBox.Center - groupBox.Extents;
	vOutMax = groupBox.Center + groupBox.Extents;

	for (size_t i = 1; i < vecInstanceMatrixPointer->size(); ++i)
	{
		BoundingBox wBox;
		localBox.Transform(wBox , (*vecInstanceMatrixPointer)[i]);

		Vec3 vMinMax[2] =
		{
			wBox.Center - wBox.Extents,
			wBox.Center + wBox.Extents
		};

		Engine_Utils::Merge_MinMax(vMinMax, vOutMin, vOutMax);
	}

	m_pInstanceWorldMinMax[MIN] = vOutMin;
	m_pInstanceWorldMinMax[MAX] = vOutMax;

	return S_OK;
}

HRESULT CInstanceMesh::Bind_Instance(_uint iSlotNum)
{
	/* Bind_Resource */
	ID3D11Buffer* pVertexBuffers[] =
	{
		   m_pVB,
	};

	_uint		 iVertexStrides[] =
	{
		m_iVertexStride,
	};

	_uint		 iOffsets[] =
	{
		0
	};

	m_pDeviceContext->IASetVertexBuffers(iSlotNum, m_iVertexBufferCount, pVertexBuffers, iVertexStrides, iOffsets);

	return S_OK;
}

HRESULT CInstanceMesh::Bind_Resource()
{
	/* Bind_Resource */
	ID3D11Buffer* pVertexBuffers[] =
	{
		   m_pVB,
	};

	_uint		 iVertexStrides[] =
	{
		m_iVertexStride,
	};

	_uint		 iOffsets[] =
	{
		0
	};

	m_pDeviceContext->IASetVertexBuffers(1 , m_iVertexBufferCount , pVertexBuffers, iVertexStrides, iOffsets);

	return S_OK;
}

void CInstanceMesh::Render()
{
	return;
}

void CInstanceMesh::Unbind_Resource(_uint iSlotNum)
{
	ID3D11Buffer* pInstanceBuffer[] =
	{
		nullptr
	};
	_uint		  iVertexStrides[] = {
		m_iVertexStride
	};
	_uint		 iOffsets[] =
	{
		0
	};

	m_pDeviceContext->IASetVertexBuffers(iSlotNum , m_iVertexBufferCount , pInstanceBuffer , iVertexStrides , iOffsets);
}

HRESULT CInstanceMesh::ReMake_InstanceBuffer(vector<Matrix>* vecInstanceMatrixPointer)
{
	if (vecInstanceMatrixPointer == nullptr) return E_FAIL;

	Safe_Release(m_pVB);

	CVIBuffer::VIBUFFER_ORIGIN_DESC tVIBufferDesc{};
	tVIBufferDesc.VB_Usage = this->m_VB_Usage;
	tVIBufferDesc.IB_Usage = this->m_IB_Usage;

	/* Usage Setting */
	if (FAILED(Super::Initialize_Prototype(&tVIBufferDesc)))
		return E_FAIL;

	m_iVertexBufferCount = 1;
	m_iVertexStride = sizeof(VTX_INSTANCE);
	m_iVertexCount = 1;
	m_iInstanceCount = ENUM_TO_UINT(vecInstanceMatrixPointer->size());

	m_tInstanceVertexBufferDesc.ByteWidth = sizeof(VTX_INSTANCE) * m_iInstanceCount;
	m_tInstanceVertexBufferDesc.StructureByteStride = m_iVertexStride;
	m_tInstanceVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tInstanceVertexBufferDesc.Usage = m_VB_Usage;
	m_tInstanceVertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
	m_tInstanceVertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
	InstanceInitialData.pSysMem = vecInstanceMatrixPointer->data();

	if (FAILED(m_pDevice->CreateBuffer(&m_tInstanceVertexBufferDesc, &InstanceInitialData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

void CInstanceMesh::Update_Matrix(const Matrix& WorldMatrix, _uint iIndex)
{
	if (m_tInstanceVertexBufferDesc.Usage != D3D11_USAGE_DYNAMIC)
		return;
	D3D11_MAPPED_SUBRESOURCE pResource{ nullptr };

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &pResource);

	VTX_INSTANCE* pVtxMatrix = reinterpret_cast<VTX_INSTANCE*>(pResource.pData);

	::memcpy(&pVtxMatrix[iIndex].vRight.x , &WorldMatrix._11 , sizeof(VTX_INSTANCE) );

	m_pDeviceContext->Unmap(m_pVB, 0);
}

void CInstanceMesh::Update_Matrix(const vector<Matrix>& vecWorldMatrix)
{
	if (m_tInstanceVertexBufferDesc.Usage != D3D11_USAGE_DYNAMIC)
		return;

	D3D11_MAPPED_SUBRESOURCE pResource{ nullptr };

	m_iVisibleInstanceCount = (std::min)((_uint)vecWorldMatrix.size(), m_iInstanceCount);

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &pResource);

	VTX_INSTANCE* pVtxMatrix = reinterpret_cast<VTX_INSTANCE*>(pResource.pData);

	::memcpy(pVtxMatrix, vecWorldMatrix.data(), sizeof(VTX_INSTANCE) * m_iVisibleInstanceCount);

	m_pDeviceContext->Unmap(m_pVB, 0);
}



CInstanceMesh* CInstanceMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CInstanceMesh* pInstanceMesh = new CInstanceMesh(pDevice, pDeviceContext);

	if (FAILED(pInstanceMesh->Initialize_Prototype()))
	{
		Safe_Release(pInstanceMesh);
		MSG_BOX("Instance Mesh is failed to Create");
		return nullptr;
	}
	return pInstanceMesh;
}

CComponent* CInstanceMesh::Clone(void* pArg)
{
	CInstanceMesh* pInsStaticMesh = new CInstanceMesh(*this);

	if (FAILED(pInsStaticMesh->Initialize(pArg)))
	{
		Safe_Release(pInsStaticMesh);
		MSG_BOX("InstanceMesh Is Failed To Clone");
		return nullptr;
	}

	return pInsStaticMesh;
}

void CInstanceMesh::Free()
{
	Super::Free();

	Safe_Delete_Array(m_pInstanceWorldMinMax);

}

