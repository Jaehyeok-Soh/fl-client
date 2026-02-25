#include "Engine_pch.h"
#include "VIBuffer_Particle.h"
#include "GameInstance.h"
#include "StructuredBuffer.h"
#include "GameObject.h"
#include "ComputeShader.h"

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
	m_pOwner = pDesc->pOwner;
	return S_OK;
}

HRESULT CVIBuffer_Particle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Particle::Reset_Simulation()
{
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

	m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return S_OK;
}

void CVIBuffer_Particle::Render()
{
	//Debug_CheckVertexBuffer();
	m_pDeviceContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iInstanceCount, 0, 0, 0);

}

void CVIBuffer_Particle::Update_Simulation(CComputeShader* ComputeShader, Vec3 vLook, Vec3 finalGravity, _float fTImeDelta, _uint TimeFlag, DTO::E_SHAPETYPE eType)
{
	if (ComputeShader == nullptr) return;

	// 가변 데이터 desc 작성
	EFFECT_PARTICLE_MU_ELEMENT tMUDesc;
	tMUDesc.fTimeDelta = fTImeDelta;
	tMUDesc.iMoveState = ENUM_TO_UINT(eType);
	tMUDesc.bIsLoop = m_tParticleDesc.isLoop; 
	tMUDesc.fStartSpeed = m_tParticleDesc.m_fStartSpeeds;
	tMUDesc.vLook = vLook;
	tMUDesc.vPivot = m_tParticleDesc.vPivot;
	tMUDesc.iTimeFlag = TimeFlag;
	tMUDesc.vFinalGravity = finalGravity;
	tMUDesc.fExternalStrength = {};
	tMUDesc.UseContinueFlag = m_tParticleDesc.UseContinueFlag;
	
	// comshader에 컨스턴트 버퍼를 통해 매 프레임 갱신 : 가변 데이터
	ComputeShader->Bind_Compute_EffectData(tMUDesc);

	// Compute Shader 실행
	_uint iGroupX = (m_iInstanceCount + 31) / 32;
	ComputeShader->Dispatch(iGroupX, 1, 1);
}

void CVIBuffer_Particle::Debug_CheckVertexBuffer()
{
	if (m_pVB == nullptr) return;

	// 1. 원본 버퍼의 정보를 가져온다.
	D3D11_BUFFER_DESC desc;
	m_pVB->GetDesc(&desc);

	// 2. CPU가 읽을 수 있는 Staging Buffer 생성
	ID3D11Buffer* pStagingBuffer = nullptr;
	D3D11_BUFFER_DESC stagingDesc = desc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	stagingDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateBuffer(&stagingDesc, nullptr, &pStagingBuffer)))
		return;

	// 3. GPU -> Staging Buffer로 데이터 복사
	m_pDeviceContext->CopyResource(pStagingBuffer, m_pVB);

	// 4. 데이터 읽기
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (SUCCEEDED(m_pDeviceContext->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
	{
		// 네가 확인한 56바이트 VTXMESH 구조체로 캐스팅
		VTXMESH* pVertices = reinterpret_cast<VTXMESH*>(mappedResource.pData);
		_uint iCount = desc.ByteWidth / sizeof(VTXMESH);

		// 너무 많으면 터지니까 앞의 10개만 확인해보자
		for (_uint i = 0; i < 10 && i < iCount; ++i)
		{
			printf("[%d] Pos: %.2f, %.2f, %.2f | UV: %.2f, %.2f\n",
				i, pVertices[i].vPosition.x, pVertices[i].vPosition.y, pVertices[i].vPosition.z,
				pVertices[i].vUV.x, pVertices[i].vUV.y);
		}

		m_pDeviceContext->Unmap(pStagingBuffer, 0);
	}

	Safe_Release(pStagingBuffer);

}
void CVIBuffer_Particle::Free()
{
	//if(IsClone() == false)
	//{
	Safe_Delete_Array(m_pSpeeds);
	Safe_Delete_Array(m_pInstanceVertices);
	/*}*/
	Safe_Release(m_pVBInstance);
	Super::Free();
}
