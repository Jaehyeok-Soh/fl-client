#pragma once
#include "Base.h"

NS_BEGIN(Engine)

template<typename T>
class ENGINE_DLL CConstant_Buffer final : public CBase
{
	using Super = CBase;
private:
	CConstant_Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CConstant_Buffer() = default;

	HRESULT Initialize();
public:
	ID3D11Buffer* Get_Buffer() { return m_pBuffer; }
	ID3D11Buffer** Get_Buffer_AddressOf() { return &m_pBuffer; }
	HRESULT Copy_Data(const T& data);
private:
	_bool m_bHasLast = { false };
	T m_Last{};
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pDeviceContext = { nullptr };
	ID3D11Buffer* m_pBuffer = { nullptr };
public:
	static CConstant_Buffer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

template<typename T>
inline CConstant_Buffer<T>::CConstant_Buffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

template<typename T>
inline HRESULT CConstant_Buffer<T>::Initialize()
{
	D3D11_BUFFER_DESC desc;
	::ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	// CPU Write / GPU Read
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	// constnatbuffer를 만들때는 16byte 정렬이 이루어 져야 하기때문에 구조체 내 dummy를 추가한다.
	desc.ByteWidth = sizeof(T);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(m_pDevice->CreateBuffer(&desc, nullptr, &m_pBuffer)))
		return E_FAIL;

	return S_OK;
}

template<typename T>
inline HRESULT CConstant_Buffer<T>::Copy_Data(const T& data)
{
	if (m_bHasLast == true && 0 == ::memcmp(&m_Last, &data, sizeof(T)))
		return S_OK;

	m_Last = data;
	m_bHasLast = true;
	D3D11_MAPPED_SUBRESOURCE SubResource;
	::ZeroMemory(&SubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);
	::memcpy(SubResource.pData, &data, sizeof(T));
	m_pDeviceContext->Unmap(m_pBuffer, 0);

	return S_OK;
}

template<typename T>
inline CConstant_Buffer<T>* CConstant_Buffer<T>::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CConstant_Buffer<T>* pInstance = new CConstant_Buffer<T>(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CConstant_Buffer<T>::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

template<typename T>
inline void CConstant_Buffer<T>::Free()
{
	Safe_Release(m_pBuffer);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}

NS_END