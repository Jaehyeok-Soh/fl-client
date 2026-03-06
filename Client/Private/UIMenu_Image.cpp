#include "pch.h"
#include "UIMenu_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
#include "CameraMan.h"

#include "MainPlayer.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUIMenu_Image::CUIMenu_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMenu_Image::CUIMenu_Image(const CUIMenu_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMenu_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Image::Initialize(void* pArg)
{
	MENU_IMAGE_DESC* pDesc = static_cast<MENU_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}


HRESULT CUIMenu_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUIMenu_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMenu_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIMenu_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMenu_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMenu_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Image::Ready_Components(MENU_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMenu_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MENU_BG:
	{

	}
	break;
	case DTO::EUIDImageSubClassType::MENU_ICON:
	{
		m_pGameInstance->Subscribe<BOSS_STAGING_EVENT_START>([this]() { this->Set_Invisible(); });
		m_pGameInstance->Subscribe<BOSS_STAGING_EVENT_END>([this]() { this->Set_Visible(); });
	}
	break;
	case DTO::EUIDImageSubClassType::MENU_ICON_BG:
	{
	}
	break;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}
	return S_OK;
}

void CUIMenu_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MENU_BG:
	{
		//m_fProgressTimeAcc	= 0.f;
		//m_fProgress_Ratio		= 1.f;
		Ready_LerpChange(1.f, 1.f, 0.f, 1.f, m_fDelay);
	}
	break;
	case DTO::EUIDImageSubClassType::MENU_ICON:
	{
		Ready_Fade(0.5f, 1.f, 0.f, 0.f);
	}
	break;
	case DTO::EUIDImageSubClassType::MENU_ICON_BG:
	{
		Ready_Fade(0.5f, 1.f, 0.f, 0.f);
	}
	break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}
}

void CUIMenu_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MENU_BG:
	{
		//m_fProgressTimeAcc = 0.f;
		//m_fProgress_Ratio = 0.f;
		Ready_LerpChange(1.f, 0.f, 1.f, 1.f, m_fDelay);
	}
	break;
	case DTO::EUIDImageSubClassType::MENU_ICON:
	{
		Ready_Fade(0.5f, 1.f, 0.f, 0.f);
	}
	break;
	case DTO::EUIDImageSubClassType::MENU_ICON_BG:
	{
		Ready_Fade(0.5f, 1.f, 0.f, 0.f);
	}
	break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}
}

_bool CUIMenu_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MENU_BG)
	{
		//m_fTimeAcc += fTimeDelta;
		//_float t = m_fTimeAcc / 1.f;

		//if (t >= 1.f)
		//{
		//	m_fProgress_Ratio = 0.f;	
		//	m_isFin_Event = true;
		//	m_isActive = true;
		//	return true;
		//}
		//m_fProgress_Ratio = 1.f - t;

		const _bool isLerp = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		if (isLerp)
		{
			m_isFin_Event = true;
			m_isActive = true;
			return true;
		}
	}
	else
	{
		const _bool isFade = Tick_Fade(fTimeDelta);
		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;
			return true;
		}
	}
	return false;
}

_bool CUIMenu_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MENU_BG)
	{
		//m_fTimeAcc += fTimeDelta;
		//_float t = m_fTimeAcc / 1.f;
		//if (t >= 1.f)
		//{
		//	m_fProgress_Ratio = 1.f;
		//	m_isFin_Event = true;
		//	m_isActive = true;
		//	return true;
		//}
		//m_fProgress_Ratio = t;
		
		const _bool isLerp = Tick_LerpChange(&m_fProgress_Ratio, fTimeDelta);
		if (isLerp)
		{
			m_isFin_Event = true;
			m_isActive = true;
			return true;
		}
	}
	else
	{
		const _bool isFade = Tick_Fade(fTimeDelta);
		if (isFade)
		{
			m_isFin_Event = true;
			m_isActive = true;
			return true;
		}
	}
	return false;
}
CUIMenu_Image* CUIMenu_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMenu_Image* pInstance = new CUIMenu_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMenu_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMenu_Image::Clone(void* pArg)
{
	CUIMenu_Image* pInstance = new CUIMenu_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMenu_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMenu_Image::Free()
{
	Super::Free();
}
