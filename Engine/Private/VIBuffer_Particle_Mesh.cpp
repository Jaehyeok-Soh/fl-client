#include "Engine_pch.h"
#include "VIBuffer_Particle_Mesh.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"
#include "GameObject.h"
#include "ComputeShader.h"

#define BOX 0
#define CIRCLE 1
#define SPHERE 2
#define CONE 3

CVIBuffer_Particle_Mesh::CVIBuffer_Particle_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

CVIBuffer_Particle_Mesh::CVIBuffer_Particle_Mesh(const CVIBuffer_Particle_Mesh& rhs)
	: Super(rhs)
{
}

HRESULT CVIBuffer_Particle_Mesh::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Particle_Mesh::Initialize(void* pArg)
{
	// 슬롯 0번을 채워주기 위해서 값을 복사해온다.
	PARTICLE_ORIGIN_DESC* pDesc = static_cast<PARTICLE_ORIGIN_DESC*>(pArg);
	if (pDesc == nullptr) return E_FAIL;
	else
	{
		m_tParticleDesc = *pDesc;
		m_tParticleOriginDesc = m_tParticleDesc;
	}

	Resize_InstanceBuffer(*pDesc);

	return S_OK;
}

//  =============   새로 버퍼 할당  ==============
HRESULT CVIBuffer_Particle_Mesh::Resize_InstanceBuffer(const PARTICLE_ORIGIN_DESC& Desc)
{
	if (m_tParticleDesc.pModel == nullptr) return E_FAIL;
	// 매쉬 하나만 들고오기 (어차피 이펙트 객체는 매쉬가 1개.)
	CMesh* pMesh = m_tParticleDesc.pModel->Get_Mesh(0);

	// Stride 재설정
	m_iVertexBufferCount = 2;
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	m_iInstanceCount = Desc.iInstnaceCount;

	// 기존 버퍼 해제
	Safe_Release(m_pVB); // 원본 메쉬 VB도 해제하고 삭제
	Safe_Release(m_pIB);
	Safe_Release(m_pVBInstance);
	Safe_Delete_Array(m_pInstanceVertices);
	Safe_Delete_Array(m_pSpeeds);

	// 버텍스 버퍼 복사해서 가지고오기.
	Copy_VertexBuffer(pMesh);
	// 인덱스 버퍼 복사해서 가지고오기.
	Copy_IndexBuffer(pMesh);
	// 인스턴스 버퍼 재할당.
	Set_Instance_Buffer();

	Set_ResizeBuffer_SpecificRandom();
		
	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance);
}

void CVIBuffer_Particle_Mesh::Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc)
{
	// 그냥 매프레임 resize 갈겨 난 몰라
	Resize_InstanceBuffer(Desc);
	m_fStartSpeeds = Desc.m_fStartSpeeds;
	m_bIsLoop = Desc.isLoop;
	m_vPivot = Desc.vPivot;
	m_tParticleDesc = Desc;
}

