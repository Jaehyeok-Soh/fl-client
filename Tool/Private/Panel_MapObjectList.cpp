#include "Panel_MapObjectList.h"

USING(Tool)

CPanel_MapObjectList::CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel,pOwner,pDevice,pDeviceContext)
{
}

HRESULT CPanel_MapObjectList::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());

	if (ImGui::CollapsingHeader("Test"))
	{
		ImGui::Text("Believe GangBuengJun");
	}



	ImGui::End();

	return S_OK;
}

void CPanel_MapObjectList::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

CPanel_MapObjectList* CPanel_MapObjectList::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return  new CPanel_MapObjectList(pLabel, pOwner, pDevice, pDeviceContext);
}

void CPanel_MapObjectList::Free()
{
	Super::Free();


}
