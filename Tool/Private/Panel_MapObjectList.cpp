#include "pch.h"
#include "Panel_MapObjectList.h"
#include "StaticModel.h"
#include "Engine_Utils.h"
#include "ImGui_Layout_Transform.h"
#include "Level_Map.h"
#include "CameraMan.h"
#include "Camera.h"
#include "Panel_FileExplore.h"

USING(Tool)

CPanel_MapObjectList::CPanel_MapObjectList(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), m_pGameInstance(CGameInstance::GetInstance()), m_pTransformLayout(nullptr), m_pCamera(nullptr)
	, m_pCameraCom(nullptr), m_wszMapObjectLayerTag{}
{

	m_pTransformLayout = CImGui_Layout_Transform::Create("Layout_Transform", m_pDevice, m_pDeviceContext);

	Safe_AddRef(m_pGameInstance);

	m_pCamera = static_cast<Engine::CCameraMan*>(m_pGameInstance->Get_MainCamera());

	m_pCameraCom = m_pCamera->Get_Component<CCamera>();

	m_arrayMapObjectList.fill(nullptr);
}


HRESULT CPanel_MapObjectList::Initialize()
{
	if (FAILED(Ready_LayerTag()))
		return E_FAIL;

	if (FAILED(Update_MapObjectList()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPanel_MapObjectList::Ready_LayerTag()
{
	lstrcpyW(m_wszMapObjectLayerTag[ENUM_TO_UINT(EMapObject_Type::STATICMODEL)],g_wszStaticModelLayer);
	return S_OK;
}

HRESULT CPanel_MapObjectList::Render(CToolObject* pGo)
{
	if (FAILED(Render_CamInfo()))
		return E_FAIL;

	ImGui::Separator();


	if (FAILED(Render_MapObjectList()))
		return E_FAIL;

	ImGui::Separator();


	if (FAILED(Render_SelectInfo()))
		return E_FAIL;
	

	return S_OK;
}

void CPanel_MapObjectList::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);


	m_pSelectMapObject = static_cast<CMapObject*>(static_cast<CLevel_Map*>(m_pOwnerLevel)->Get_SelectToolObject());

}

HRESULT CPanel_MapObjectList::Update_MapObjectList()
{
	for (_uint i = 0; i < ENUM_TO_UINT(EMapObject_Type::END); ++i)
	{
		m_arrayMapObjectList[i] = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP), m_wszMapObjectLayerTag[i]);
	}

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_MapObjectList()
{
	ImGui::Begin(m_strLabel.c_str());

	if(FAILED(Update_MapObjectList()))
		return E_FAIL;


	ImGui::SeparatorText(" Function ");

	if (ImGui::Button("Clear"))
	{
		m_pSelectMapObject = nullptr;
		static_cast<CLevel_Map*>(this->m_pOwnerLevel)->Set_SelectToolObject(nullptr);
		m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::MAP), g_wszStaticModelLayer);
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("List"))
	{
		list<CGameObject*>* pListStaticModelLayer = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::MAP), g_wszStaticModelLayer);

		if (!pListStaticModelLayer || pListStaticModelLayer->empty())
		{
			ImGui::Text(" Empty Layer");
		}
		else
		{
			UINT32 iIndex = 0;
			for (auto& StaticMesh : *pListStaticModelLayer)
			{
				if (StaticMesh)
				{
					CStaticModel* pStaticModel = static_cast<CStaticModel*>(StaticMesh);

					if (StaticMesh == m_pSelectMapObject)
						ImGui::TextColored(ImVec4(1.0, 0.f, 0.f, 1.f), pStaticModel->Get_Name().c_str());
					else
						ImGui::Text(pStaticModel->Get_Name().c_str());
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
						static_cast<CLevel_Map*>(m_pOwnerLevel)->On_ChangeSelectedObject(pStaticModel);
					else if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
							m_pCamera->Get_Component<CTransform>()->Set_Info(TRANSFORM_INFO_STATE::POS, pStaticModel->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS));
				}
			}
		}
	}

	ImGui::End();

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_CamInfo()
{
	ImGui::Begin("Camera Info");


	if (ImGui::TreeNode(" Camera S R T "))
	{
		CTransform* pTransfrom = m_pCamera->Get_Component<CTransform>();

		if (!pTransfrom) return E_FAIL;

		Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();

		Vec3 vLook = pTransfrom->Get_Info(TRANSFORM_INFO_STATE::LOOK);
		vLook.Normalize();

		Vec3 vPosition = WorldMatrix.Translation();
		Vec3 vRotation = WorldMatrix.ToEuler();
		Vec3 vScale = pTransfrom->Get_Scaled();


		if (ImGui::InputFloat3("Position", &vPosition.x))
			pTransfrom->Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
		ImGui::InputFloat3("Rotation", &vRotation.x);
		if (ImGui::InputFloat3("Scale", &vScale.x))
			pTransfrom->Set_Scale(vScale);

		ImGui::Text(" Look [ %.2f , %.2f , %.2f ] ", vLook.x, vLook.y, vLook.z);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" Camera Setting "))
	{
		float fFov = m_pCameraCom->Get_Fov() * To_DEGREE;
		if (ImGui::DragFloat("Fov#Fov", &fFov))
			m_pCameraCom->Set_Fov(fFov * TO_RAD);
		float fFar = m_pCameraCom->Get_Far();
		if (ImGui::DragFloat("Far#Far", &fFar))
			m_pCameraCom->Set_Fov(fFar);
		ImGui::TreePop();
	}
	ImGui::End();

	return S_OK;
}

HRESULT CPanel_MapObjectList::Render_SelectInfo()
{

	ImGui::Begin(" Select Info ");

	if (m_pSelectMapObject == nullptr)
		ImGui::Text(" Select Map Object is Empty ");
	else
	{
		m_pTransformLayout->Render(m_pSelectMapObject);
		m_pSelectMapObject->Draw_ImGui();	
	}

	ImGui::End();

	return S_OK;
}

CPanel_MapObjectList* CPanel_MapObjectList::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_MapObjectList* pPanel = new CPanel_MapObjectList(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pPanel->Initialize()))
	{
		Safe_Release(pPanel);
		MSG_BOX(" MapObjec List Panel Is Failed To Create");
		return nullptr;
	}

	return  pPanel;
}

void CPanel_MapObjectList::Free()
{
	Super::Free();


	Safe_Release(m_pTransformLayout);
	Safe_Release(m_pGameInstance);
}
