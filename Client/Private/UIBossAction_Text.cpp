#include "pch.h"
#include "UIBossAction_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIBossAction_Text::CUIBossAction_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIBossAction_Text::CUIBossAction_Text(const CUIBossAction_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIBossAction_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Initialize(void* pArg)
{
	BOSS_ACTION_TEXT_DESC* pDesc = static_cast<BOSS_ACTION_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}


void CUIBossAction_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossAction_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIBossAction_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIBossAction_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	Tick_By_Type(fTimeDelta);

	if (FAILED(Convert_Stat_To_Text()))
		return;
}

HRESULT CUIBossAction_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Ready_Components(BOSS_ACTION_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossAction_Text::Attach_Personal_Info()
{
	return S_OK;
}

void CUIBossAction_Text::Tick_By_Type(const _float fTimeDelta)
{
}

HRESULT CUIBossAction_Text::Convert_Stat_To_Text()
{
	return S_OK;
}

void CUIBossAction_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIBossAction_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
}

void CUIBossAction_Text::Initialize_InVisible_Event()
{
}

_bool CUIBossAction_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_isActive = true;
	m_isFin_Event = true;
	return true;
}

_bool CUIBossAction_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUIBossAction_Text* CUIBossAction_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossAction_Text* pInstance = new CUIBossAction_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossAction_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossAction_Text::Clone(void* pArg)
{
	CUIBossAction_Text* pInstance = new CUIBossAction_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossAction_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossAction_Text::Free()
{
	Super::Free();
}
