#include "VIBuffer_Particle.h"

CVIBuffer_Particle::CVIBuffer_Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Particle::CVIBuffer_Particle(const CVIBuffer_Particle& rhs)
	: Super(rhs)
	, m_pVBInstance{ rhs.m_pVBInstance }
	, m_InstanceBufferDesc{ rhs.m_InstanceBufferDesc }
	, m_pInstanceVertices{ rhs.m_pInstanceVertices }
	, m_iIndexCountPerInstance{ rhs.m_iIndexCountPerInstance }
	, m_iInstanceCount{ rhs.m_iInstanceCount }
	, m_iInstanceVertexStride{ rhs.m_iInstanceVertexStride }
	, m_pSpeeds{ rhs.m_pSpeeds }
	, m_bIsLoop{ rhs.m_bIsLoop }
{
}

HRESULT CVIBuffer_Particle::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

	PARTICLE_ORIGIN_DESC* pDesc = static_cast<PARTICLE_ORIGIN_DESC*>(pArg);
	m_bIsLoop = pDesc->isLoop;
	m_iInstanceCount = pDesc->iInstnaceCount;
	m_vPivot = pDesc->vPivot;
	return S_OK;
}

HRESULT CVIBuffer_Particle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Particle::Bind_Resource()
{
	ID3D11Buffer* pVertexBuffers[] = {
		m_pVB,
		m_pVBInstance
	};

	_uint		 iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint		 iOffsets[] = {
		0,
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iVertexBufferCount, pVertexBuffers, iVertexStrides, iOffsets);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

void CVIBuffer_Particle::Render()
{
	m_pDeviceContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iInstanceCount, 0, 0, 0);
}

void CVIBuffer_Particle::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;
		pVertices[i].vLifeTime.x += fTimeDelta;
		if (true == m_bIsLoop && pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Spread(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		/*pVertices[i].vTranslation.y -= m_pSpeeds[i] * fTimeDelta;*/
		_vector		vLook = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f)) * m_pSpeeds[i];

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vLook * fTimeDelta);

		pVertices[i].vLifeTime.x += fTimeDelta;
		if (true == m_bIsLoop && pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Free()
{
	if(IsClone() == false)
	{
		Safe_Delete_Array(m_pSpeeds);
		Safe_Delete_Array(m_pInstanceVertices);
	}
	Safe_Release(m_pVBInstance);
	Super::Free();
}
