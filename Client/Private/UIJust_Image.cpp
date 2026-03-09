#include "pch.h"
#include "UIJust_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"
#include <UI_Manager.h>

CUIJust_Image::CUIJust_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIJust_Image::CUIJust_Image(const CUIJust_Image& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIJust_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIJust_Image::Initialize(void* pArg)
{
	JUST_IMAGE_DESC* pDesc = static_cast<JUST_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	m_fOriginAlpha = m_fAlpha_Ratio;
	return S_OK;
}

HRESULT CUIJust_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIJust_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIJust_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIJust_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIJust_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIJust_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIJust_Image::Ready_Components(JUST_IMAGE_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIJust_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();
	return S_OK;
}

void CUIJust_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	Ready_Fade(0.5f, 0.f, m_fOriginAlpha, m_fDelay);
}

void CUIJust_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	Ready_Fade(0.5f, m_fOriginAlpha, 0.f, m_fDelay);
}

_bool CUIJust_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	if (Tick_Fade(fTimeDelta))
	{
		m_fAlpha_Ratio = m_fOriginAlpha;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

_bool CUIJust_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	if (Tick_Fade(fTimeDelta))
	{
		m_fAlpha_Ratio = 0.f;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

void CUIJust_Image::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					this->Set_Visible();
				}
			})
	);
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			})
	);

	m_pGameInstance->Subscribe<CINEMATIC_START>(
		[this]()
		{ 
			this->Set_Invisible(); 
		});

	m_pGameInstance->Subscribe<CINEMATIC_END>(
		[this]() 
		{ 
			this->Set_Visible(); 
		});
}

CUIJust_Image* CUIJust_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIJust_Image* pInstance = new CUIJust_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIJust_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIJust_Image::Clone(void* pArg)
{
	CUIJust_Image* pInstance = new CUIJust_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIJust_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIJust_Image::Free()
{
	Super::Free();
}
