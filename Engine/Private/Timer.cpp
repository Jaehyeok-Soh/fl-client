#include "Engine_pch.h"
#include "Timer.h"

CTimer::CTimer()
{
	::ZeroMemory(&m_FrameTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_FixTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_LastTime, sizeof(LARGE_INTEGER));
	::ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Initialize()
{
	::QueryPerformanceCounter(&m_FrameTime);			// 1077
	::QueryPerformanceCounter(&m_LastTime);			// 1085
	::QueryPerformanceCounter(&m_FixTime);			// 1090

	::QueryPerformanceFrequency(&m_CpuTick);		// cpu tick 값을 얻어오는 함수
	return S_OK;
}

_float CTimer::Update_Timer()
{
	::QueryPerformanceCounter(&m_FrameTime);

	if (m_FrameTime.QuadPart - m_FixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		m_FixTime = m_FrameTime;
	}

	m_fTimeDelta = (std::min)((m_FrameTime.QuadPart - m_LastTime.QuadPart) / (_float)m_CpuTick.QuadPart, m_fMax_TimeDelta);
	m_LastTime = m_FrameTime;
	return m_fTimeDelta;
}

void CTimer::Free()
{
	Super::Free();
}

CTimer* CTimer::Create()
{
	CTimer* pInstance = new CTimer();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CTimer::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}
