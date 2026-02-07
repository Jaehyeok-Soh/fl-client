#include "pch.h"
#include "UI_Hierachy.h"
#include "ToolCanvas.h"
#include "ToolUI.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "GameInstance.h"

CUI_Hierachy::CUI_Hierachy(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pToolManager(CImGui_ToolManager::GetInstance())
{
	Safe_AddRef(m_pToolManager);
}

HRESULT CUI_Hierachy::Initialize_Prototype()
{
	return S_OK;
}

void CUI_Hierachy::Update(const _float fTimeDelta)
{

}

HRESULT CUI_Hierachy::Render(CToolObject* pGo)
{
	return S_OK;
}


CUI_Hierachy* CUI_Hierachy::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Hierachy* pInstance = new CUI_Hierachy(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Hierachy::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Hierachy::Free()
{
	Safe_Release(m_pToolManager);
	Super::Free();
}


