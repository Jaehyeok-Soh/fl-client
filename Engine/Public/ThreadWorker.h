#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CThreadPool;

class CThreadWorker final : public CBase
{
	using Super = CBase;
private:
	CThreadWorker(CThreadPool* pOwnerPool);
	virtual ~CThreadWorker() = default;

	HRESULT Initialize();
public:
	void operator()();
private:
	CThreadPool* m_pOwnerPool = { nullptr };
public:
	static CThreadWorker* Create(CThreadPool* pOwnerPool);
	virtual void Free() override;
};

NS_END