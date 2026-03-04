#include "pch.h"
#include "UIText.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIText::CUIText(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIText::CUIText(const CUIText& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIText::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIText::Initialize(void* pArg)
{
	UI_TEXT_DESC* pDesc = static_cast<UI_TEXT_DESC*>(pArg);
	m_eTextSubClassType = pDesc->eTextSubClass;
	m_eShaderType = pDesc->eShaderType;
	m_wstrFontTag	= pDesc->wstrFontTag;
	if (m_wstrFontTag == L"")
		int a = 0;
	m_wstrText		= pDesc->wstrText;
	m_vFontColor	= pDesc->vFontColor;
	m_fFontRotate	= pDesc->fRotate;
	m_fFontScale	= pDesc->fScale;
	m_ePivot		= pDesc->ePivot;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	m_vFontPos = Vec2{ m_fX, m_fY };
	return S_OK;
}

HRESULT CUIText::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIText::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	return S_OK;
}

void CUIText::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIText::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIText::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIText::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	if (nullptr != m_pWorldUIComp)
	{
		m_vFontPos.x = m_fX;
		m_vFontPos.y = m_fY;
		m_fScaleOffset = m_pWorldUIComp->Get_ScaleOffset();
	}
	else
	{
		m_vFontPos.x = m_vRenderPos.x;
		m_vFontPos.y = m_vRenderPos.y;
		m_fScaleOffset = 1.f;
	}
}

HRESULT CUIText::Render()
{
	// UI는 별도의 UI Manager에서 Render 그룹에 넣어주고, m_isVisible 같은 예외처리를 최하단 객체에서 해줌
	// Request_DrawFont를 Update계열에서 호출하면 Text UI는 매니저의 Render나 m_isVisible 같은 렌더 상태 제어를 받지 않고 무조건 출력됨
	// Text만 따로 예외처리 하기 싫어서 Render에서 호출
	Sync_FontDesc();
	if (FAILED(m_pGameInstance->Request_DrawFont(m_tFontDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIText::Ready_Components(UI_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIText::Bind_ShaderResources()
{
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

void CUIText::Sync_FontDesc()
{
	m_tFontDesc.eFontShaderType = m_eShaderType;
	m_tFontDesc.strFontTag = m_wstrFontTag;
	m_tFontDesc.strText = m_wstrText;
	Vec2 fontPos = Vec2{ m_vFontPos.x, m_vFontPos.y };
	m_tFontDesc.vPosition = fontPos;
	m_tFontDesc.vColor = m_vFontColor;
	m_tFontDesc.ePivot = m_ePivot;
	m_tFontDesc.fRotate = m_fFontRotate;
	m_tFontDesc.fScale = m_fFontScale * m_fScaleOffset;
}

_wstring CUIText::Float_To_Wstring(const _float f, _uint iDecimal)
{
	_tchar sz[64];
	switch (iDecimal)
	{
	case 0:	swprintf_s(sz, 64, L"%.0f", f);	break;
	case 1:	swprintf_s(sz, 64, L"%.1f", f);	break;
	default: swprintf_s(sz, 64, L"%.2f", f); break;
	}
	return sz;
}

void CUIText::Ready_Fade_Text(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fDelay)
{
	m_vFontColor.w				= fStartAlpha;

	m_fFont_FadeTimeAcc			= 0.f;
	m_fFont_FadeDelayTimeAcc	= 0.f;

	m_fFont_FadeDelay			= fDelay;
	m_fFont_FadeDuration		= fDuration;
	m_fFont_StartAlphaRatio		= fStartAlpha;
	m_fFont_TargetAlphaRatio	= fTargetAlpha;
}

_bool CUIText::Tick_Fade_Text(const _float fTimeDelta)
{
	m_fFont_FadeDelayTimeAcc += fTimeDelta;
	if (m_fFont_FadeDelayTimeAcc < m_fFont_FadeDelay)
		return false;

	m_fFont_FadeTimeAcc += fTimeDelta;

	_float t = m_fFont_FadeTimeAcc / m_fFont_FadeDuration;

	if (t >= 1.f)
	{
		m_vFontColor.w = m_fFont_TargetAlphaRatio;
		return true;
	}

	m_vFontColor.w = m_fFont_StartAlphaRatio + (m_fFont_TargetAlphaRatio - m_fFont_StartAlphaRatio) * t;
	return false;
}

void CUIText::Free()
{
	Super::Free();
}
