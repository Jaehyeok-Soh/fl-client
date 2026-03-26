#pragma once
#include "Base.h"

#define MAX_TIMESCALE 4.f

struct THitStop
{
	TIME_LINE time;
	_bool Is_Active() const { return time.Is_Active(); }
};

struct TSlowMo
{
	TIME_LINE time;
	_float fScale{ 1.f };
	_bool Is_Active() const { return time.Is_Active(); }
};

NS_BEGIN(Engine)

class CTimeScale_Manager final : public CBase
{
	using Super = CBase;
private:
	CTimeScale_Manager();
	virtual ~CTimeScale_Manager() = default;

	HRESULT Initialize();
public:
	_float Get_UnscaledDeltaTime() const { return m_fUnscaledDeltaTime; }
	_float Get_ScaledDeltaTime() const { return m_fScaledDeltaTime; }
	_float Get_AppliedScale() const { return m_fAppliedScale; }
	void Request_HitStop(_float fUnscaledDurationTime);
	void Request_SloMo(_float fScale, _float fUnscaledDurationTime);
	void Active_TimeStop();
	void Deactive_TimeStop();
	void Active_SloMo(_float fScale);
	void Deactivate_SloMo();
	void Set_GlobalScale(_float fScale) { m_fGlobalScale = std::clamp(fScale, 0.f, MAX_TIMESCALE); }
	_float Begin_Frame(_float fUnscaledDeltaTime);
	void Clear();
private:
	_float m_fUnscaledDeltaTime{ 0.f };
	_float m_fScaledDeltaTime{ 0.f };
	_float m_fGlobalScale{ 1.f };
	_float m_fAppliedScale{ 1.f };

	THitStop m_tHitstop{};
	TSlowMo m_tSloMo{};
public:
	static CTimeScale_Manager* Create();
	virtual void Free() override;
};

NS_END