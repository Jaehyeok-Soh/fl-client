#include "pch.h"
#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "UIData_Repository.h"
#include "ToolUI.h"

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
	
	/* 씬 */
	SetUp_Level();
	Load_Data();

	/* 캔버스 태그 받고 생성 */
	Make_Canvas();

	if (0 != m_pUIManager->Get_NumCanvas())
	{
		/* 캔버스 크기,위치 등 조정 */
		Edit_Canvas();

		/* 레이어 태그 받고 생성 */
		Make_Layers();

		/* 레이어 선택 */
		Edit_Layers();
	}


	ImGui::End();
	return S_OK;
}

void CUI_Inspector::Load_Data()
{
	if (ImGui::Button("Load UI Data", ImVec2(0, 0)))
	{
		OPENFILENAMEW ofn{};
		_tchar szFile[MAX_PATH] = { 0 };

		ofn.lStructSize = sizeof(OPENFILENAMEW);
		ofn.hwndOwner = g_hWnd;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = L"Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if (::GetOpenFileNameW(&ofn) == TRUE)
		{
			wstring result = szFile;
			if (FAILED(CUIData_Repository::GetInstance()->Load_UIData(result, m_pUIManager->Get_CurCanvas_Ref())))
				return;

			m_pUIManager->Remake_UIObjects();
		}
	}
}

void CUI_Inspector::SetUp_Level()
{
	ImGui::Text("<<SetUp Level>>");
	ImGui::Combo("Current_Selected_Level", &m_iCurSelectLevelID, m_szArrClientLevelType, static_cast<uint32_t>(m_vecClientLevelType.size()));
}

void CUI_Inspector::Make_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Canvas>>");

	Input_Canvas_Tag();
	Make_Canvas_Btn();

	/* Create 버튼을 눌렀다면 */
	if (m_isCreateCanvas)
	{
		if (m_strCanvasTag == "")
		{
			MSG_BOX("CUI_Inspector::Make_Canvas, Empty Tag");
			m_isCreateCanvas = FALSE;
			return;
		}

		/* 캔버스 태그가 있다*/

		const auto& vecCanvas = m_pUIManager->Get_CurCanvas();
		for (uint32_t i = 0; i < m_pUIManager->Get_NumCanvas(); ++i)
		{
			if (m_strCanvasTag == vecCanvas[i].strTag)
			{
				MSG_BOX("CUI_Inspector::Make_Canvas, Tag Already Created");
				m_isCreateCanvas = FALSE;
				return;
			}
		}

		CANVAS_DATA tData = {};
		tData.strTag = m_strCanvasTag;
		m_pUIManager->Add_CanvasData(tData);

		m_strCanvasTag = "";
		m_isCreateCanvas = FALSE;
	}
}

void CUI_Inspector::Edit_Canvas()
{
	ImGui::NewLine();
	ImGui::Text("<<Select CanvasTag To Edit>>");
	ImGui::BeginChild("CanvasList", ImVec2(0, 50), true);
	for (uint32_t i = 0; i < m_pUIManager->Get_NumCanvas(); ++i)
	{
		bool selected = (m_pUIManager->Get_CurCanvasIndex() == i);
		if (ImGui::Selectable(m_pUIManager->Get_CanvasData_Ptr(i)->strTag.c_str(), selected))
			m_pUIManager->Change_Canvas(i);
	}
	ImGui::EndChild();

	Setting_Canvas_CustomSize_Btn();
	ImGui::SameLine();
	Setting_Canvas_ViewportSize_Btn();
	Input_Canvas_TransformInfo();

	if (ImGui::Button("Delete Selected Canvas"))
		m_pUIManager->Remove_CanvasData();
}

