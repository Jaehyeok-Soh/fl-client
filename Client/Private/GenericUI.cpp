#include "pch.h"
#include "GenericUI.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UIAction_Registry.h"
#include "GameInstance.h"

CGenericUI::CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CGenericUI::CGenericUI(const CGenericUI& rhs)
	:CUIObject(rhs)
{
}

HRESULT CGenericUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Initialize(void* pArg)
{
	GENERIC_UI_DESC* pDesc = static_cast<GENERIC_UI_DESC*>(pArg);
	
	m_eRectTransformType	= static_cast<ERectTransform>(pDesc->iRectTransformType);
	m_wstrTextureTag		= pDesc->wstrTextureTag;
	m_iTextureIndex			= pDesc->iTextureIndex;
	m_pParentCanvasCache	= pDesc->pCanvasCache;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGenericUI::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_iInteractState = static_cast<uint32_t>(DTO::EUIEvent_Flag::NONE);
	return S_OK;
}

void CGenericUI::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGenericUI::Update(const _float fTimeDelta)
{
	m_vRenderPos = Vec3{ m_vRectPos.x + m_vMoveOffset.x + m_fX, m_vRectPos.y + m_vMoveOffset.y + m_fY, m_fZ };
	Move_Position(m_vRenderPos.x, m_vRenderPos.y, m_vRenderPos.z);

	m_tRenderRect.left		= static_cast<LONG>(m_vRenderPos.x - (m_fWidth * 0.5f));
	m_tRenderRect.right		= static_cast<LONG>(m_vRenderPos.x + (m_fWidth * 0.5f));
	m_tRenderRect.top		= static_cast<LONG>(m_vRenderPos.y - (m_fHeight * 0.5f));
	m_tRenderRect.bottom	= static_cast<LONG>(m_vRenderPos.y + (m_fHeight * 0.5f));

	Super::Update(fTimeDelta);
}

void CGenericUI::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CGenericUI::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CGenericUI::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

_bool CGenericUI::Calc_HitEvent()
{
	if (::PtInRect(&m_tRenderRect, m_pGameInstance->Get_MousePos()))
		return TRUE;
	return FALSE;
}

void CGenericUI::Acting_By_InteractState()
{
	if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::INVOKED))
	{
	}

	if (m_iInteractState == DTO::EUIEvent_Flag::NONE)
	{

	}
	else
	{
		if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESS_ENTER))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESS_EXIT))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVER_ENTER))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVER_EXIT))
		{
		}

		if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESSING))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVERING))
		{
		}
	}
}

HRESULT CGenericUI::Ready_Components(GENERIC_UI_DESC* pDesc)
{
	return S_OK;
}

HRESULT CGenericUI::Bind_ShaderResources()
{
	return S_OK;
}

void CGenericUI::Free()
{
	Super::Free();
}
