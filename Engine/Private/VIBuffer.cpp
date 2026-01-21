#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super()
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: Super(rhs)
	, m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VB_Usage(rhs.m_VB_Usage)
	, m_IB_Usage(rhs.m_IB_Usage)
	, m_VB_CPUAccesFlag(rhs.m_VB_CPUAccesFlag)
	, m_IB_CPUAccesFlag(rhs.m_IB_CPUAccesFlag)
	, m_pVertexPositions(rhs.m_pVertexPositions)
	, m_pIndices(rhs.m_pIndices)
	, m_iVertexBufferCount(rhs.m_iVertexBufferCount)
	, m_iVertexCount(rhs.m_iVertexCount)
	, m_iVertexStride(rhs.m_iVertexStride)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_iIndexCount(rhs.m_iIndexCount)
	, m_ePrimitiveType(rhs.m_ePrimitiveType)
	, m_eIndexFormat(rhs.m_eIndexFormat)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	if (!pArg)
	{
		m_VB_Usage = { D3D11_USAGE::D3D11_USAGE_DEFAULT };
		m_IB_Usage = { D3D11_USAGE::D3D11_USAGE_IMMUTABLE };
	}
	else
	{
		VIBUFFER_ORIGIN_DESC* pDesc = static_cast<VIBUFFER_ORIGIN_DESC*>(pArg);
		m_VB_Usage = pDesc->VB_Usage;
		m_IB_Usage = pDesc->IB_Usage;
	}

	switch (m_VB_Usage)
	{
	case D3D11_USAGE_DEFAULT:
	case D3D11_USAGE_IMMUTABLE:
	{
		m_VB_CPUAccesFlag = 0;
	} break;
	case D3D11_USAGE_DYNAMIC:
	{
		m_VB_CPUAccesFlag = D3D11_CPU_ACCESS_WRITE;
	} break;
	case D3D11_USAGE_STAGING:
	{
		m_VB_CPUAccesFlag = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	} break;
	default:
		return E_FAIL;
	}

	switch (m_IB_Usage)
	{
	case D3D11_USAGE_DEFAULT:
	case D3D11_USAGE_IMMUTABLE:
	{
		m_IB_CPUAccesFlag = 0;
	} break;
	case D3D11_USAGE_DYNAMIC:
	{
		m_IB_CPUAccesFlag = D3D11_CPU_ACCESS_WRITE;
	} break;
	case D3D11_USAGE_STAGING:
	{
		m_IB_CPUAccesFlag = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	} break;
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer::Render()
{
	m_pDeviceContext->DrawIndexed(m_iIndexCount, 0, 0);
}

HRESULT CVIBuffer::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] =
	{
		   m_pVB,
		   /*m_pVBInstance,*/
	};

	_uint		 iVertexStrides[] =
	{
		m_iVertexStride,
	};

	_uint		 iOffsets[] =
	{
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pVertexBuffers, iVertexStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

void CVIBuffer::Free()
{
	if (IsClone() == false)
	{
		Safe_Delete_Array(m_pVertexPositions);
		Safe_Delete_Array(m_pIndices);
	}

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Super::Free();
}
