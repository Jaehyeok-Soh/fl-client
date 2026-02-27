#include "pch.h"
#include "UIMenu_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIMenu_Text::CUIMenu_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIMenu_Text::CUIMenu_Text(const CUIMenu_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIMenu_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Text::Initialize(void* pArg)
{
	MENU_TEXT_DESC* pDesc = static_cast<MENU_TEXT_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIMenu_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	m_vOriginFontColor = m_vFontColor;

	return S_OK;
}

void CUIMenu_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMenu_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMenu_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMenu_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMenu_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Text::Ready_Components(MENU_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

void CUIMenu_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;

	if (eEvent == ETriggerEventType::PRESS_ENTER)
	{
		if (m_isVisible)
			Set_Invisible();
		else
			Set_Visible();
	}
	else if (eEvent == ETriggerEventType::HOVER_ENTER)
	{
		Set_Visible();
	}
	else if (eEvent == ETriggerEventType::HOVER_EXIT)
	{
		Set_Invisible();
	}
}

void CUIMenu_Text::Initialize_Visible_Event()
{
	m_vFontColor = Vec4{ 0.f ,0.f ,0.f ,0.f };

	if (m_eTextSubClassType == DTO::EUITextSubClassType::MENU_ESC_TEXT)
	{
		m_isActive = true;
		m_isFin_Event = false;
	}
	else
	{
		m_isActive = false;
		m_isFin_Event = false;
	}

	m_fTimeAcc = 0.f;
	m_fDelayTimeAcc = 0.f;
}

void CUIMenu_Text::Initialize_InVisible_Event()
{
	m_vFontColor = m_vOriginFontColor;
	m_isActive = false;
	m_isFin_Event = false;
	m_fTimeAcc = 0.f;
	m_fDelayTimeAcc = 0.f;
}

_bool CUIMenu_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fDelayTimeAcc += fTimeDelta;
	if (m_fDelayTimeAcc < m_fDelay)
		return false;

	m_fTimeAcc += fTimeDelta;

	_float t = m_fTimeAcc / 0.5f;
	if (t >= 1.f)
	{
		t = 1.f;
		m_vFontColor = m_vOriginFontColor;
		m_isActive = true;
		m_isFin_Event = true;
		return true;
	}

	const Vec4 vStart = Vec4{ 0.f, 0.f, 0.f, 0.f };
	const Vec4 vEnd = m_vOriginFontColor;
	m_vFontColor = vStart + (vEnd - vStart) * t;

	return false;
}

_bool CUIMenu_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isActive = true;
	m_isFin_Event = true;
	return true;
}

CUIMenu_Text* CUIMenu_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMenu_Text* pInstance = new CUIMenu_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMenu_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMenu_Text::Clone(void* pArg)
{
	CUIMenu_Text* pInstance = new CUIMenu_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMenu_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMenu_Text::Free()
{
	Super::Free();
}
