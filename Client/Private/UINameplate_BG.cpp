#include "pch.h"
#include "UINameplate_BG.h"
#include "Client_Defines.h"
//=================
// Component
//=================
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUINameplate_BG::CUINameplate_BG(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIDynamic_Image(pDevice, pDeviceContext)
{
}

CUINameplate_BG::CUINameplate_BG(const CUINameplate_BG& rhs)
	:CUIDynamic_Image(rhs)
{
}

HRESULT CUINameplate_BG::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINameplate_BG::Initialize(void* pArg)
{
	NAMEPLATE_BG_DESC* pDesc = static_cast<NAMEPLATE_BG_DESC*>(pArg);
	m_isInteract = true;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINameplate_BG::Attach_Personal_Info()
{
	return S_OK;
}

HRESULT CUINameplate_BG::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUINameplate_BG::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUINameplate_BG::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUINameplate_BG::Update_Late(const _float fTimeDelta)
{
 	Super::Update_Late(fTimeDelta);
}

void CUINameplate_BG::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUINameplate_BG::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUINameplate_BG::Ready_Components(NAMEPLATE_BG_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUINameplate_BG::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

void CUINameplate_BG::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUINameplate_BG::Initialize_Visible_Event()
{
	m_isFin_Event = false;
	m_isActive = false;
}

_bool CUINameplate_BG::Tick_Visible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUINameplate_BG::Spawn_FromPool(void* pArg)
{
	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	
	auto* pComp = Get_Script_Component(L"WorldUIComponent");
	if (nullptr == pComp)
		return E_FAIL;

	m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
	if (nullptr == m_pWorldUIComp)
		return E_FAIL;

	m_pWorldUIComp->Set_Target(pDesc->pTarget);
	/* ¸ó½ºÅÍ ½ºÅÈ ÄÄÆ÷³ÍÆ® ºÎÂø */

	return S_OK;
}

HRESULT CUINameplate_BG::Despawn_FromPool()
{
	return S_OK;
}

CUINameplate_BG* CUINameplate_BG::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUINameplate_BG* pInstance = new CUINameplate_BG(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUINameplate_BG::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUINameplate_BG::Clone(void* pArg)
{
	CUINameplate_BG* pInstance = new CUINameplate_BG(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUINameplate_BG::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUINameplate_BG::Free()
{
	Super::Free();
}
