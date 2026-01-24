#include "Engine_pch.h"
#include "Bounding.h"

CBounding::CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CBounding::Initialize(BOUNDING_DESC* pDesc)
{
	return S_OK;
}

void CBounding::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}
