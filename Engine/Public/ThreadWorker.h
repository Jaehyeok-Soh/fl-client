#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CThreadPool;

class CThreadWorker
{
public:
	CThreadWorker(CThreadPool* pOwnerPool);
	virtual ~CThreadWorker() = default;
	void operator()();
private:
	CThreadPool* m_pOwnerPool = { nullptr };
};

NS_END