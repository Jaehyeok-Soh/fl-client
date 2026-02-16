#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTimer final : public CBase
{
	using Super = CBase;
public:

private:
	_float	m_fMax_TimeDelta = { 0.07f };
private:
	CTimer();
	virtual ~CTimer() = default;

	HRESULT Initialize();
public:
	_float Get_TimeDelta() const { return m_fTimeDelta; }
	void Set_MaxTimeDelta(_float fMax) { m_fMax_TimeDelta = fMax; }
public:
	_float Update_Timer();

private:
	LARGE_INTEGER		m_FrameTime = {};
	LARGE_INTEGER		m_FixTime = {};
	LARGE_INTEGER		m_LastTime = {};
	LARGE_INTEGER		m_CpuTick = {};

	_float				m_fTimeDelta = {};
public:
	virtual void Free() override;
	static CTimer* Create();
};

NS_END
