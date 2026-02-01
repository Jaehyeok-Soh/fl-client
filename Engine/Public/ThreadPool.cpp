#include "Engine_pch.h"
#include "ThreadPool.h"
#include "ThreadWorker.h"

CThreadPool::CThreadPool()
{
}

HRESULT CThreadPool::Initialize()
{
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
	Super::Free();
}