void CUI_Inspector::Make_Layers()
{
	Input_Layer_Tag();

	if (m_isCreateLayer)
	{
		if (m_strLayerTag == "")
		{
			MSG_BOX("CUI_Inspector::Make_Layers, Empty Tag");
			m_isCreateLayer = FALSE;
			return;
		}

		const auto& vecLayer = m_pUIManager->Get_CurLayers();
		for (uint32_t i = 0; i < m_pUIManager->Get_NumLayer(m_pUIManager->Get_CurCanvasIndex()); ++i)
		{
			if ( m_strLayerTag == vecLayer[i].strTag)
			{
				MSG_BOX("CUI_Inspector::Make_Layers, This LayerTag Already Exist in Currnet Canvas");
				m_isCreateLayer = FALSE;
				return;
			}
		}

		if (m_isCreateLayer)
		{
			LAYER_DATA tData = {};
			tData.strTag = m_strLayerTag;
			m_pUIManager->Add_LayerData(tData);
			m_strLayerTag = "";
			m_isCreateLayer = FALSE;
		}
	}
}

void CUI_Inspector::Edit_Layers()
{
	ImGui::NewLine();
	ImGui::Text("<<Select Layer>>");
	ImGui::BeginChild("LayerList", ImVec2(0, 100), true);


	for (uint32_t i = 0; i < m_pUIManager->Get_NumLayer(m_pUIManager->Get_CurCanvasIndex()); ++i)
	{
		bool selected = (m_pUIManager->Get_CurLayerIndex() == i);
		if (ImGui::Selectable(m_pUIManager->Get_LayerData_Ptr(i)->strTag.c_str(), selected))
			m_pUIManager->Change_Layers(i);
	}
	ImGui::EndChild();

	if (0 != m_pUIManager->Get_NumLayer(m_pUIManager->Get_CurCanvasIndex()))
	{
		ImGui::NewLine();
		ImGui::Text("<<UI>>");
		ImGui::TextUnformatted("UIName :");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(150.f);
		ImGui::InputText("##UIName", &m_strUIName);
		Make_UI_Btn();

		if (!m_pUIManager->Get_CurUIDatas().empty())
		{
			Show_UI_List();
			Edit_UI();
		}
	}

}

void CUI_Inspector::Add_NewUI()
{
	GENERIC_UI_DATA tData = {};
	tData.strName = m_strUIName;
	m_pUIManager->Add_UIData(tData);
	m_strUIName = "";
}

void CUI_Inspector::Edit_UI()
{
	SetUp_UI_Common_Info();

	if (ImGui::Button("Save UI"))
	{
		CUIData_Repository::GetInstance()->Save_UIData(L"../../Resources/Data/UIData/Data.json");
	}
}

