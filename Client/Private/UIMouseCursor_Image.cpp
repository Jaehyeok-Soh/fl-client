#include "pch.h"
#include "UIMouseCursor_Image.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIMouseCursor_Image::CUIMouseCursor_Image(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUIMouseCursor_Image::CUIMouseCursor_Image(const CUIMouseCursor_Image& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUIMouseCursor_Image::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMouseCursor_Image::Initialize(void* pArg)
{
	MOUSE_CURSOR_IMAGE_DESC* pDesc = static_cast<MOUSE_CURSOR_IMAGE_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIMouseCursor_Image::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIMouseCursor_Image::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMouseCursor_Image::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	Tick_By_Type(fTimeDelta);
}

void CUIMouseCursor_Image::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIMouseCursor_Image::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMouseCursor_Image::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMouseCursor_Image::Ready_Components(MOUSE_CURSOR_IMAGE_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMouseCursor_Image::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMouseCursor_Image::Attach_Personal_Info()
{
	::ShowCursor(false);

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_DEFAULT:
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_CLICK:
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_TEXTEDIT:
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_GRAB:
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_HAND:
		break;
	default:
		break;
	}

	return S_OK;
}

void CUIMouseCursor_Image::Bind_Events()
{
}

void CUIMouseCursor_Image::Tick_By_Type(const _float fTimeDelta)
{
	if (KEY_BUTTON_HOLD(DIK_LALT))
	{
		m_isLAlt = true;

		if (MOUSE_LBUTTON_HOLD)
		{
			m_isLButton = true;
		}
		else
		{
			m_isLButton = false	;
		}
	}
	else
	{
		m_isLAlt = false;
	}

	if (!m_isLAlt)
	{
		m_pGameInstance->Request_CursorMode(ECursorMode::LockedHiddenCenter);
	}

	switch (m_eDImageSubClass)
	{
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_DEFAULT:
		if (m_isLAlt && !m_isLButton)
		{
			m_isVisible = true;
			m_fX = (_float)m_pGameInstance->Get_MousePos().x - (g_iWinSizeX * 0.5f);
			m_fY = -((g_iWinSizeY * 0.5f) - (_float)m_pGameInstance->Get_MousePos().y);
			m_pGameInstance->Request_CursorMode(ECursorMode::InVisibleClipped);
		}
		else
		{
			m_isVisible = false;
		}
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_CLICK:
		if (m_isLAlt && m_isLButton)
		{
			m_isVisible = true;
			m_fX = (_float)m_pGameInstance->Get_MousePos().x - (g_iWinSizeX * 0.5f);
			m_fY = -((g_iWinSizeY * 0.5f) - (_float)m_pGameInstance->Get_MousePos().y);
			m_pGameInstance->Request_CursorMode(ECursorMode::InVisibleClipped);
		}
		else
		{
			m_isVisible = false;
		}
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_TEXTEDIT:
		m_isVisible = false;
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_GRAB:
		m_isVisible = false;
		break;
	case DTO::EUIDImageSubClassType::MOUSE_CURSOR_HAND:
		m_isVisible = false;
		break;
	default:
		break;
	}
}

void CUIMouseCursor_Image::Initialize_Visible_Event()
{

}

_bool CUIMouseCursor_Image::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

void CUIMouseCursor_Image::Initialize_InVisible_Event()
{

}

_bool CUIMouseCursor_Image::Tick_InVisible_Event(const _float fTimeDelta)
{

	return true;
}

CUIMouseCursor_Image* CUIMouseCursor_Image::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMouseCursor_Image* pInstance = new CUIMouseCursor_Image(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMouseCursor_Image::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMouseCursor_Image::Clone(void* pArg)
{
	CUIMouseCursor_Image* pInstance = new CUIMouseCursor_Image(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMouseCursor_Image::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMouseCursor_Image::Free()
{
	Super::Free();
}
