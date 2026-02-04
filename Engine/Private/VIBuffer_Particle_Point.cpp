#include "Engine_pch.h"
#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ComputeShader.h"
#include "StructuredBuffer.h"

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Particle_Point::CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& rhs)
	: Super(rhs)
{
	//m_pVBInstance = nullptr;
	//m_pInstanceVertices = nullptr;
	//m_pSpeeds = nullptr;

	//// 깊복
	//if (rhs.m_iInstanceCount > 0)
	//{
	//	m_InstanceBufferDesc = rhs.m_InstanceBufferDesc;

	//	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];
	//	m_pSpeeds = new _float[m_iInstanceCount];

	//	memcpy(m_pInstanceVertices, rhs.m_pInstanceVertices, sizeof(VTXPARTICLE) * m_iInstanceCount);
	//	memcpy(m_pSpeeds, rhs.m_pSpeeds, sizeof(_float) * m_iInstanceCount);
	//}
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

	m_pVertexPositions = new Vec3[m_iVertexCount];
	ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

	m_pVertexPositions[0] = pVertices[0].vPosition = Vec3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA      VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	PARTICLE_POINT_ORIGIN_DESC* pParticleDesc = static_cast<PARTICLE_POINT_ORIGIN_DESC*>(pArg);
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_iIndexCountPerInstance = m_iIndexCount;
	Set_Owner(pParticleDesc->pOwner);

#pragma region INSTANCE_BUFFER

	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		m_pInstanceVertices[i].vInstanceNumber = (_uint)i;
	}

	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 초기화용이므로 DEFAULT나 IMMUTABLE
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = 0;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance)))
		return E_FAIL;

#pragma endregion 




