#include "pch.h"
#include "UIText.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
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
	m_pTargetStat	= pDesc->pTargetStat;
	m_eSubClassType	= pDesc->eOwner;
	m_wstrFontTag	= pDesc->wstrFontTag;
	m_wstrText		= pDesc->wstrText;
	m_vFontColor	= pDesc->vFontColor;
	m_fFontRotate	= pDesc->fRotate;
	m_fFontScale	= pDesc->fScale;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	m_vFontPos = Vec2{ m_fX, m_fY };
	return S_OK;
}

HRESULT CUIText::Attach_Personal_Info()
{
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::NONE_OWNER:
		return S_OK;
	case DTO::EUISubClassType::PLAYER_HP:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUISubClassType::PLAYER_LV:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUISubClassType::PLAYER_ENERGY:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUISubClassType::PLAYER_ARMOR:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}

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
	m_vFontPos.x = m_vRenderPos.x;
	m_vFontPos.y = m_vRenderPos.y;

	Super::Update_Late(fTimeDelta);
}

void CUIText::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIText::Render()
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

	if (FAILED(m_pGameInstance->Draw_Text(m_wstrFontTag, m_wstrText.c_str(), m_vFontPos, m_vFontColor, m_fFontRotate, m_fFontScale)))
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

	Super::Bind_ShaderResources();

	return S_OK;
}

void CUIText::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (eEvent == ETriggerEventType::HOVER_ENTER)
	{
		Set_Visible();
	}
	else if (eEvent == ETriggerEventType::HOVER_EXIT)
	{
		Set_Invisible();
	}
}

void CUIText::Initialize_Visible_Event()
{
	m_vFontColor = Vec4{ 0.f ,0.f ,0.f ,0.f };
	m_fTimeAcc = 0.f;
}

void CUIText::Initialize_InVisible_Event()
{
}

_bool CUIText::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc < m_fDelay)
		return false;

	m_vFontColor.x += fTimeDelta * 2.f;
	m_vFontColor.y += fTimeDelta * 2.f;
	m_vFontColor.z += fTimeDelta * 2.f;
	m_vFontColor.w += fTimeDelta * 2.f;
	if (m_vFontColor.w > 1.f)
	{
		m_vFontColor.w = 1.f;
		return true;
	}
	return false;
}

_bool CUIText::Tick_InVisible_Event(const _float fTimeDelta)
{
	return _bool();
}

CUIText* CUIText::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIText* pInstance = new CUIText(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIText::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIText::Clone(void* pArg)
{
	CUIText* pInstance = new CUIText(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIText::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIText::Free()
{
	Super::Free();
}
