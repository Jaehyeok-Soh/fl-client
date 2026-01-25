#include "Engine_pch.h"
#include "VIBuffer_Particle_Mesh.h"
#include "GameInstance.h"
#include "Model.h"
#include "Mesh.h"

VIBuffer_Particle_Mesh::VIBuffer_Particle_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

VIBuffer_Particle_Mesh::VIBuffer_Particle_Mesh(const VIBuffer_Particle_Mesh& rhs)
	: Super(rhs)
{
}

HRESULT VIBuffer_Particle_Mesh::Initialize_Prototype(void* pArg)
{
	if (FAILED(Super::Initialize_Prototype(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT VIBuffer_Particle_Mesh::Initialize(void* pArg)
{
	// 슬롯 0번을 채워주기 위해서 값을 복사해온다.
	PARTICLE_Mesh_ORIGIN_DESC* pDesc = static_cast<PARTICLE_Mesh_ORIGIN_DESC*>(pArg);
	m_iInstanceVertexStride = sizeof(VTXPOS_PARTICLEMESH);
	m_eIndexFormat = m_iIndexStride == 2 ? DXGI_FORMAT::DXGI_FORMAT_R16_UINT : DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

	CModel* pModel = pDesc->_Model;
	if (!pModel) return E_FAIL;

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

VIBuffer_Particle_Mesh* VIBuffer_Particle_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	VIBuffer_Particle_Mesh* pInstance = new VIBuffer_Particle_Mesh(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX("Failed to Created : VIBuffer_Particle_Mesh");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* VIBuffer_Particle_Mesh::Clone(void* pArg)
{
	VIBuffer_Particle_Mesh* pInstance = new VIBuffer_Particle_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : VIBuffer_Particle_Mesh");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void VIBuffer_Particle_Mesh::Free()
{
	Super::Free();
}
