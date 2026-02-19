#include "pch.h"
#include "UIPlayerStat_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
#include "GameInstance.h"

CUIPlayerStat_Text::CUIPlayerStat_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIPlayerStat_Text::CUIPlayerStat_Text(const CUIPlayerStat_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIPlayerStat_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Initialize(void* pArg)
{
	PLAYER_STAT_DESC* pDesc = static_cast<PLAYER_STAT_DESC*>(pArg);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIPlayerStat_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIPlayerStat_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIPlayerStat_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPlayerStat_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPlayerStat_Text::Render()
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

	if (FAILED(m_pGameInstance->Draw_Text(m_wstrFontTag, m_wstrText.c_str(), m_vFontPos, m_vFontColor, m_ePivot, m_fFontRotate, m_fFontScale)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Ready_Components(PLAYER_STAT_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIPlayerStat_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	return S_OK;
}

void CUIPlayerStat_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
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
}

void CUIPlayerStat_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	m_vFontColor = Vec4{ 0.f ,0.f ,0.f ,0.f };
	m_fTimeAcc = 0.f;
}

void CUIPlayerStat_Text::Initialize_InVisible_Event()
{
}

_bool CUIPlayerStat_Text::Tick_Visible_Event(const _float fTimeDelta)
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
		m_isActive = true;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

_bool CUIPlayerStat_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIPlayerStat_Text* CUIPlayerStat_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIPlayerStat_Text* pInstance = new CUIPlayerStat_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIPlayerStat_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIPlayerStat_Text::Clone(void* pArg)
{
	CUIPlayerStat_Text* pInstance = new CUIPlayerStat_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIPlayerStat_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIPlayerStat_Text::Free()
{
	Super::Free();
}
