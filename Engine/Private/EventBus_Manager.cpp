#include "EventBus_Manager.h"

CEventBus_Manager::CEventBus_Manager()
{
	m_channels.reserve(50);
}

HRESULT CEventBus_Manager::Initialize()
{
	return S_OK;
}

void CEventBus_Manager::Clear_All()
{
	for (auto& Pair : m_channels)
	{
		Safe_Delete(Pair.second);
	}
	m_channels.clear();
}

CEventBus_Manager* CEventBus_Manager::Create()
{
	CEventBus_Manager* pInstance = new CEventBus_Manager;
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CEventBus_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEventBus_Manager::Free()
{
	Clear_All();
	Super::Free();	
}
