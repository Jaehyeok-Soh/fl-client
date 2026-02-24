#include "Engine_pch.h"
#include "VIBuffer_Particle_Point.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "ComputeShader.h"
#include "StructuredBuffer.h"

#define BOX 0
#define CIRCLE 1
#define SPHERE 2
#define CONE 3

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
	if (FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

	m_iVertexCount = 1;

	Safe_Delete_Array(m_pVertexPositions);

	m_pVertexPositions = new Vec3[m_iVertexCount];
	ZeroMemory(m_pVertexPositions, sizeof(Vec3) * m_iVertexCount);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Initialize(void* pArg)
{
	PARTICLE_POINT_ORIGIN_DESC* pParticleDesc = static_cast<PARTICLE_POINT_ORIGIN_DESC*>(pArg);
	if (pParticleDesc == nullptr) return E_FAIL;

	m_tParticleDesc = *pParticleDesc;
	m_iInstanceCount = pParticleDesc->iInstnaceCount;
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_iVertexBufferCount = 2; 

	Set_Owner(pParticleDesc->pOwner);

	return Resize_InstanceBuffer(*pParticleDesc);
}

//  =============   새로 버퍼 할당  ==============
HRESULT CVIBuffer_Particle_Point::Resize_InstanceBuffer(const PARTICLE_ORIGIN_DESC& Desc)
{
	m_iInstanceCount = Desc.iInstnaceCount;

	Safe_Release(m_pVB);
	Safe_Release(m_pVBInstance);
	Safe_Delete_Array(m_pInstanceVertices);
	Safe_Delete_Array(m_pSpeeds);

	if (FAILED(Set_VertexBuffer(Desc)))
		return E_FAIL;

	if (FAILED(Set_InstanceBuffer()))
		return E_FAIL;

	HRESULT hr = S_OK;

	hr = Set_ResizeBuffer_SpecificRandom();

	return hr;
}

void CVIBuffer_Particle_Point::Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc)
{
	m_tParticleDesc = Desc;
	Resize_InstanceBuffer(Desc);

	m_fStartSpeeds = Desc.m_fStartSpeeds;
	m_bIsLoop = Desc.isLoop;
	m_vPivot = Desc.vPivot;
}