void CUI_Inspector::SetUp_UI_Common_Info()
{
	if (!ImGui::Begin("[[ Inspector ]]"))
	{
		ImGui::End();
		return;
	}

	/* =========================
	 *  Common Setting (Top)
	 * ========================= */

	if (ImGui::BeginTabBar("##SetUpUICommonInfo", ImGuiTabBarFlags_Reorderable))
	{
		if (ImGui::BeginTabItem("Common Setting"))
		{
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::BeginChild("##ContentBox1", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_None);

	/* 렉트 트랜스폼 */
	Input_RectTransform();
	/* 텍스쳐 경로 */
	// TODO: Input_TexturePath();

	ImGui::EndChild();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	/* =========================
	 *  Palette Tabs (Bottom)
	 * ========================= */

	static int s_iTab = 0;

	if (ImGui::BeginTabBar("##PersonalUISetting", ImGuiTabBarFlags_Reorderable))
	{
		if (ImGui::BeginTabItem("Button"))
		{
			s_iTab = 0;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Image"))
		{
			s_iTab = 1;
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Text"))
		{
			s_iTab = 2;
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::BeginChild("##ContentBox", ImVec2(0.f, 0.f), true, ImGuiWindowFlags_None);

	if (s_iTab == 0)
	{
		ImGui::Text("Button Tab");
		ImGui::Spacing();

		if (ImGui::Button("Create Button")) {}
		ImGui::SameLine();
		if (ImGui::Button("Delete Button")) {}
	}
	else if (s_iTab == 1)
	{
		ImGui::Text("Image Tab");
		ImGui::Spacing();

		ImGui::Text("ImTextureID가 있으면 ImGui::Image(...)로 출력하시면 됩니다.");
		if (ImGui::Button("Import Texture")) {}
	}
	else // s_iTab == 2
	{
		ImGui::Text("Text Tab");
		ImGui::Spacing();

		static char buf[128] = {};
		ImGui::InputText("Label", buf, IM_ARRAYSIZE(buf));
	}

	ImGui::EndChild();
	ImGui::End();
}

void CUI_Inspector::Show_UI_List()
{
	ImGui::NewLine();
	ImGui::Text("<<UI List>>");
	ImGui::BeginChild("UIList", ImVec2(0, 100), true);

	for (uint32_t i = 0; i < m_pUIManager->Get_NumUI(m_pUIManager->Get_CurCanvasIndex(), m_pUIManager->Get_CurLayerIndex()); ++i)
	{
		bool selected = (m_pUIManager->Get_CurUIIndex() == i);
		if (ImGui::Selectable(m_pUIManager->Get_UIData_Ptr(i)->strName.c_str(), selected))
			m_pUIManager->Change_UIData(i);
	}
	ImGui::EndChild();
}

void CUI_Inspector::Input_Canvas_Tag()
{
	ImGui::TextUnformatted("CanvasTag :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##CanvasTag", &m_strCanvasTag);
}

void CUI_Inspector::Input_Canvas_TransformInfo()
{
	/* 내맘대로 만들겠다 */
	if (m_isCustomSize)
	{
		auto* pData = m_pUIManager->Get_CanvasData_Ptr(m_pUIManager->Get_CurCanvasIndex());

		/* Width / Height */
		Scrub_Float("Width :", "CanvasSizeX", &pData->fWidth, 0.01f, 0.1f, 1.0f, 120.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Height :", "CanvasSizeY", &pData->fHeight, 0.01f, 0.1f, 1.0f, 120.f);

		/* Pos X / Y / Z */
		Scrub_Float("X :", "CanvasPosX", &pData->fPosX, 0.01f, 0.1f, 1.0f, 100.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Y :", "CanvasPosY", &pData->fPosY, 0.01f, 0.1f, 1.0f, 100.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Z :", "CanvasPosZ", &pData->fPosZ, 0.01f, 0.1f, 1.0f, 100.f);

		pData->isUsingViewport = FALSE;
	}
	/* 뷰포트 기준으로 캔버스를 만들겠다 */
	else if (m_isViewportSize)
	{
		auto* pData = m_pUIManager->Get_CanvasData_Ptr(m_pUIManager->Get_CurCanvasIndex());

		pData->fWidth = m_pToolManager->Get_CurViewportSize().x;
		pData->fHeight = m_pToolManager->Get_CurViewportSize().y;
		pData->fPosX = 0.f;
		pData->fPosY = 0.f;
		pData->fPosZ = 0.f;
		pData->isUsingViewport = TRUE;
	}
}

void CUI_Inspector::Input_Layer_Tag()
{
	ImGui::NewLine();
	ImGui::Text("<<Layer>>");
	ImGui::TextUnformatted("LayerTag :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##LayerTag", &m_strLayerTag);

	Make_Layer_Btn();
}

void CUI_Inspector::Input_RectTransform()
{
	ImGui::Text("Current Select : ");
	ImGui::SameLine();
	ImGui::Text(RectTransformToString(static_cast<ERectTransform>(m_iRectTransformIndex)).c_str());

	if (ImGui::BeginTable("##Grid33", 3, ImGuiTableFlags_SizingStretchSame))
	{
		for (int i = 0; i < 9; ++i)
		{
			ImGui::TableNextColumn();

			bool isSelected = (m_iRectTransformIndex == i);

			// 선택된 버튼만 색을 바꿔서 밝게 보이게
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			}

			if (ImGui::Button(RectTransformToString(static_cast<ERectTransform>(i)).c_str(), ImVec2(-FLT_MIN, 48.f)))
			{
				m_iRectTransformIndex = i;
			}

			if (isSelected)
				ImGui::PopStyleColor(3);
		}
		ImGui::EndTable();
	}

	/* Width / Height */
	auto* pData = m_pUIManager->Get_UIData_Ptr(m_pUIManager->Get_CurUIIndex());
	Scrub_Float("Width :", "UISizeX", &pData->fWidth, 0.01f, 0.1f, 1.0f, 100.f);
	ImGui::SameLine(0.f, 16.f);
	Scrub_Float("Height :", "UISizeY", &pData->fHeight, 0.01f, 0.1f, 1.0f, 100.f);

	/* Pos X / Y / Z */
	Scrub_Float("X :", "UIPosX", &pData->fPosX, 0.01f, 0.1f, 1.0f, 100.f);
	ImGui::SameLine(0.f, 16.f);
	Scrub_Float("Y :", "UIPosY", &pData->fPosY, 0.01f, 0.1f, 1.0f, 100.f);
	ImGui::SameLine(0.f, 16.f);
	Scrub_Float("Z :", "UIPosZ", &pData->fPosZ, 0.01f, 0.1f, 1.0f, 100.f);

	if (nullptr != m_pUIManager->Get_UI_Ptr(m_pUIManager->Get_CurUIIndex()))
	{
		m_pUIManager->Get_UI_Ptr(m_pUIManager->Get_CurUIIndex())->Set_Position(
			static_cast<_float>(m_pUIManager->Get_UIData_Ptr(m_pUIManager->Get_CurUIIndex())->fPosX),
			static_cast<_float>(m_pUIManager->Get_UIData_Ptr(m_pUIManager->Get_CurUIIndex())->fPosY));
	}

}

_bool CUI_Inspector::Scrub_Float(const _char* label, const _char* Id, OUT _float* pValue, float fValuePerPixel, float fStep, float fStep_fast, float fSize)
{
	ImGui::PushID(Id);

	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::CalcTextSize(label);

	ImGui::InvisibleButton("##ScrubLabel", size);

	if (ImGui::IsItemHovered())
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);

	_bool changed = FALSE;

	if (ImGui::IsItemActive())
	{
		float dx = ImGui::GetIO().MouseDelta.x;
		if (dx != 0.f)
		{
			float scale = 1.f;
			if (ImGui::GetIO().KeyShift) scale = 10.f;
			if (ImGui::GetIO().KeyCtrl)  scale = 0.1f;

			*pValue += dx * fValuePerPixel * scale;

			if (-FLT_MAX < FLT_MAX)
			{
				if (*pValue < -FLT_MAX) *pValue = -FLT_MAX;
				if (*pValue > FLT_MAX) *pValue = FLT_MAX;
			}
			changed = TRUE;
		}
	}
	ImGui::SetCursorScreenPos(pos);
	ImGui::TextUnformatted(label);
	ImGui::SameLine();

	ImGui::SetNextItemWidth(fSize);
	if (ImGui::InputScalar("##Value", ImGuiDataType_Float, pValue, &fStep, &fStep_fast, "%.1f"))
		changed = TRUE;

	ImGui::PopID();

	return changed;
}

void CUI_Inspector::Make_Canvas_Btn()
{
	if (ImGui::Button("Create Canvas With This Tag"))
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

void CUI_Inspector::Make_Layer_Btn()
{
	if (ImGui::Button("Create Layer With This Tag"))
	{
		m_isCreateLayer = TRUE;
	}
}

void CUI_Inspector::Make_UI_Btn()
{
	if (ImGui::Button("Create UI With This Layer"))
	{
		m_isCreateUI = TRUE;
		m_iRectTransformIndex = 4;

		if (m_strUIName == "")
		{
			MSG_BOX("CUI_Inspector::Make_UI_Btn, Empty Tag");
			m_isCreateCanvas = FALSE;
			return;
		}

		/* UI 태그가 있다*/
		const auto& vecUI = m_pUIManager->Get_CurUIDatas();
		for (uint32_t i = 0; i < m_pUIManager->Get_NumUI(m_pUIManager->Get_CurCanvasIndex(), m_pUIManager->Get_CurLayerIndex()); ++i)
		{
			if (m_strCanvasTag == vecUI[i].strName)
			{
				MSG_BOX("CUI_Inspector::Make_UI_Btn, Tag Already Created");
				m_isCreateCanvas = FALSE;
				return;
			}
		}
		Add_NewUI();
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


