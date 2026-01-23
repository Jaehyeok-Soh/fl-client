#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"

CUI_Inspector::CUI_Inspector(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pToolManager(CImGui_ToolManager::GetInstance()),
	m_pUIManager(CImGui_UIManager::GetInstance())
{
	Safe_AddRef(m_pToolManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CUI_Inspector::Initialize_Prototype()
{
	m_vecClientLevelType.resize(g_iClientLevelType_Count);

	/* 클라이언트 레벨 저장 및 const char* 로 변환해서 따로 저장 / 툴에서 레벨을 이름보고 정할 수 있게하기 위해서 */
	for (size_t i = 0; i < g_iClientLevelType_Count; ++i)
	{
		m_vecClientLevelType[i] = (ClientleveltypeToString(static_cast<EClientLevelType>(i)));
		m_szArrClientLevelType[i] = m_vecClientLevelType[i].c_str();
	}

	return S_OK;
}

void CUI_Inspector::Update(const _float fTimeDelta)
{

}

HRESULT CUI_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);
	
	SetUp_Level();
	
	Create_Canvas();

	if (0 != m_pUIManager->Get_CanvasData_Size())
		Edit_Canvas();

	ImGui::End();
	return S_OK;
}

void CUI_Inspector::SetUp_Level()
{
	ImGui::Text("<<SetUp Level>>");
	ImGui::Combo("Current_Selected_Level", &m_iCurSelectLevelID, m_szArrClientLevelType, m_vecClientLevelType.size());
}

void CUI_Inspector::Create_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Canvas>>");

	Input_Canvas_Tag();
	Create_Canvas_Btn();

	/* Create 버튼을 눌렀다면 */
	if (m_isCreateCanvas)
	{
		if (m_strCurEditor_CanvasTag == "")
		{
			MSG_BOX("CUI_Inspector::Create_Canvas, Empty Tag");
			m_isCreateCanvas = FALSE;
			return;
		}

		/* 태그가 있다*/
		if (m_pUIManager->HasCanvasTag(m_strCurEditor_CanvasTag))
		{
			MSG_BOX("CUI_Inspector::Create_Canvas, Tag Already Created");
			m_isCreateCanvas = FALSE;
			return;
		}

		CANVAS_DATA tData = {};
		tData.strTag = m_strCurEditor_CanvasTag;
		m_pUIManager->Add_CanvasData(tData);

		m_strCurEditor_CanvasTag = "";
		m_isCreateCanvas = FALSE;
	}
}

void CUI_Inspector::Edit_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Select CanvasTag To Edit>>");
	ImGui::BeginChild("CanvasList", ImVec2(0, 100), true);
	for (int i = 0; i < m_pUIManager->Get_CanvasData_Size(); ++i)
	{
		bool selected = (m_pUIManager->Get_CurCanvasIndex() == i);
		if (ImGui::Selectable(m_pUIManager->Get_CanvasData(i).strTag.c_str(), selected))
			m_pUIManager->Set_CurCanvasIndex(i);
	}
	ImGui::EndChild();

	Setting_Canvas_CustomSize_Btn();
	ImGui::SameLine();
	Setting_Canvas_ViewportSize_Btn();
	Input_Canvas_TransformInfo();

	if (ImGui::Button("Delete Selected Canvas"))
	{
		m_pUIManager->Remove_CurCanvasData();
	}
}

void CUI_Inspector::Input_Canvas_Tag()
{
	ImGui::TextUnformatted("CanvasTag :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##CanvasTag", &m_strCurEditor_CanvasTag);
}

void CUI_Inspector::Input_Canvas_TransformInfo()
{
	/* 내맘대로 만들겠다 */
	if (m_isCustomSize)
	{
		/* Width / Height */
		ImGui::TextUnformatted("Width :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(90.f);
		ImGui::InputScalar("##CanvasSizeX", ImGuiDataType_S32, &m_pUIManager->Get_CurCanvasDataRef().iWidth);

		ImGui::SameLine(0.f, 16.f);

		ImGui::TextUnformatted("Height :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(90.f);
		ImGui::InputScalar("##CanvasSizeY", ImGuiDataType_S32, &m_pUIManager->Get_CurCanvasDataRef().iHeight);

		/* Pos X / Y / Z */
		ImGui::TextUnformatted("X :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosX", ImGuiDataType_S32, &m_pUIManager->Get_CurCanvasDataRef().iPosX);

		ImGui::SameLine(0.f, 16.f);

		ImGui::TextUnformatted("Y :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosY", ImGuiDataType_S32, &m_pUIManager->Get_CurCanvasDataRef().iPosY);

		ImGui::SameLine(0.f, 16.f);
		
		ImGui::TextUnformatted("Z :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(60.f);
		ImGui::InputScalar("##CanvasPosZ", ImGuiDataType_S32, &m_pUIManager->Get_CurCanvasDataRef().iPosZ);
		m_pUIManager->Get_CurCanvasDataRef().isUsingViewport = FALSE;
	}
	/* 뷰포트 기준으로 캔버스를 만들겠다 */
	else if (m_isViewportSize)
	{
		m_pUIManager->Get_CurCanvasDataRef().iWidth = m_pToolManager->Get_CurViewportSize().x;
		m_pUIManager->Get_CurCanvasDataRef().iHeight = m_pToolManager->Get_CurViewportSize().y;
		m_pUIManager->Get_CurCanvasDataRef().iPosX = 0;
		m_pUIManager->Get_CurCanvasDataRef().iPosY = 0;
		m_pUIManager->Get_CurCanvasDataRef().iPosZ = 0;
		m_pUIManager->Get_CurCanvasDataRef().isUsingViewport = TRUE;
	}
}

void CUI_Inspector::Create_Canvas_Btn()
{
	if (ImGui::Button("Create With This Tag"))
		m_isCreateCanvas = TRUE;
}

void CUI_Inspector::Setting_Canvas_CustomSize_Btn()
{
	if (ImGui::Button("Setting By CustomSize"))
	{
		m_isCustomSize = TRUE;
		m_isViewportSize = FALSE;

	}
}

void CUI_Inspector::Setting_Canvas_ViewportSize_Btn()
{
	if (ImGui::Button("Setting By ViewportSize"))
	{
		m_isCustomSize = FALSE;
		m_isViewportSize = TRUE;
	}
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
	Safe_Release(m_pToolManager);
	Safe_Release(m_pUIManager);
	Super::Free();
}


