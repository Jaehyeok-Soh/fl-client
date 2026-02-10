#include "pch.h"
#include "UITrigger.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUITrigger::CUITrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUITrigger::CUITrigger(const CUITrigger& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUITrigger::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUITrigger::Initialize(void* pArg)
{
	UI_TRIGGER_DESC* pDesc = static_cast<UI_TRIGGER_DESC*>(pArg);
	m_eOwnerType = pDesc->eOwner;
	m_tTriggerData = std::move(pDesc->tTriggerData);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUITrigger::Attach_Personal_Info()
{
	switch (m_eOwnerType)
	{
	case DTO::EUIOwnerType::NONE_OWNER:
		return S_OK;
	case DTO::EUIOwnerType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUITrigger::Bind_Cache()
{
	for (const _string& str : m_tTriggerData.vecHoverEnterTriggerCanvas)
	{
		auto* pCanvas = m_pUIManager->Find_Canvas(m_iLevelID, str);
		if (nullptr == pCanvas)
			return E_FAIL;
		
	}
	for(const _string& str : m_tTriggerData.vecHoverExitTriggerCanvas)
	for(const _string& str : m_tTriggerData.vecPressEnterTriggerCanvas)
	for(const _string& str : m_tTriggerData.vecPressExitTriggerCanvas)

	m_tTriggerData.vecHoverEnterTriggerUI;
	m_tTriggerData.vecHoverExitTriggerUI;
	m_tTriggerData.vecPressEnterTriggerUI;
	m_tTriggerData.vecPressExitTriggerUI;

	return S_OK;
}

HRESULT CUITrigger::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Bind_Cache()))
		return E_FAIL;

	return S_OK;
}

void CUITrigger::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUITrigger::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUITrigger::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUITrigger::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUITrigger::Render()
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

HRESULT CUITrigger::Ready_Components(UI_TRIGGER_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUITrigger::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();

	return S_OK;
}

CUITrigger* CUITrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUITrigger* pInstance = new CUITrigger(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUITrigger::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUITrigger::Clone(void* pArg)
{
	CUITrigger* pInstance = new CUITrigger(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUITrigger::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUITrigger::Free()
{
	Super::Free();
}
