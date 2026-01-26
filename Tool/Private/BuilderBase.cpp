#include "pch.h"
#include "BuilderBase.h"
#include "GameInstance.h"

CBuilderBase::CBuilderBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);

	m_iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
}

void CBuilderBase::Free()
{
	Super::Free();
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
