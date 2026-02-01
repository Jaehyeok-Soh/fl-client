#include "pch.h"
#include "Panel_MapTool.h"
#include "GameInstance.h"
#include "MapToolManager.h"
#include "MapObject.h"
#include "Camera.h"
#include "CameraMan.h"

CPanel_MapTool::CPanel_MapTool(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel,pOwner , pDevice, pDeviceContext)
{
}

HRESULT CPanel_MapTool::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	m_pMapToolManager = CMapToolManager::GetInstance();


	m_pCameraMan = m_pGameInstance->Get_MainCamera();
	m_pCamera = m_pCameraMan->Get_Component<CCamera>();

	if (!m_pCamera) return E_FAIL;
	if (!m_pCameraMan) return E_FAIL;

	Safe_AddRef(m_pMapToolManager);
	Safe_AddRef(m_pGameInstance);

	return S_OK;
}

HRESULT CPanel_MapTool::Render(CToolObject* pGo)
{
	ImGui::Begin(" Map Tool Window ");

	if (ImGui::CollapsingHeader(" Camera Setting "))
	{
		if (FAILED(Render_CameraSetting()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}
	if (ImGui::CollapsingHeader(" Ray Setting "))
	{
		if (FAILED(Render_RaySetting()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
	}

	ImGui::End();

	Render_PreViewInfo();

	return S_OK;
}


HRESULT CPanel_MapTool::Render_RaySetting()
{
	ImGui::NewLine();
	ImGui::DragFloat(" Ray Range ", &m_pMapToolManager->m_fMouseRange, 0.1f);
	ImGui::NewLine();
	ImGui::DragFloat(" Mouse Wheel Speed ", &m_pMapToolManager->m_fMouseWheelSpeed, 0.1f);

	return S_OK;
}

HRESULT CPanel_MapTool::Render_CameraSetting()
{
	if (ImGui::TreeNode(" S R T "))
	{
		CTransform* pTransfrom = m_pCameraMan->Get_Component<CTransform>();

		if (!pTransfrom) return E_FAIL;

		Matrix WorldMatrix = pTransfrom->Get_WorldMatrix();

		Vec3 vLook = pTransfrom->Get_Info(TRANSFORM_INFO_STATE::LOOK);
		vLook.Normalize();

		Vec3 vPosition = WorldMatrix.Translation();
		Vec3 vRotation = WorldMatrix.ToEuler();
		Vec3 vScale = pTransfrom->Get_Scaled();


		if (ImGui::DragFloat3("Position", &vPosition.x))
			pTransfrom->Set_Info(TRANSFORM_INFO_STATE::POS, vPosition);
		ImGui::DragFloat3("Rotation", &vRotation.x);
		if (ImGui::DragFloat3("Scale", &vScale.x))
			pTransfrom->Set_Scale(vScale);

		ImGui::Text(" Look [ %.2f , %.2f , %.2f ] ", vLook.x, vLook.y, vLook.z);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode(" Info Setting "))
	{
		float fFov = m_pCamera->Get_Fov() * To_DEGREE;
		if (ImGui::DragFloat("Fov#Fov", &fFov))
			m_pCamera->Set_Fov(fFov * TO_RAD);
		float fFar = m_pCamera->Get_Far();
		if (ImGui::DragFloat("Far#Far", &fFar))
			m_pCamera->Set_Fov(fFar);

		CTransform* pTransform = m_pCameraMan->Get_Component<CTransform>();

		float fMoveSpeed = pTransform->Get_MovePerSec();
		float fMoveTurnSpeed = pTransform->Get_RotatePerSec();

		if (ImGui::DragFloat(" Move Speed ", &fMoveSpeed, 0.1f))
			pTransform->Set_MovePerSec(fMoveSpeed);

		if (ImGui::DragFloat(" Rotation Speed ", &fMoveSpeed, 0.1f))
			pTransform->Set_RotatePerSec(fMoveTurnSpeed);



		ImGui::TreePop();
	}


	return S_OK;
}

HRESULT CPanel_MapTool::Render_PreViewInfo()
{
	ImGui::Begin(" Preview Info ");

	if (m_pMapToolManager->m_pPreviewMapobject == nullptr)
	{
		ImGui::Text(" Preview Info is Empty ");
		ImGui::End();
		return S_OK;
	}
	
	ImGui::Separator();

	if (ImGui::Button(" Delete "))
		m_pMapToolManager->Delete_Preview();

	ImGui::Separator();


	m_pMapToolManager->DrawImGui_Preview();

	ImGui::End();

	return S_OK;
}

void CPanel_MapTool::Update(const _float fTimeDelta)
{


	return;
}


CPanel_MapTool* CPanel_MapTool::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_MapTool* pPanel = new CPanel_MapTool(pLabel,pOwner,pDevice,pDeviceContext);
	if (FAILED(pPanel->Initialize()))
	{
		Safe_Release(pPanel);
		MSG_BOX(" Panel MapTool is failed to Create ");
		return nullptr;
	}
	return pPanel;
}

void CPanel_MapTool::Free()
{
	Super::Free();

	Safe_Release(m_pMapToolManager);
	Safe_Release(m_pGameInstance);
}