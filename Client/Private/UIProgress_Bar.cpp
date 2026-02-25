#include "pch.h"
#include "UIProgress_Bar.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIProgress_Bar::CUIProgress_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIProgress_Bar::CUIProgress_Bar(const CUIProgress_Bar& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIProgress_Bar::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIProgress_Bar::Initialize(void* pArg)
{
	PROGRESS_BAR_DESC* pDesc = static_cast<PROGRESS_BAR_DESC*>(pArg);
	m_eSubClassType = pDesc->eOwner;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIProgress_Bar::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	m_fDuration = 1.f;
	return S_OK;
}

void CUIProgress_Bar::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIProgress_Bar::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

}

void CUIProgress_Bar::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	Trigger_Ratio();
}

void CUIProgress_Bar::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	Tick_Ratio(fTimeDelta);
}

HRESULT CUIProgress_Bar::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Super::Render()))
		return E_FAIL;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIProgress_Bar::Ready_Components(PROGRESS_BAR_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIProgress_Bar::Bind_ShaderResources()
{
	return S_OK;
}

void CUIProgress_Bar::Trigger_Ratio()
{
	_float fEpsilon = 0.0001f;
	if (fabs(m_fCurRatio - m_fPreRatio) > fEpsilon)
	{
		m_isChangeRatio = TRUE;
		m_fStartRatio = m_fProgress_Ratio;
		m_fTargetRatio = m_fCurRatio;
		m_fTimeAcc = 0.f;
		m_fDelayTimeAcc = 0.f;
	}
	m_fPreRatio = m_fCurRatio;
}

void CUIProgress_Bar::Tick_Ratio(const _float fTimeDelta)
{
	if (m_isChangeRatio)
	{
		m_fDelayTimeAcc += fTimeDelta;
		if (m_fDelayTimeAcc <= m_fDelay)
			return;

		m_fTimeAcc += fTimeDelta;
		_float t = m_fTimeAcc / m_fDuration;

		if (t >= 1.f)
		{
			m_fProgress_Ratio = m_fTargetRatio;
			m_isChangeRatio = FALSE;
		}
		else
		{
			m_fProgress_Ratio = m_fStartRatio + (m_fTargetRatio - m_fStartRatio) * t;
		}
	}
}

void CUIProgress_Bar::Free()
{
	Super::Free();
}
