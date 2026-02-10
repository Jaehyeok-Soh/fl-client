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

	if (ImGui::CollapsingHeader(" Map Tool Setting "))
	{
		if (FAILED(Render_CheckAndBind()))
		{
			ImGui::TreePop();
			return E_FAIL;
		}
		if (FAILED(Render_MakeMapObjectSetting()))
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

HRESULT CPanel_MapTool::Render_CheckAndBind()
{
	ImGui::SeparatorText(" Chekc Static & Instance Model  Merget InstanceModel ");

	/* UE Model Data 전용 */
	if (ImGui::Button(" Bind Staitc & Instance Model To Instance Model "))
		m_pMapToolManager->Check_And_Bind_FromUE();


	ImGui::Separator();
	return S_OK;
}

HRESULT CPanel_MapTool::Render_MakeMapObjectSetting()
{

	ImGui::SeparatorText(" Map Object Batch Mode ");

#pragma region Batch Mode

	ImGui::NewLine();

	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MapToolObjectBatchMode());
	m_strBuffer = MapToolObjectBatchMode_ToString(static_cast<EMapToolObjectBatchMode>(m_iBuffer));
	if (ImGui::BeginCombo("##MapObjectBatchMode", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EMapToolObjectBatchMode::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(MapToolObjectBatchMode_ToString(static_cast<EMapToolObjectBatchMode>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MapToolObjectBatchMode(static_cast<EMapToolObjectBatchMode>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}


	if (ImGui::TreeNode(" Brush Option Setting "))
	{
		m_pMapToolManager->m_tBrushModeOption.Render_ImGui();
		ImGui::TreePop();
	}

	ImGui::NewLine();

	ImGui::Separator();

#pragma endregion

	ImGui::SeparatorText("Make Map Object Setting");

	ImGui::NewLine();

	ImGui::SeparatorText(" Level Type ");

#pragma region Draw Type

	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MakeMapObejctClientLevelType());
	m_strBuffer = ClientleveltypeToString(static_cast<EClientLevelType>(m_iBuffer));
	if (ImGui::BeginCombo("##ClientLevelType", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EClientLevelType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(ClientleveltypeToString(static_cast<EClientLevelType>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MakeMapObjectClientLevelType(static_cast<EClientLevelType>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

#pragma endregion

	ImGui::Separator();


	ImGui::SeparatorText(" Draw Type ");

#pragma region Draw Type

	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MakeMapObjectDrawType());
	m_strBuffer = EMapObject_DrawType_ToString(static_cast<EMapObject_DrawType>(m_iBuffer));
	if (ImGui::BeginCombo("##DrawType", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EMapObject_DrawType::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(EMapObject_DrawType_ToString(static_cast<EMapObject_DrawType>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MakeMapObjectDrawType(static_cast<EMapObject_DrawType>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}


#pragma endregion

	ImGui::Separator();

	ImGui::SeparatorText(" Cliet Make Path Setting ");

#pragma region Client Make Path Combo
	m_iBuffer = static_cast<_int>(m_pMapToolManager->Get_MakeMapObjectClientMakePath());
	m_strBuffer = ClientMakePath_ToString(static_cast<EClientMakePath>(m_iBuffer));
	if (ImGui::BeginCombo("##ClientMakePath", m_strBuffer.c_str()))
	{
		for (_int i = 0; i < static_cast<_uint>(EClientMakePath::END); ++i)
		{
			bool isSelected = i == m_iBuffer;
			if (ImGui::Selectable(ClientMakePath_ToString(static_cast<EClientMakePath>(i)).c_str(), &isSelected))
				m_pMapToolManager->Set_MakeMapObjectClientMakePath(static_cast<EClientMakePath>(i));
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
#pragma endregion

	ImGui::Separator();


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
			m_pCamera->Set_Far(fFar);

		CTransform* pTransform = m_pCameraMan->Get_Component<CTransform>();

		float fMoveSpeed = pTransform->Get_MovePerSec();
		float fMoveTurnSpeed = pTransform->Get_RotatePerSec();

		if (ImGui::DragFloat(" Move Speed ", &fMoveSpeed, 0.1f))
			pTransform->Set_MovePerSec(fMoveSpeed);

		if (ImGui::DragFloat(" Rotation Speed ", &fMoveTurnSpeed, 0.1f))
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
	{
		m_pMapToolManager->Delete_Preview();
		ImGui::End();
		return S_OK;
	}

	ImGui::Separator();

	ImGui::NewLine();

	ImGui::SeparatorText(" Model Info ");

	ImGui::Text( " Model Name => [ %s ] " , m_pMapToolManager->m_pPreviewMapobject->Get_ModelFileName().c_str());

	ImGui::Separator();

	Vec3 vScale,vPosition;
	Quat vQuat{};

	m_pMapToolManager->m_pPreviewMapobject->Get_SRT(vScale, vQuat,vPosition);


	if (ImGui::BeginTable("SRT_Quat_Display", 5, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg))
	{
		// 헤더 설정
		ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
		ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Z", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("W", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableHeadersRow();

		// --- Position 행 ---
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Position");
		ImGui::TableSetColumnIndex(1); ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%.2f", vPosition.x);
		ImGui::TableSetColumnIndex(2); ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "%.2f", vPosition.y);
		ImGui::TableSetColumnIndex(3); ImGui::TextColored(ImVec4(0.4f, 0.4f, 1.0f, 1.0f), "%.2f", vPosition.z);
		ImGui::TableSetColumnIndex(4); ImGui::TextUnformatted("-"); // Position은 W가 없음

		// --- Rotation 행 (Quaternion) ---
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Quat Rot");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%.4f", vQuat.x);
		ImGui::TableSetColumnIndex(2); ImGui::Text("%.4f", vQuat.y);
		ImGui::TableSetColumnIndex(3); ImGui::Text("%.4f", vQuat.z);
		ImGui::TableSetColumnIndex(4); ImGui::Text("%.4f", vQuat.w);

		// --- Scale 행 ---
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Scale");
		ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", vScale.x);
		ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", vScale.y);
		ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", vScale.z);
		ImGui::TableSetColumnIndex(4); ImGui::TextUnformatted("-"); // Scale도 W가 없음

		ImGui::EndTable();
	}


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