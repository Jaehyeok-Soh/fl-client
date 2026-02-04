#include "Engine_pch.h"
#include "InstanceMesh.h"
#include "GameInstance.h"

CInstanceMesh::CInstanceMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CInstanceMesh::CInstanceMesh(const CInstanceMesh& rhs)
	: CVIBuffer(rhs)
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
}

