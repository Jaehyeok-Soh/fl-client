#include "Engine_pch.h"
#include "VIBuffer_Particle_Mesh.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

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
	PARTICLE_Mesh_ORIGIN_DESC* pDesc = static_cast<PARTICLE_Mesh_ORIGIN_DESC*>(pArg);
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	m_iVertexBufferCount = 2;

	CModel* pModel = pDesc->_Model;
	if (!pModel) return E_FAIL;

 	_uint pMeshCount = pModel->Get_MeshCount();
	CMesh* pMesh = pModel->Get_Mesh(0);
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

			m_iIndexCountPerInstance = IBDesc.ByteWidth / (m_eIndexFormat == DXGI_FORMAT_R32_UINT ? sizeof(_uint) : sizeof(_short));

			m_pDeviceContext->Unmap(pIBStagingBuffer, 0);
		}
		Safe_Release(pIBStagingBuffer);
	}

#pragma region INSTANCE BUFFER
	
	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	// 인스턴스 갯수만큼 Speed 배열 할당
	m_pSpeeds = new _float[m_iInstanceCount];
	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];

	for (size_t i = 0; i < m_iInstanceCount; i++)
	{
		_float      fScale = m_pGameInstance->Rand_Float(pDesc->vSize.x, pDesc->vSize.y) * 0.5f;
		m_pSpeeds[i] = m_pGameInstance->Rand_Float(pDesc->vSpeed.x, pDesc->vSpeed.y);

		m_pInstanceVertices[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
		m_pInstanceVertices[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
		m_pInstanceVertices[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
		m_pInstanceVertices[i].vTranslation = Vec4(
			m_pGameInstance->Rand_Float(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f),
			m_pGameInstance->Rand_Float(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand_Float(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f),
			1.f
		);

		m_pInstanceVertices[i].vLifeTime = Vec2(0.f, m_pGameInstance->Rand_Float(pDesc->vLifeTime.x, pDesc->vLifeTime.y));
	}

#pragma endregion

	return S_OK;
}

//  =============   새로 버퍼 할당  ==============
HRESULT CVIBuffer_Particle_Mesh::Resize_InstanceBuffer(_uint iNumInstanceCount)
{
	// Stride 재설정
	m_iInstanceVertexStride = sizeof(VTXPARTICLE);
	m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

	//m_iIndexStride = 2;
	//m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_iInstanceCount = iNumInstanceCount;

	// 기존 버퍼 해제
	Safe_Release(m_pVBInstance);
	Safe_Delete_Array(m_pInstanceVertices);
	Safe_Delete_Array(m_pSpeeds);

	if (m_tParticleDesc.pModel == nullptr)
		return E_FAIL;

	CMesh* pMesh = m_tParticleDesc.pModel->Get_Mesh(0);
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

#pragma region INSTANCE BUFFER
	
	m_InstanceBufferDesc.ByteWidth = m_iInstanceCount * m_iInstanceVertexStride;
	m_InstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_InstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_InstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_InstanceBufferDesc.MiscFlags = 0;
	m_InstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	// 인스턴스 갯수만큼 Speed 배열 할당
	m_pSpeeds = new _float[m_iInstanceCount];
	m_pInstanceVertices = new VTXPARTICLE[m_iInstanceCount];

	if (m_tParticleDesc.isRandomSeed == false)
	{
		for (size_t i = 0; i < m_iInstanceCount; i++)
		{
			_float      fScale = m_tParticleDesc.vSize.y * 0.5f;
			m_pSpeeds[i] = m_tParticleDesc.vSpeed.y;

			m_pInstanceVertices[i].vRight = Vec4(fScale, 0.f, 0.f, 0.f);
			m_pInstanceVertices[i].vUp = Vec4(0.f, fScale, 0.f, 0.f);
			m_pInstanceVertices[i].vLook = Vec4(0.f, 0.f, fScale, 0.f);
			m_pInstanceVertices[i].vTranslation = Vec4(
				m_tParticleDesc.vCenter.x,
				m_tParticleDesc.vCenter.y,
				m_tParticleDesc.vCenter.z,
				1.f
			);

			m_pInstanceVertices[i].vLifeTime = Vec2(0.f, m_tParticleDesc.vLifeTime.y);
		}
	}

	else
	{
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
	}
#pragma endregion

	D3D11_SUBRESOURCE_DATA InstanceInitialData{};
	InstanceInitialData.pSysMem = m_pInstanceVertices;

	return m_pDevice->CreateBuffer(&m_InstanceBufferDesc, &InstanceInitialData, &m_pVBInstance);
}

void CVIBuffer_Particle_Mesh::Set_ParticleDesc(const PARTICLE_ORIGIN_DESC& Desc)
{
	if (m_iInstanceCount != Desc.iInstnaceCount ||
		m_tParticleDesc.vSize.x != Desc.vSize.x ||
		m_tParticleDesc.vSize.y != Desc.vSize.y ||
		m_tParticleDesc.vRange.x != Desc.vRange.x ||
		m_tParticleDesc.vRange.y != Desc.vRange.y ||
		m_tParticleDesc.vRange.z != Desc.vRange.z ||
		m_tParticleDesc.isRandomSeed != Desc.isRandomSeed ||
		m_tParticleDesc.pModel != Desc.pModel ||
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
