#pragma once
#include "Base.h"

NS_BEGIN(Engine)

/*
	{StructedBuffer 생성 이유}

	 - 기존 ConstantBuffer와 Buffer 구조가 다름

	 1.  
		i)  ConstantBuffer는 SRV를 아예 쓰지 않는 독자적인 통로였었다.
			SRV를 쓰지 않고 무조건 DATE 값만 16Byte 정렬해서 던져주고 있었다.

	    ii) StructuredBuffer는 ID3D11Buffer 1개로만 데이터 통로를 뚫는 것이 아니라
			어떤식으로 데이터를 읽어야하는지 그리고 어떻게 내보내야하는지 알려주어야한다.
			즉, SRV 1개 + UAV 1개가 필요하다.
						*SRV (Shader Resource View)		*UAV (Unordered Access View)

			- 일반적인 Compute Shading 연산을 하려면 ID3D11Buffer 1개 + SRV 1개 + UAV 1개.
			- CPU로 결과를 가져와야할 때,			   ID3D11Buffer 2개 + SRV 1개 + UAV 1개.

			 (( GPU 전용 메모리는 CPU가 읽지 못해서, 복사 전용인 Staging Buffer를 하나 더 만드는 것. )) << 이는 Rookiss 코드에서도 적나라하게 나온다.

		


	 [ 여기서 잠깐!!! ConstantBuffer는 SRV 멤버 변수가 없었는데요? ]   띠요오오옹?
		 -> 잠깐 SRV 정의를 보고가자.

		SRV는 DATA가 아니라 View이다.
		GPU입장에서는 메모리(Buffer & Texture)는 그냥 Byte 덩어리일 뿐. 이게 RGBA 색상 값인지, 3D 좌표인지, 단순 숫자 배열인지 GPU는 스스로 알지 못함.

		ID3D11Buffer / ID3D11Texture2D : 실제 데이터가 담긴 메모리 본체
		SRV(Shader Resource View) : 이 메모리를 읽기 전용(Shader Resource)로 해석해서 봐라. 라고 GPU에게 알려주는 안경 역할.

		Texture를 바인딩할 때는 GPU에게 이건 Texture이고 RGBA형태로 읽어야해! 라고 알려주어야하기 때문에 SRV를 통해서 보여주어야한다.
		우리가 ConstantBuffer를 생성해서 ID3D11Buffer를 통해서 값을 map unmap을 통해 CPU에서 값을 덮어씌워주고 GPU가 그것을 읽고 있었을 뿐.
		-> 그냥 float 몇개짜리 ** 데이터 ** 야!! 하고 알려주고 있었을 뿐이다. 
		-> 그래서 GPU에서 16byte씩 데이터 정렬이 필요했던 것.
*/
template<typename T>
class StructuredBuffer :
    public CBase
{
	using Super = CBase;

private:
	StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumElements);
	virtual ~StructuredBuffer() = default;

	HRESULT Initialize(_uint iNumElements);
public:
	ID3D11Buffer* Get_Buffer() { return m_pBuffer; }
	ID3D11Buffer** Get_Buffer_AddressOf() { return &m_pBuffer; }

	ID3D11ShaderResourceView* Get_SRV() { return m_pSRV; }
	ID3D11ShaderResourceView** Get_SRV_AddressOf() { return &m_pSRV; }

	ID3D11UnorderedAccessView* Get_UAV() { return m_pUAV; }
	ID3D11UnorderedAccessView** Get_UAV_AddressOf() { return &m_pUAV; }

	HRESULT Copy_Data(const T* data, _uint iCount);
	HRESULT Resize(_uint iNewNumElements);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	ID3D11Buffer* m_pBuffer = { nullptr };
	ID3D11ShaderResourceView* m_pSRV = { nullptr };
	ID3D11UnorderedAccessView* m_pUAV = { nullptr };

public:
	_uint m_iElementCount = 0;
	_uint m_iElementSize = 0;

public:
	static StructuredBuffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumElements);
	virtual void Free() override;
};

template<typename T>
inline StructuredBuffer<T>::StructuredBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumElements)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_iElementCount(iNumElements)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

/*
	어라 Constant Buffer 때는 없었잖아요 선생님
	m_iElementCount << 누구세요?

	GPU에 할당받아야 할 공간의 크기가 얼마인지 알려주어야 쓰레드 풀을 GPU가 미리 준비하고 
	Dispatch를 실행시켰을 떄 설정한 배열 갯수만큼 계산할 수 있다.

	Struct<T>의 크기 * Struct<T> 갯수만큼 GPU Data Capacity 확보.
*/

template<typename T>
inline HRESULT StructuredBuffer<T>::Initialize(_uint iNumElements) 
{
	m_iElementCount = iNumElements;
	m_iElementSize = sizeof(T);

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

/*
	Constant Buffer와는 다르게 구조체가 배열일 수도 있으니 &참조 형태가 아니라 포인터로 받는다.
	
	D3D11_USAGE_DEFAULT 버퍼는 Map / UnMap을 사용할 수 없다.

	DEFAULT는 CPU가 데이터를 넣으려면 UpdateSubresource를 쓰거나 별도의 Staging Buffer를 거져야 한다.
	초기 데이터를 쏴주고 싶은게 있을 수도 있으니 Copy Data 구조를 그대로 가져간다.
*/
template<typename T>
inline HRESULT StructuredBuffer<T>::Copy_Data(const T* data, _uint iCount)
{
	if (data == nullptr) return E_FAIL;

	// 어디서부터 어디까지 복사해라 명령을 따로 내려주어야한다.
	D3D11_BOX destBox;
	destBox.left = 0;
	destBox.right = iCount * sizeof(T);
	destBox.top = 0;
	destBox.bottom = 1;
	destBox.front = 0;
	destBox.back = 1;


	m_pDeviceContext->UpdateSubresource(m_pBuffer, 0, &destBox, data, 0, 0);

	return S_OK;
}

template<typename T>
inline HRESULT StructuredBuffer<T>::Resize(_uint iNewNumElements)
{
	if (m_iElementCount == iNewNumElements) return S_OK;

	// 기존 리소스 해제
	Safe_Release(m_pBuffer);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);

	return Initialize(iNewNumElements);
}

template<typename T>
inline StructuredBuffer<T>* StructuredBuffer<T>::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iNumElements)
{
	StructuredBuffer<T>* pInstance = new StructuredBuffer<T>(pDevice, pDeviceContext, iNumElements);

	if (FAILED(pInstance->Initialize(iNumElements)))
	{
		MSG_BOX("StructuedBuffer<T>::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

template<typename T>
inline void StructuredBuffer<T>::Free()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}

NS_END