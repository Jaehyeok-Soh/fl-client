#include "pch.h"
#include "UIComponent_Inspector.h"
#include "ToolCanvas.h"
#include "ToolUI.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "GameInstance.h"

CUIComponent_Inspector::CUIComponent_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pUIManager(CImGui_UIManager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CUIComponent_Inspector::Initialize_Prototype()
{
	return S_OK;
}

void CUIComponent_Inspector::Update(const _float fTimeDelta)
{

}

HRESULT CUIComponent_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);

	m_pSelectedUI = m_pUIManager->Safe_Access_UI(m_pUIManager->Get_CurUIIndex());
	if (nullptr != m_pSelectedUI)
	{
	
	}

	ImGui::End();
	return S_OK;
}

void CUIComponent_Inspector::Edit_ImageComponent()
{
	ImGui::Text("Source Image");
}

void CUIComponent_Inspector::Edit_ButtonComponent()
{
	ImGui::Text("Hovered Texture");
	ImGui::Text("Pressed Texture");
	ImGui::Text("Selected Texture");
}

CUIComponent_Inspector* CUIComponent_Inspector::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIComponent_Inspector* pInstance = new CUIComponent_Inspector(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIComponent_Inspector::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUIComponent_Inspector::Free()
{
	Safe_Release(m_pUIManager);
	Super::Free();
}


