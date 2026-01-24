#include "Engine_pch.h"
#include "Timer_Manager.h"
#include "Timer.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Initialize()
{
	return S_OK;
}

_float CTimer_Manager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (!pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Add_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (pTimer)
	{
		MSG_BOX("CTimer_Manager::Add_Timer, KeyValue Duplicated");
		return E_FAIL;
	}

	pTimer = CTimer::Create();
	if (!pTimer)
		return E_FAIL;	

	m_mapTimers.emplace(pTimerTag, pTimer);
	return S_OK;
}

void CTimer_Manager::Remove_Timer(const _tchar* pTimerTag)
{
	auto itr = m_mapTimers.find(pTimerTag);
	if (itr == m_mapTimers.end())
		return;

	CTimer* pTimer = itr->second;
	Safe_Release(pTimer);
	m_mapTimers.erase(itr);
}

void CTimer_Manager::Compute_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (!pTimer)
		return;

	pTimer->Update_Timer();
}

void CTimer_Manager::Clear_Timers()
{
	if (m_mapTimers.size() <= 0)
		return;

	for (auto& element : m_mapTimers)
	{
		Safe_Release(element.second);
	}
	m_mapTimers.clear();
}

CTimer* CTimer_Manager::Find_Timer(const _tchar* pTimerTag)
{
	if (m_mapTimers.size() <= 0)
		return nullptr;

	map<const _wstring, CTimer*>::iterator itr = m_mapTimers.find(pTimerTag);

	if (itr == m_mapTimers.end())
		return nullptr;

	return itr->second;
}

void CTimer_Manager::Free()
{
	Clear_Timers();
	Super::Free();
}

CTimer_Manager* CTimer_Manager::Create()
{
	CTimer_Manager* pInstance = new CTimer_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CTimer_Manager::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}
