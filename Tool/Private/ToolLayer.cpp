#include "pch.h"

#include "ToolLayer.h"
#include "Tool_Defines.h"
#include "GameInstance.h"
#include "ImGui_UIManager.h"
#include "Engine_Utils.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"

#include "ToolUI.h"

CToolLayer::CToolLayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext),
	m_pUIManager(CImGui_UIManager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

CToolLayer::CToolLayer(const CToolLayer& rhs)
	:CUIObject(rhs)
{
}

HRESULT CToolLayer::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CToolLayer::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	TOOLLAYER_DESC* pDesc = static_cast<TOOLLAYER_DESC*>(pArg);
	m_strTag = pDesc->strTag;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolLayer::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Set_SizeToTextureScale();
	return S_OK;
}

void CToolLayer::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CToolLayer::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CToolLayer::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CToolLayer::Ready_Before_Render(const _float fTimeDelta)
{
	Sync_Data();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CToolLayer::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolLayer::Ready_Components(TOOLLAYER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CToolLayer::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CToolLayer::Safe_Add_UI(CToolUI* pUI)
{
	if (nullptr == pUI)
		return E_FAIL;

	m_vecToolUIs.push_back(pUI);
	return S_OK;
}

vector<CToolUI*>* CToolLayer::Safe_Access_UIObject_Vector_Ptr()
{
	if (m_vecToolUIs.empty())
		return nullptr;

	return &m_vecToolUIs;
}

CToolUI* CToolLayer::Safe_Access_UIObject_Ptr(int32_t index)
{
	if (m_vecToolUIs.empty())
		return nullptr;

	int32_t NumLayer = static_cast<int32_t>(m_vecToolUIs.size());
	if (index >= NumLayer || index < 0)
		return nullptr;

	return m_vecToolUIs[index];
}

CToolUI* CToolLayer::Safe_Access_CurUIObject_Ptr()
{
	if (m_vecToolUIs.empty())
		return nullptr;

	int32_t NumLayer = static_cast<int32_t>(m_vecToolUIs.size());
	if (m_pUIManager->Get_CurLayerIndex() >= NumLayer || m_pUIManager->Get_CurLayerIndex() < 0)
		return nullptr;

	return m_vecToolUIs[m_pUIManager->Get_CurLayerIndex()];
}

void CToolLayer::Sync_Data()
{
	m_tLayerData.strTag = m_strTag;
}

CToolLayer* CToolLayer::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CToolLayer* pInstance = new CToolLayer(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CToolLayer::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CToolLayer::Clone(void* pArg)
{
	CToolLayer* pInstance = new CToolLayer(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CToolLayer::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolLayer::Free()
{
	Safe_Release(m_pUIManager);

	for (auto* p : m_vecToolUIs)
	{
		if (nullptr == p)
			continue;
		Safe_Release(p);
	}
	m_vecToolUIs.clear();
	Super::Free();
}