HRESULT CVIBuffer_Particle_Point::Set_VertexBuffer(const PARTICLE_ORIGIN_DESC& Desc)
{
	m_iVertexStride = sizeof(VTXPOS);
	m_iVertexCount = 1;
	m_iIndexStride = 0;
	m_eIndexFormat = DXGI_FORMAT_UNKNOWN;

	m_iIndexCount = 0;
	m_iVertexBufferCount = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_iIndexCountPerInstance = m_iIndexCount;

	D3D11_BUFFER_DESC           VertexBufferDesc{};
	VertexBufferDesc.ByteWidth = m_iVertexStride * m_iVertexCount;
	VertexBufferDesc.Usage = m_VB_Usage;
	VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertexBufferDesc.CPUAccessFlags = m_VB_CPUAccesFlag;
	VertexBufferDesc.MiscFlags = 0;
	VertexBufferDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iVertexCount];
	ZeroMemory(pVertices, sizeof(VTXPOS) * m_iVertexCount);

	m_pVertexPositions[0] = pVertices[0].vPosition = Vec3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA      VertexInitialData{};
	VertexInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VertexBufferDesc, &VertexInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Set_ResizeBuffer_SpecificRandom()
{
	EFFECT_PARTICLE_IMMU_ELEMENT* pInitialData = new EFFECT_PARTICLE_IMMU_ELEMENT[m_iInstanceCount];
	_uint iFlags = m_tParticleDesc.iRandomFlags;

	_float fDuration = m_tParticleDesc.fDuration;
	if (fDuration <= 0.f) fDuration = 1.f; // 방어 코드

	// 생성 간격 계산
	_float fSpawnInterval = fDuration / (_float)m_iInstanceCount;

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		_float fScale = m_tParticleDesc.vSize.x * 0.5f;

		if (iFlags & DTO::E_RANDOM_FLAG::RAND_SIZE)
			fScale = m_pGameInstance->Rand_Float(m_tParticleDesc.vSize.x, m_tParticleDesc.vSize.y) * 0.5f;

		_float fMaxLifeTime = m_tParticleDesc.vLifeTime.y;
		if (iFlags & DTO::E_RANDOM_FLAG::RAND_LIFE)
			fMaxLifeTime = m_pGameInstance->Rand_Float(m_tParticleDesc.vLifeTime.x, m_tParticleDesc.vLifeTime.y);

		if (fMaxLifeTime < 1.0f) 
			fMaxLifeTime = 1.0f;


		// [핵심 수정] 2. 수명 설정 (vLifeTime.x = 현재수명, vLifeTime.y = 최대수명)
		// i가 커질수록 더 큰 음수값이 들어가서 더 늦게 0(탄생)에 도달합니다.
		_float fInitialDelay = i * fSpawnInterval;
		pInitialData[i].vParticle_LifeTime = Vec2(-fInitialDelay, fMaxLifeTime);


		pInitialData[i].fSpeed = 1.f;
		pInitialData[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
		pInitialData[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
		pInitialData[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
		// ======== Emission Type에 따른 위치 계산 ========
		SimpleMath::Vector3 vPos = m_tParticleDesc.vCenter;

		if (iFlags & DTO::E_RANDOM_FLAG::RAND_POS)
		{
			switch (m_tParticleDesc.EmissionFlagType)
			{
			case BOX:
			{
				vPos.x += m_pGameInstance->Rand_Float(-m_tParticleDesc.vRange.x * 0.5f, m_tParticleDesc.vRange.x * 0.5f);
				vPos.y += m_pGameInstance->Rand_Float(-m_tParticleDesc.vRange.y * 0.5f, m_tParticleDesc.vRange.y * 0.5f);
				vPos.z += m_pGameInstance->Rand_Float(-m_tParticleDesc.vRange.z * 0.5f, m_tParticleDesc.vRange.z * 0.5f);
				break;
			}

			case CIRCLE:
			{
				_float fRadius = m_tParticleDesc.vRange.x;
				_float fAngle = m_pGameInstance->Rand_Float(0.f, DirectX::XM_2PI);

				vPos.x += cos(fAngle) * fRadius;
				vPos.y += sin(fAngle) * fRadius;
				break;
			}
			case SPHERE:
			{
				_float fPhi = m_pGameInstance->Rand_Float(0.f, DirectX::XM_2PI);
				_float fTheta = acos(m_pGameInstance->Rand_Float(-1.f, 1.f));

				Vec3 vUnitVector;
				vUnitVector.x = sin(fTheta) * cos(fPhi);
				vUnitVector.y = sin(fTheta) * sin(fPhi);
				vUnitVector.z = cos(fTheta);

				// 반지름과 부피 적용
				_float fRadius = m_tParticleDesc.vRange.x * m_pGameInstance->Rand_Float(0.f, 1.f);
				vPos += vUnitVector * fRadius;
				break;
			}

			case CONE:
			{
				// vRange.x : 밑면 반지름으로 쓰기
				// vRange.y : 원뿔 각도 

				_float fRadius = m_tParticleDesc.vRange.x * m_pGameInstance->Rand_Float(0.f, 1.f);
				_float fAngle = m_pGameInstance->Rand_Float(0.f, DirectX::XM_2PI);

				// 밑면 위치 Circle과 동일함.
				Vec3 vBottomPos;
				vBottomPos.x = cos(fAngle) * fRadius;
				vBottomPos.z = sin(fAngle) * fRadius;
				vBottomPos.y = 0.f;

				// 원뿔 위쪽으로 퍼지는 오프셋
				vPos += vBottomPos;
				break;
			}
			}
		}
		pInitialData[i].vTranslation = Vec4(vPos.x, vPos.y, vPos.z, 1.f);

		pInitialData[i].vParticle_OriginMatrix =
			Matrix(pInitialData[i].vRight,
				pInitialData[i].vUp,
				pInitialData[i].vLook,
				pInitialData[i].vTranslation);
	}
	CComputeShader* pShader = m_tParticleDesc.pComputeShader;
	if (pShader == nullptr)
	{
		MSG_BOX("VIBUFFER_PARTICLE_POINT : Can't Bind Effect Compute Data : ERROR SHADER NULLPTR");
		return E_FAIL;
	}
	pShader->Resize_InputStruct(0, pInitialData, sizeof(EFFECT_PARTICLE_IMMU_ELEMENT), m_iInstanceCount);
	Safe_Delete_Array(pInitialData);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Point::Set_InstanceBuffer()
{
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;

	m_pSpeeds = new _float[m_iInstanceCount];
	::ZeroMemory(m_pSpeeds, sizeof(_float) * m_iInstanceCount);

	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];
	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		m_pInstanceVertices[i].vInstanceNumber = (_uint)i;
	}

	m_InstanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = 0;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance);
}



HRESULT CVIBuffer_Particle_Point::Bind_Resource()
{
	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

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
