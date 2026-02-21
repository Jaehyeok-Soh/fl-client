#include "Engine_pch.h"
#include "TimeScale_Manager.h"

CTimeScale_Manager::CTimeScale_Manager()
{
}

HRESULT CTimeScale_Manager::Initialize()
{
	return S_OK;
}

void CTimeScale_Manager::Request_HitStop(_float fUnscaledDurationTime)
{
	if (fUnscaledDurationTime > m_tHitstop.time.Get_Remain())
		m_tHitstop.time.Start(fUnscaledDurationTime);
}

void CTimeScale_Manager::Request_SloMo(_float fScale, _float fUnscaledDurationTime)
{
	m_tSloMo.fScale = fScale;
	m_tSloMo.time.Start(fUnscaledDurationTime);
}

void CTimeScale_Manager::Active_SloMo(_float fScale)
{
	if (m_tSloMo.Is_Active() == true)
		return;

	m_tSloMo.fScale = fScale;
	m_tSloMo.time.Start(FLT_MAX);
}

void CTimeScale_Manager::Deactivate_SloMo()
{
	if (m_tSloMo.Is_Active() == false)
		return;

	m_tSloMo.fScale = 1.f;
	m_tSloMo.time.Clear();
}

_float CTimeScale_Manager::Begin_Frame(_float fUnscaledDeltaTime)
{
	m_fUnscaledDeltaTime = fUnscaledDeltaTime;

	if (m_tHitstop.Is_Active() == true)
		m_tHitstop.time.Tick(fUnscaledDeltaTime);
	if (m_tSloMo.Is_Active() == true)
		m_tSloMo.time.Tick(fUnscaledDeltaTime);

	// HitStop > SloMo > Global
	if (m_tHitstop.Is_Active() == true)
		m_fAppliedScale = 0.0f;
	else if (m_tSloMo.Is_Active() == true)
		m_fAppliedScale = std::clamp(m_tSloMo.fScale, 0.f, 1.f) * m_fGlobalScale;
	else
		m_fAppliedScale = m_fGlobalScale;

	m_fScaledDeltaTime = m_fUnscaledDeltaTime * m_fAppliedScale;
	return m_fScaledDeltaTime;
}

CTimeScale_Manager* CTimeScale_Manager::Create()
{
	CTimeScale_Manager* pInstance = new CTimeScale_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CTimeScale_Manager::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimeScale_Manager::Clear()
{
	m_fUnscaledDeltaTime = 0.f;
	m_fScaledDeltaTime = 0.f;
	m_fGlobalScale = 1.f;
	m_fAppliedScale = 1.f;
	m_tHitstop.time.Clear();
	m_tSloMo.time.Clear();
}

void CTimeScale_Manager::Free()
{
	Super::Free();
}
