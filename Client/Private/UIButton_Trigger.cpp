#include "pch.h"
#include "UIButton_Trigger.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIButton_Trigger::CUIButton_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIButton_Trigger::CUIButton_Trigger(const CUIButton_Trigger& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIButton_Trigger::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIButton_Trigger::Initialize(void* pArg)
{
	UI_TRIGGER_DESC* pDesc = static_cast<UI_TRIGGER_DESC*>(pArg);
	m_eSubClassType = pDesc->eOwner;
	m_tTriggerData = std::move(pDesc->tTriggerData);

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIButton_Trigger::Attach_Personal_Info()
{
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::NONE_OWNER:
		return S_OK;
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIButton_Trigger::Bind_Cache()
{
	for (const _string& str : m_tTriggerData.vecHoverEnterTriggerCanvas)
	{
		auto* pCanvas = m_pUIManager->Find_Canvas(m_iLevelID, str);
		if (nullptr == pCanvas)
			return E_FAIL;

	}
	for (const _string& str : m_tTriggerData.vecHoverExitTriggerCanvas)
		for (const _string& str : m_tTriggerData.vecPressEnterTriggerCanvas)
			for (const _string& str : m_tTriggerData.vecPressExitTriggerCanvas)

				m_tTriggerData.vecHoverEnterTriggerUI;
	m_tTriggerData.vecHoverExitTriggerUI;
	m_tTriggerData.vecPressEnterTriggerUI;
	m_tTriggerData.vecPressExitTriggerUI;

	return S_OK;
}

HRESULT CUIButton_Trigger::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Bind_Cache()))
		return E_FAIL;

	return S_OK;
}

void CUIButton_Trigger::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIButton_Trigger::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIButton_Trigger::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIButton_Trigger::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIButton_Trigger::Render()
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

HRESULT CUIButton_Trigger::Ready_Components(UI_TRIGGER_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIButton_Trigger::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();

	return S_OK;
}

CUIButton_Trigger* CUIButton_Trigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIButton_Trigger* pInstance = new CUIButton_Trigger(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIButton_Trigger::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIButton_Trigger::Clone(void* pArg)
{
	CUIButton_Trigger* pInstance = new CUIButton_Trigger(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIButton_Trigger::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIButton_Trigger::Free()
{
	Super::Free();
}
