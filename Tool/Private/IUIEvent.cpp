#include "pch.h"
#include "Tool_Defines.h"

#include "IUIEvent.h"
#include "GameInstance.h"

IUIEvent::IUIEvent(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:m_pDevice(pDevice),
	m_pDeviceContext(pDeviceContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

IUIEvent::IUIEvent(const IUIEvent& rhs) 
	: m_pDevice(rhs.m_pDevice),
	m_pDeviceContext(rhs.m_pDeviceContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT IUIEvent::Initialize(void* pArg)
{
	UIEVENT_DESC* pDesc = static_cast<UIEVENT_DESC*>(pArg);
	m_pOwner = pDesc->pOwner;

	return S_OK;
}

void IUIEvent::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pGameInstance);
}