#pragma region COMPUTE_SHADER
	EFFECT_PARTICLE_IMMU_ELEMENT* pInitialData = new EFFECT_PARTICLE_IMMU_ELEMENT[m_iInstanceCount];

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		_float      fScale = m_pGameInstance->Rand_Float(pParticleDesc->vSize.x, pParticleDesc->vSize.y) * 0.5f;

		pInitialData[i].fSpeed = 1.f;
		pInitialData[i].vParticle_LifeTime = Vec2(0.f, m_pGameInstance->Rand_Float(pParticleDesc->vLifeTime.x, pParticleDesc->vLifeTime.y));
		pInitialData[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
		pInitialData[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
		pInitialData[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
		pInitialData[i].vTranslation = Vec4(
			m_pGameInstance->Rand_Float(pParticleDesc->vCenter.x - pParticleDesc->vRange.x * 0.5f, pParticleDesc->vCenter.x + pParticleDesc->vRange.x * 0.5f),
			m_pGameInstance->Rand_Float(pParticleDesc->vCenter.y - pParticleDesc->vRange.y * 0.5f, pParticleDesc->vCenter.y + pParticleDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand_Float(pParticleDesc->vCenter.z - pParticleDesc->vRange.z * 0.5f, pParticleDesc->vCenter.z + pParticleDesc->vRange.z * 0.5f),
			1.f
		);
		pInitialData[i].vParticle_OriginMatrix =
			Matrix(pInitialData[i].vRight,
				pInitialData[i].vUp,
				pInitialData[i].vLook,
				pInitialData[i].vTranslation);
	}
	CComputeShader* pShader = pParticleDesc->pComputeShader;
	if (pShader == nullptr)
	{
		MSG_BOX("VIBUFFER_PARTICLE_POINT : Can't Bind Effect Compute Data : ERROR SHADER NULLPTR");
		return E_FAIL;
	}
	pShader->Bind_InputStructuredBuffer_Data(pInitialData, sizeof(EFFECT_PARTICLE_IMMU_ELEMENT), m_iInstanceCount);
	Safe_Delete_Array(pInitialData);
#pragma endregion

	return S_OK;
}

//  =============   새로 버퍼 할당  ==============
HRESULT CVIBuffer_Particle_Point::Resize_InstanceBuffer(_uint iNumInstanceCount)
{
	m_iInstanceCount = iNumInstanceCount;

	// 기존 버퍼 해제
	Safe_Release(m_pVBInstance);
	Safe_Delete_Array(m_pInstanceVertices);
	Safe_Delete_Array(m_pSpeeds);

	// 새로운 버퍼 생성
	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_pSpeeds = new _float[m_iInstanceCount];
	::ZeroMemory(m_pSpeeds, sizeof(_float) * m_iInstanceCount);

	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		m_pInstanceVertices[i].vInstanceNumber = (_uint)i;
	}

	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 초기화용이므로 DEFAULT나 IMMUTABLE
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = 0;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	// 버퍼를 재할당 했다면 입자들 생명주기 등등 전부 새롭게.

	//if (m_tParticleDesc.isRandomSeed == false)
	//{
	//	for (size_t i = 0; i < m_iInstanceCount; i++)
	//	{
	//		_float      fScale = m_tParticleDesc.vSize.y * 0.5f;
	//		m_pSpeeds[i] = m_tParticleDesc.vSpeed.y;

	//		m_pInstanceVertices[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
	//		m_pInstanceVertices[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
	//		m_pInstanceVertices[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
	//		m_pInstanceVertices[i].vTranslation = Vec4(
	//			m_tParticleDesc.vCenter.x,
	//			m_tParticleDesc.vCenter.y,
	//			m_tParticleDesc.vCenter.z,
	//			1.f
	//		);

	//		m_pInstanceVertices[i].vLifeTime = Vec2(0.f, m_tParticleDesc.vLifeTime.y);
	//	}
	//}

	//else if (m_tParticleDesc.isRandomSeed == true)
	//{
	//	for (size_t i = 0; i < m_iInstanceCount; i++)
	//	{
	//		_float      fScale = m_pGameInstance->Rand_Float(m_tParticleDesc.vSize.x, m_tParticleDesc.vSize.y) * 0.5f;
	//		m_pSpeeds[i] = m_pGameInstance->Rand_Float(m_tParticleDesc.vSpeed.x, m_tParticleDesc.vSpeed.y);

	//		m_pInstanceVertices[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
	//		m_pInstanceVertices[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
	//		m_pInstanceVertices[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
	//		m_pInstanceVertices[i].vTranslation = Vec4(
	//			m_pGameInstance->Rand_Float(m_tParticleDesc.vCenter.x - m_tParticleDesc.vRange.x * 0.5f, m_tParticleDesc.vCenter.x + m_tParticleDesc.vRange.x * 0.5f),
	//			m_pGameInstance->Rand_Float(m_tParticleDesc.vCenter.y - m_tParticleDesc.vRange.y * 0.5f, m_tParticleDesc.vCenter.y + m_tParticleDesc.vRange.y * 0.5f),
	//			m_pGameInstance->Rand_Float(m_tParticleDesc.vCenter.z - m_tParticleDesc.vRange.z * 0.5f, m_tParticleDesc.vCenter.z + m_tParticleDesc.vRange.z * 0.5f),
	//			1.f
	//		);

	//		m_pInstanceVertices[i].vLifeTime = Vec2(0.f, m_pGameInstance->Rand_Float(m_tParticleDesc.vLifeTime.x, m_tParticleDesc.vLifeTime.y));
	//	}
	//}

	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance);
}

void CVIBuffer_Particle_Point::Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc)
{
	if (m_iInstanceCount != Desc.iInstnaceCount ||
		m_tParticleDesc.vSize.x != Desc.vSize.x ||
		m_tParticleDesc.vSize.y != Desc.vSize.y ||
		m_tParticleDesc.vRange.x != Desc.vRange.x ||
		m_tParticleDesc.vRange.y != Desc.vRange.y ||
		m_tParticleDesc.vRange.z != Desc.vRange.z ||
		m_tParticleDesc.isRandomSeed != Desc.isRandomSeed ||
		m_tParticleDesc.vLifeTime.y != Desc.vLifeTime.y)
	{
		// 인스턴스 할 갯수가 줄었다면 버퍼 재할당하자
		m_tParticleDesc = Desc;
		Resize_InstanceBuffer(Desc.iInstnaceCount);
	}

	m_fStartSpeeds = Desc.m_fStartSpeeds;
	m_bIsLoop = Desc.isLoop;
	m_vPivot = Desc.vPivot;
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
