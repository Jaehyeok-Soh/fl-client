#include "Engine_pch.h"
#include "ThreadPool.h"
#include "ThreadWorker.h"

CThreadPool::CThreadPool()
{
}

HRESULT CThreadPool::Initialize()
{
    m_bShutdown_Requested = false;
    m_iBusyThreads = 0;

    _uint iThreadCount = std::thread::hardware_concurrency();
    if (iThreadCount == 0)
        iThreadCount = 4; // fallback
    try
    {
        m_vecThreads.reserve(iThreadCount);

        for (_uint i = 0; i < iThreadCount; ++i)
        {
            m_vecThreads.emplace_back(CThreadWorker(this));
        }
    }
    catch (...)
    {
        return E_FAIL;
    }

    return S_OK;
}

CThreadPool* CThreadPool::Create()
{
	CThreadPool* pInstance = new CThreadPool();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CThreadPool::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CThreadPool::Free()
{
    {
        std::lock_guard<mutex> lock(m_mutex);
        m_bShutdown_Requested = true;
    }

    m_convar.notify_all();

    for (auto& t : m_vecThreads)
    {
        if (t.joinable())
            t.join();
    }
    m_vecThreads.clear();
	Super::Free();
}
