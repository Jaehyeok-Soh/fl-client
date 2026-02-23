#include "pch.h"
#include "UIHover_Image.h"
#include "Client_Defines.h"
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

CUIHover_Image::CUIHover_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIHover_Image::CUIHover_Image(const CUIHover_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIHover_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIHover_Image::Initialize(void* pArg)
{
	HOVER_IMAGE_DESC* pDesc = static_cast<HOVER_IMAGE_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIHover_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::HOVER_POPUP_ICON:
	{

	}
	return S_OK;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_BG:
	{
		m_fOriginWidth = m_fWidth;
	}
	return S_OK;
	case DTO::EUIDImageSubClassType::HOVER_POPUP_TEXT:
	{

	}
	return S_OK;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIHover_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	Attach_Personal_Info();
	return S_OK;
}

void CUIHover_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIHover_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIHover_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIHover_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIHover_Image::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();
	return S_OK;
}

HRESULT CUIHover_Image::Ready_Components(HOVER_IMAGE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIHover_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	return S_OK;
}

void CUIHover_Image::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;

	if (eEvent == ETriggerEventType::HOVER_ENTER)
	{
		Set_Visible();
	}
	else if (eEvent == ETriggerEventType::HOVER_EXIT)
	{
		Set_Invisible();
	}
}

void CUIHover_Image::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::HOVER_POPUP_BG)
	{
		m_fWidth = 0.1f;
	}
	else
	{
		m_fTimeAcc = 0.f;
		m_fAlpha_Ratio = 0.f;
	}
}

void CUIHover_Image::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
}

_bool CUIHover_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::HOVER_POPUP_BG)
	{
		m_fWidth += fTimeDelta * 500.f;
		Set_Size(m_fWidth, m_fHeight);

		if (m_fWidth >= m_fOriginWidth)
		{
			m_fWidth = m_fOriginWidth;
			m_isFin_Event = true;
			return true;
		}
	}
	else
	{
		m_fTimeAcc += fTimeDelta;
		if (m_fTimeAcc < m_fDelay)
			return false;

		m_fAlpha_Ratio += fTimeDelta * 2.f;
		if (m_fAlpha_Ratio >= 1.f)
		{
			m_fAlpha_Ratio = 1.f;
			m_isFin_Event = true;
			return true;
		}
	}
	return false;
}

_bool CUIHover_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	return true;
}

CUIHover_Image* CUIHover_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIHover_Image* pInstance = new CUIHover_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIHover_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIHover_Image::Clone(void* pArg)
{
	CUIHover_Image* pInstance = new CUIHover_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIHover_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIHover_Image::Free()
{
	Super::Free();
}