HRESULT CVIBuffer_Particle_Mesh::Set_ResizeBuffer_SpecificRandom()
{
	EFFECT_PARTICLE_IMMU_ELEMENT* pInitialData = new EFFECT_PARTICLE_IMMU_ELEMENT[m_iInstanceCount];
	_uint iFlags = m_tParticleDesc.iRandomFlags; // 툴에서 받아온 비트 플래그

	_float fDuration = m_tParticleDesc.fDuration;
	if (fDuration <= 0.f) fDuration = 1.f; // 방어 코드

	_float fSpawnInterval = fDuration / (_float)m_iInstanceCount;
	_float fSpeed = 1.f;

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		// 1. 크기(Size) 결정
		// 매쉬의 경우 vSize.y를 기본 배율로 사용하고 계시네요.
		_float fScale = m_tParticleDesc.vSize.y;
		if (iFlags & DTO::E_RANDOM_FLAG::RAND_SIZE)
		{
			fScale = m_pGameInstance->Rand_Float(m_tParticleDesc.vSize.x, m_tParticleDesc.vSize.y);
		}

		_float fMaxLifeTime = m_tParticleDesc.vLifeTime.y;
		if (iFlags & DTO::E_RANDOM_FLAG::RAND_LIFE)
			fMaxLifeTime = m_pGameInstance->Rand_Float(m_tParticleDesc.vLifeTime.x, m_tParticleDesc.vLifeTime.y);

		if (iFlags & DTO::E_RANDOM_FLAG::RAND_SPEED)
			fSpeed = m_pGameInstance->Rand_Float(0.1f, 1.f);

		if (fMaxLifeTime < 1.0f)
			fMaxLifeTime = 1.0f;


		if (m_tParticleDesc.UseBurst)
		{
			pInitialData[i].vParticle_LifeTime = Vec2(m_tParticleDesc.vLifeTime.x, fMaxLifeTime);
		}
		else
		{
			_float fInitialDelay = i * fSpawnInterval;
			pInitialData[i].vParticle_LifeTime = Vec2(-fInitialDelay, fMaxLifeTime);
		}

		pInitialData[i].fSpeed = fSpeed;
		pInitialData[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
		pInitialData[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
		pInitialData[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);

		// 3. 위치(Position) 결정
		SimpleMath::Vector3 vPos = m_tParticleDesc.vCenter;

		if (iFlags & DTO::E_RANDOM_FLAG::RAND_POS)
		{
			// 기존 매쉬 에미션 타입별 랜덤 로직 수행
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
		pInitialData[i].vParticle_OriginMatrix = Matrix(
			pInitialData[i].vRight, pInitialData[i].vUp,
			pInitialData[i].vLook, pInitialData[i].vTranslation);
	}

	CComputeShader* pShader = m_tParticleDesc.pComputeShader;
	if (pShader == nullptr)
	{
		MSG_BOX("VIBUFFER_PARTICLE_MESH ERROR : SHADER NULLPTR");
		Safe_Delete_Array(pInitialData);
		return E_FAIL;
	}
	pShader->Resize_InputStruct(0, pInitialData, sizeof(EFFECT_PARTICLE_IMMU_ELEMENT), m_iInstanceCount);
	pShader->Resize_OutputStruct(0, pInitialData, sizeof(EFFECT_INSTANCE), m_iInstanceCount);

	Safe_Delete_Array(pInitialData);

	return S_OK;
}

HRESULT CVIBuffer_Particle_Mesh::Set_Instance_Buffer()
{
	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		m_pInstanceVertices[i].vInstanceNumber = (_uint)i;
	}

	return S_OK;
}

HRESULT CVIBuffer_Particle_Mesh::Copy_IndexBuffer(CMesh* pMesh)
{
	// 인덱스 버퍼 복사해오기
	{
		ID3D11Buffer* pOriginalIB = pMesh->Get_IBBuffer();
		D3D11_BUFFER_DESC IBDesc;
		pOriginalIB->GetDesc(&IBDesc);

		D3D11_BUFFER_DESC IBStagingDesc = IBDesc;
		IBStagingDesc.Usage = D3D11_USAGE_STAGING;
		IBStagingDesc.BindFlags = 0;
		IBStagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		IBStagingDesc.MiscFlags = 0;

		ID3D11Buffer* pIBStagingBuffer = nullptr;
		if (FAILED(m_pDevice->CreateBuffer(&IBStagingDesc, nullptr, &pIBStagingBuffer)))
			return E_FAIL;

		m_pDeviceContext->CopyResource(pIBStagingBuffer, pOriginalIB);

		D3D11_MAPPED_SUBRESOURCE IBMappedResource{};
		if (SUCCEEDED(m_pDeviceContext->Map(pIBStagingBuffer, 0, D3D11_MAP_READ, 0, &IBMappedResource)))
		{
			D3D11_BUFFER_DESC NewIBDesc = IBDesc;
			NewIBDesc.Usage = D3D11_USAGE_DEFAULT;
			NewIBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			NewIBDesc.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA IBInitData{};
			IBInitData.pSysMem = IBMappedResource.pData;

			if (FAILED(m_pDevice->CreateBuffer(&NewIBDesc, &IBInitData, &m_pIB)))
				return E_FAIL;

			//m_iIndexCountPerInstance = IBDesc.ByteWidth / 2;
			m_iIndexCountPerInstance = IBDesc.ByteWidth / (m_eIndexFormat == DXGI_FORMAT_R32_UINT ? sizeof(_uint) : sizeof(_short));

			m_pDeviceContext->Unmap(pIBStagingBuffer, 0);
		}
		Safe_Release(pIBStagingBuffer);
	}

	return S_OK;
}

HRESULT CVIBuffer_Particle_Mesh::Copy_VertexBuffer(CMesh* pMesh)
{
	// Vertex Buffer 복사하기

	ID3D11Buffer* pOriginalVB = pMesh->Get_VBBuffer();
	D3D11_BUFFER_DESC bufferDesc;
	pOriginalVB->GetDesc(&bufferDesc);

	D3D11_BUFFER_DESC StagingDesc = bufferDesc;
	StagingDesc.Usage = D3D11_USAGE_STAGING;
	StagingDesc.BindFlags = 0;
	StagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	StagingDesc.MiscFlags = 0;

	ID3D11Buffer* pStagingBuffer = nullptr;
	if (FAILED(m_pDevice->CreateBuffer(&StagingDesc, nullptr, &pStagingBuffer)))
		return E_FAIL;

	m_pDeviceContext->CopyResource(pStagingBuffer, pOriginalVB);

	D3D11_MAPPED_SUBRESOURCE MappedResource{};
	if (SUCCEEDED(m_pDeviceContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &MappedResource)))
	{
		m_iVertexCount = bufferDesc.ByteWidth / sizeof(VTXMESH);
		m_iVertexStride = sizeof(VTXMESH);

		D3D11_BUFFER_DESC NewVBDesc = bufferDesc;
		NewVBDesc.Usage = D3D11_USAGE_DEFAULT;
		NewVBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		NewVBDesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData{};
		InitData.pSysMem = MappedResource.pData;

		m_pDevice->CreateBuffer(&NewVBDesc, &InitData, &m_pVB);
		m_pDeviceContext->Unmap(pStagingBuffer, 0);
	}
	Safe_Release(pStagingBuffer);

	return S_OK;
}

CVIBuffer_Particle_Mesh* CVIBuffer_Particle_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	CVIBuffer_Particle_Mesh* pInstance = new CVIBuffer_Particle_Mesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Particle_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Particle_Mesh::Clone(void* pArg)
{
	CVIBuffer_Particle_Mesh* pInstance = new CVIBuffer_Particle_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Particle_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Particle_Mesh::Free()
{
	Super::Free();
}
