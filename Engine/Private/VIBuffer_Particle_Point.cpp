#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs)
	: Super(rhs)
{
}

HRESULT CVIBuffer_Particle_Point::Initialize_Prototype(void* pArg)
{
	if(FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

	m_iVertexStride = sizeof(VTXPOS);
	m_iVertexCount = 1;
	m_iIndexStride = 0;
	m_eIndexFormat = DXGI_FORMAT_UNKNOWN;

	m_iIndexCount = 0;
	m_iVertexBufferCount = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	PARTICLE_POINT_ORIGIN_DESC* pParticleDesc = static_cast<PARTICLE_POINT_ORIGIN_DESC*>(pArg);
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_iIndexCountPerInstance = m_iIndexCount;

#pragma region VERTEX_BUFFER
	D3D11_BUFFER_DESC           VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iVertexCount;
	VertexBufferDesc.Usage = m_VB_Usage;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iVertexCount);

	m_pVertexPositions = new _float3[m_iVertexCount];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iVertexCount);

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA      VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

#pragma endregion

#pragma region INSTANCE_BUFFER

	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];
	::ZeroMemory(m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iInstanceCount);

	m_pSpeeds = new _float[m_iInstanceCount];
	::ZeroMemory(m_pSpeeds, sizeof(_float) * m_iInstanceCount);

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		_float      fScale = m_pGameInstance->Rand_Float(pParticleDesc->vSize.x, pParticleDesc->vSize.y) * 0.5f;
		m_pSpeeds[i] = m_pGameInstance->Rand_Float(pParticleDesc->vSpeed.x, pParticleDesc->vSpeed.y);

		m_pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand_Float(pParticleDesc->vCenter.x - pParticleDesc->vRange.x * 0.5f, pParticleDesc->vCenter.x + pParticleDesc->vRange.x * 0.5f),
			m_pGameInstance->Rand_Float(pParticleDesc->vCenter.y - pParticleDesc->vRange.y * 0.5f, pParticleDesc->vCenter.y + pParticleDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand_Float(pParticleDesc->vCenter.z - pParticleDesc->vRange.z * 0.5f, pParticleDesc->vCenter.z + pParticleDesc->vRange.z * 0.5f),
			1.f
		);

		m_pInstanceVertices[i].vLifeTime = _float2(0.f, m_pGameInstance->Rand_Float(pParticleDesc->vLifeTime.x, pParticleDesc->vLifeTime.y));
	}

	Safe_Delete_Array(pVertices);
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	D3D11_SUBRESOURCE_DATA      InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Bind_Resource()
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
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

void CVIBuffer_Particle_Point::Render()
{
	m_pDeviceContext->DrawInstanced(1, m_iInstanceCount, 0, 0);
}

CVIBuffer_Particle_Point* CVIBuffer_Particle_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Particle_Point::Clone(void* pArg)
{
	CVIBuffer_Particle_Point* pInstance = new CVIBuffer_Particle_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Point");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Point::Free()
{
	Super::Free();
}
