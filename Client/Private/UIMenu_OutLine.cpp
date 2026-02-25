#include "pch.h"
#include "UIMenu_OutLine.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIMenu_OutLine::CUIMenu_OutLine(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMenu_OutLine::CUIMenu_OutLine(const CUIMenu_OutLine& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMenu_OutLine::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_OutLine::Initialize(void* pArg)
{
	MENU_OUTLINE_DESC* pDesc = static_cast<MENU_OUTLINE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_OutLine::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	return S_OK;
}

void CUIMenu_OutLine::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMenu_OutLine::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMenu_OutLine::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMenu_OutLine::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	Emit_Light(fTimeDelta);
}

HRESULT CUIMenu_OutLine::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_OutLine::Ready_Components(MENU_OUTLINE_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIMenu_OutLine::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

void CUIMenu_OutLine::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;

	// 일회성 플래그
	if (ETriggerEventType::HOVER_ENTER == eEvent)
	{
		m_isTrigger_HoverEnter	= true;
		m_isFin_Event = false;
		m_fBrightness = 1.f;
	}

	// 일회성 플래그
	if (ETriggerEventType::HOVER_EXIT == eEvent)
	{
		m_isTrigger_HoverExit	= true;
		m_isFin_Event = false;
		m_fBrightness = m_fEmit;
	}

	// 일회성 플래그
	if (ETriggerEventType::PRESS_ENTER == eEvent)
	{
		m_isActive = false;

		if (m_isVisible)
			Set_Invisible();
		else
			Set_Visible();
	}
}

void CUIMenu_OutLine::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	const _float fDuration = 0.5f;
	Ready_Fade(fDuration, 0.f, 1.f, m_fDelay);
}

void CUIMenu_OutLine::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	const _float fDuration = 0.5f;
	Ready_Fade(fDuration, 1.f, 0.f, 0.f);
}

_bool CUIMenu_OutLine::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isFade)
	{
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

_bool CUIMenu_OutLine::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isFade)
	{
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

void CUIMenu_OutLine::Emit_Light(const _float fTimeDelta)
{
	if (m_isTrigger_HoverEnter)
	{
		m_fBrightness += fTimeDelta * 10.f;
		if (m_fBrightness > m_fEmit)
		{
			m_fBrightness = m_fEmit;
			m_isTrigger_HoverEnter = false;
			m_isFin_Event = true;
			m_isActive = true;
		}
	}
	else if (m_isTrigger_HoverExit)
	{
		m_fBrightness -= fTimeDelta * 10.f;
		if (m_fBrightness < 1.f)
		{
			m_fBrightness = 1.f;
			m_isTrigger_HoverExit = false;
			m_isFin_Event = true;
			m_isActive = true;
		}
	}
}

CUIMenu_OutLine* CUIMenu_OutLine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMenu_OutLine* pInstance = new CUIMenu_OutLine(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMenu_OutLine::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMenu_OutLine::Clone(void* pArg)
{
	CUIMenu_OutLine* pInstance = new CUIMenu_OutLine(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMenu_OutLine::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMenu_OutLine::Free()
{
	Super::Free();
}
