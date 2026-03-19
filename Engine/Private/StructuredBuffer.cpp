#include "Engine_pch.h"
#include "StructuredBuffer.h"

StructuredBuffer::StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
	_uint iElementSize, _uint iNumElements)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_iElementSize(iElementSize)
	, m_iElementCount(iNumElements)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT StructuredBuffer::Initialize(_uint iElementSize, _uint iNumElements)
{
	m_iElementCount = iNumElements;
	m_iElementSize = iElementSize;

	// ID3D11Buffer 생성
	{
		D3D11_BUFFER_DESC desc;
		::ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

		// 버퍼 본체 설정
		desc.ByteWidth = m_iElementSize * m_iElementCount;
		// GPU가 직접 쓰고 읽어야 하므로 DEFAULT를 사용. (CPU가 직접 MAP / UMAP하지 않음)
		desc.Usage = D3D11_USAGE_DEFAULT;
		// 위에 기술했듯이 StructuredBuffer는 SRV와 UAV가 반드시 필요하다. 
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS; // VERTEX_BUFFER 플래그 사용 불가.
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		desc.StructureByteStride = m_iElementSize;

		if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pBuffer)))
			return E_FAIL;
	}

	// SRV 생성	: 읽기용
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		::ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;		// 구조화 버퍼는 UNKNOWN으로 설정을 한다고 한다.
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;		// D3D11_UAV_DIMENSION_BUFFER 리소스를 버퍼로 봅니다 - 출처 MSDN
		srvDesc.Buffer.NumElements = m_iElementCount;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pBuffer, &srvDesc, &m_pSRV)))
			return E_FAIL;
	}

	// UAV 생성	: 쓰기용
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		::ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;		// D3D11_UAV_DIMENSION_BUFFER 리소스를 버퍼로 봅니다 - 출처 MSDN
		uavDesc.Buffer.NumElements = m_iElementCount;

		if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pBuffer, &uavDesc, &m_pUAV)))
			return E_FAIL;
	}
	return S_OK;
}


HRESULT StructuredBuffer::Copy_Data(void* data, _uint iElementSize, _uint iCount)
{
	if (data == nullptr) return E_FAIL;

	// 어디서부터 어디까지 복사해라 명령을 따로 내려주어야한다.
	D3D11_BOX destBox;
	destBox.left = 0;
	destBox.right = iCount * iElementSize;
	destBox.top = 0;
	destBox.bottom = 1;
	destBox.front = 0;
	destBox.back = 1;

	m_pDeviceContext->UpdateSubresource(m_pBuffer, 0, &destBox, data, 0, 0);

	return S_OK;
}

HRESULT StructuredBuffer::Resize(void* data, _uint iElementSize, _uint iNewNumElements)
{
	// 기존 리소스 해제
	Safe_Release(m_pBuffer);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);

	Initialize(iElementSize, iNewNumElements);
	return Copy_Data(data, iElementSize, iNewNumElements);
}

void StructuredBuffer::CopyFrom(StructuredBuffer* pSrc)
{
	m_pDeviceContext->CopyResource(m_pBuffer, pSrc->Get_Buffer());
}

StructuredBuffer* StructuredBuffer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iElementSize, _uint iNumElements)
{
	StructuredBuffer* pInstance = new StructuredBuffer(pDevice, pDeviceContext, iElementSize, iNumElements);

	if (FAILED(pInstance->Initialize(iElementSize, iNumElements)))
	{
		MSG_BOX("StructuedBuffer<T>::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void StructuredBuffer::Free()
{
	Super::Free();

	Safe_Release(m_pBuffer);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

}
