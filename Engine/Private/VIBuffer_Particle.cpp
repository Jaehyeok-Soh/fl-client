#include "Engine_pch.h"
#include "VIBuffer_Particle.h"
#include "GameInstance.h"

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

//  =============   새로 버퍼 할당  ==============
HRESULT CVIBuffer_Particle::Resize_InstanceBuffer(_uint iNumInstanceCount)
{
	m_iInstanceCount = iNumInstanceCount;

	// 기존 버퍼 해제
	Safe_Release(m_pVBInstance);
	Safe_Delete_Array(m_pInstanceVertices);
	Safe_Delete_Array(m_pSpeeds);
	
	// 새로운 버퍼 생성
	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];
	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;

	// 버퍼를 재할당 했다면 입자들 생명주기 등등 전부 새롭게.
	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		_float      fScale = m_pGameInstance->Rand_Float(m_tParticleDesc.vSize.x, m_tParticleDesc.vSize.y) * 0.5f;
		m_pSpeeds[i] = m_pGameInstance->Rand_Float(m_tParticleDesc.vSpeed.x, m_tParticleDesc.vSpeed.y);

		m_pInstanceVertices[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = Vec4(
			m_pGameInstance->Rand_Float(m_tParticleDesc.vCenter.x - m_tParticleDesc.vRange.x * 0.5f, m_tParticleDesc.vCenter.x + m_tParticleDesc.vRange.x * 0.5f),
			m_pGameInstance->Rand_Float(m_tParticleDesc.vCenter.y - m_tParticleDesc.vRange.y * 0.5f, m_tParticleDesc.vCenter.y + m_tParticleDesc.vRange.y * 0.5f),
			m_pGameInstance->Rand_Float(m_tParticleDesc.vCenter.z - m_tParticleDesc.vRange.z * 0.5f, m_tParticleDesc.vCenter.z + m_tParticleDesc.vRange.z * 0.5f),
			1.f
		);

		m_pInstanceVertices[i].vLifeTime = Vec2(0.f, m_pGameInstance->Rand_Float(m_tParticleDesc.vLifeTime.x, m_tParticleDesc.vLifeTime.y));
	}

	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, nullptr, &m_pVBInstance);
}

// 객체를 아예 초기로 전부 초기화 해주는 Reset 버튼
void CVIBuffer_Particle::Reset_Simulation()
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		pVertices[i].vLifeTime.x = 0.f;
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc)
{
	if (m_iInstanceCount != Desc.iInstnaceCount)
	{
		// 인스턴스 할 갯수가 줄었다면 버퍼 재할당하자
		m_tParticleDesc = Desc;
		Resize_InstanceBuffer(Desc.iInstnaceCount);
	}

	m_bIsLoop = Desc.isLoop;
	m_vPivot = Desc.vPivot;
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

void CVIBuffer_Particle::Update_Simulation(_float fTImeDelta, E_PARTICLE_MOVESTATE eType)
{
	switch (eType)
	{
		case E_PARTICLE_MOVESTATE::NONE:
			break;
		case E_PARTICLE_MOVESTATE::DROP:
			Drop(fTImeDelta);
			break;
		case E_PARTICLE_MOVESTATE::RISE:
			Rise(fTImeDelta);
			break;
		case E_PARTICLE_MOVESTATE::SPREAD:
			Spread(fTImeDelta);
			break;
	}
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
		Vec3		vLook = pVertices[i].vTranslation - m_vPivot * m_pSpeeds[i];
		vLook.Normalize();
		pVertices[i].vTranslation = pVertices[i].vTranslation + vLook * fTimeDelta;
		pVertices[i].vLifeTime.x += fTimeDelta;
		if (true == m_bIsLoop && pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
		}
	}

	m_pDeviceContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle::Rise(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pDeviceContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXPARTICLE* pVertices = static_cast<VTXPARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		// 1. 위로 이동 (Y축 증가)
		pVertices[i].vTranslation.y += m_pSpeeds[i] * fTimeDelta;

		// 2. 수명 업데이트
		pVertices[i].vLifeTime.x += fTimeDelta;

		// 3. 루프 처리 (수명이 다하면 초기 위치로 리셋)
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
