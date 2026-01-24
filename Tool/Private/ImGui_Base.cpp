#include "pch.h"
#include "ImGui_Base.h"

CImGui_Base::CImGui_Base(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_strLabel(pLabel)
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

void CImGui_Base::Update(const _float fTimeDelta)
{
}

void CImGui_Base::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Super::Free();
}