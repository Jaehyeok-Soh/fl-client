#include "UI_Inspector.h"

CUI_Inspector::CUI_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext)
{
}

HRESULT CUI_Inspector::Initialize_Prototype()
{
	return S_OK;
}

void CUI_Inspector::Update(const _float fTimeDelta)
{
	int a = 0;
}

HRESULT CUI_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());
	
	ImGui::Text("Hello World");

	ImGui::End();
	return S_OK;
}

CUI_Inspector* CUI_Inspector::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Inspector* pInstance = new CUI_Inspector(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Inspector::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Inspector::Free()
{
	Super::Free();
}


