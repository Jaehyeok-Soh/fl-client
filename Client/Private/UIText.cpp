#include "pch.h"
#include "UIText.h"
#include "Client_Defines.h"

//=================
// Component
//=================
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
	if (FAILED(Ready_Components(pDesc)))
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

	m_vOriginFontColor = m_vFontColor;
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
	m_vFontPos.x = m_vRenderPos.x;
	m_vFontPos.y = m_vRenderPos.y;

	Super::Update_Late(fTimeDelta);
}

void CUIText::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIText::Render()
{
	if (!m_isVisible)
		return S_OK;
	
	Sync_FontDesc();

	if (FAILED(m_pGameInstance->Request_DrawFont(m_tFontDesc)))
		return E_FAIL;
		
	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIText::Ready_Components(UI_TEXT_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIText::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	return S_OK;
}

void CUIText::Sync_FontDesc()
{
	m_tFontDesc.eFontShaderType = m_eShaderType;
	m_tFontDesc.strFontTag = m_wstrFontTag;
	m_tFontDesc.strText = m_wstrText;
	Vec2 fontPos = Vec2{ m_vRenderPos.x, m_vRenderPos.y };
	m_tFontDesc.vPosition = fontPos;
	m_tFontDesc.vColor = m_vFontColor;
	m_tFontDesc.ePivot = m_ePivot;
	m_tFontDesc.fRotate = m_fFontRotate;
	m_tFontDesc.fScale = m_fFontScale;
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

void CUIText::Free()
{
	Super::Free();
}
