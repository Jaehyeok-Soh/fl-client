#include "Panel_MapObjectList.h"
#include "StaticModel.h"
#include "Engine_Utils.h"

USING(Tool)

CPanel_MapObjectList::CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel,pOwner,pDevice,pDeviceContext) , m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CPanel_MapObjectList::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());

	if (ImGui::CollapsingHeader("List"))
	{
		list<CGameObject*>* pListStaticModelLayer =  m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP) , g_wszStaticModelLayer);

		if (!pListStaticModelLayer || pListStaticModelLayer->empty())
		{
			ImGui::Text(" Empty Layer");
		}
		else
		{
			char szTreeNode[MAX_PATH];
			UINT32 iIndex = 0;
			for (auto& StaticMesh : *pListStaticModelLayer)
			{
				if (StaticMesh)
				{
					CStaticModel* pStaticModel = static_cast<CStaticModel*>(StaticMesh);
					
					ImGui::Text(Engine_Utils::ToString(pStaticModel->Get_ModelName()).c_str());
					if (ImGui::IsItemClicked())
					{
						m_pSelectMapObject = pStaticModel;
					}

				}
			}
		}
	}

	ImGui::End();



	ImGui::Begin(" Select Info ");


	if (m_pSelectMapObject == nullptr)
		ImGui::Text(" Select Map Object is Empty ");
	else
		m_pSelectMapObject->Draw_ImGui();

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

	Safe_Release(m_pGameInstance);

}
