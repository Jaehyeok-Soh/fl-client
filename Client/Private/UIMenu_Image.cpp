#include "pch.h"
#include "UIMenu_Image.h"
#include "Client_Defines.h"
#include "CameraMan.h"

#include "MainPlayer.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
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

HRESULT CUIMenu_Image::Attach_Personal_Info()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MENU_BG:
	{

	}
	return S_OK;
	case DTO::EUIDImageSubClassType::MENU_ICON:
	{

	}
	return S_OK;
	case DTO::EUIDImageSubClassType::MENU_ICON_BG:
	{

	}
	return S_OK;
	case DTO::EUIDImageSubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIMenu_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	Attach_Personal_Info();
	return S_OK;
}

void CUIMenu_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMenu_Image::Update(const _float fTimeDelta)
{
	if (MOUSE_LBUTTON_HOLD)
	{
		m_fProgress_Ratio += fTimeDelta;
		if (m_fProgress_Ratio > 1.f)
		{
			m_fProgress_Ratio = 1.f;
		}
	}
	else if (MOUSE_RBUTTON_HOLD)
	{
		m_fProgress_Ratio -= fTimeDelta;
		if (m_fProgress_Ratio < 0.f)
		{
			m_fProgress_Ratio = 0.f;
		}
	}

	Super::Update(fTimeDelta);
}

void CUIMenu_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMenu_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMenu_Image::Render()
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

HRESULT CUIMenu_Image::Ready_Components(MENU_IMAGE_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIMenu_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	return S_OK;
}

void CUIMenu_Image::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (eEvent == ETriggerEventType::PRESS_ENTER)
	{
		if(!m_isVisible)
			Set_Visible();
		else 
			Set_Invisible();
	}
	else if (eEvent == ETriggerEventType::PRESS_EXIT)
	{
		Set_Invisible();
	}
}

void CUIMenu_Image::Initialize_Visible_Event()
{
	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MENU_BG:
	{
		m_fTimeAcc = 0.f;
		m_fProgress_Ratio = 1.f;
	}
		break;
	case DTO::EUIDImageSubClassType::MENU_ICON:
	case DTO::EUIDImageSubClassType::MENU_ICON_BG:
	{
		m_vOriginPos = Vec2{ m_vRenderPos.x-5.f, m_vRenderPos.y };
		m_vTargetPos = Vec2{ m_vRenderPos.x, m_vRenderPos.y};
		m_fTimeAcc = 0.f;
		m_fAlpha_Ratio = 0.f;
	}
		break;
	case DTO::EUIDImageSubClassType::END:
	default:
		break;
	}
}

void CUIMenu_Image::Initialize_InVisible_Event()
{
}

_bool CUIMenu_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	_float t = m_fTimeAcc / 0.5f;

	if (m_eDImageSubClass == DTO::EUIDImageSubClassType::MENU_BG)
	{
		if (t >= 1.f)
		{
			m_fProgress_Ratio = 0.f;			
			return true;
		}
		m_fProgress_Ratio = 1.f - t;
	}
	else
	{
		if (t >= 1.f)
		{
			m_fAlpha_Ratio = 1.f;
			m_fProgress_Ratio = 1.f;
			m_fX = m_vTargetPos.x;
			m_fY = m_vTargetPos.y;
			return true;
		}
		m_fAlpha_Ratio = t;
		const Vec2 vPos = m_vOriginPos + (m_vTargetPos - m_vOriginPos) * t;
		Move_Position(vPos.x, vPos.y, m_fZ);
	}
	return false;
}

_bool CUIMenu_Image::Tick_InVisible_Event(const _float fTimeDelta)
{
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
