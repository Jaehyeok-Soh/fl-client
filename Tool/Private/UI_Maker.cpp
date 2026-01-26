#include "pch.h"
#include "UI_Maker.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "UIData_Repository.h"
#include "ToolUI.h"

CUI_Maker::CUI_Maker(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pToolManager(CImGui_ToolManager::GetInstance()),
	m_pUIManager(CImGui_UIManager::GetInstance())
{
	Safe_AddRef(m_pToolManager);
	Safe_AddRef(m_pUIManager);
}

HRESULT CUI_Maker::Initialize_Prototype()
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

void CUI_Maker::Update(const _float fTimeDelta)
{

}

HRESULT CUI_Maker::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);

	UIData_IO();

	/* 레벨을 선택 */
	SetUp_Level();

	/* 캔버스를 생성 */
	Make_Canvas();

	if (m_pUIManager->Get_NumCanvas() > 0)
	{
		Make_Layer();

		if (m_pUIManager->Get_NumLayer() > 0)
		{
			Make_UI();
		}
	}
	ImGui::End();
	return S_OK;
}

void CUI_Maker::UIData_IO()
{
	ImGui::PushID("UIDataActions");
	ImGui::SeparatorText("UI Data");

	const float fSpacing = ImGui::GetStyle().ItemSpacing.x;
	const float fAvailW = ImGui::GetContentRegionAvail().x;
	const float fCardW = (fAvailW - fSpacing * 2.f) / 3.f;
	const float fCardH = 76.f;

	auto DrawBottomFullButton = [&](const char* pLabel) -> bool
		{
			const float fBtnH = ImGui::GetFrameHeight();
			const float fPadX = ImGui::GetStyle().WindowPadding.x;
			const float fPadY = ImGui::GetStyle().WindowPadding.y;

			const float fYTarget = fCardH - fPadY - fBtnH;
			if (ImGui::GetCursorPosY() < fYTarget)
				ImGui::SetCursorPosY(fYTarget);

			ImGui::SetCursorPosX(fPadX);
			return ImGui::Button(pLabel, ImVec2(-FLT_MIN, 0.f));
		};

	////////////////////////////
	// [1] Load Data
	ImGui::BeginChild("LoadCard", ImVec2(fCardW, fCardH), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Load");
	ImGui::Text("UI info Load from JSON.");
	ImGui::Spacing();

	if (DrawBottomFullButton("Load UI"))
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
			if (FAILED(CUIData_Repository::GetInstance()->Load_UIData(result, m_pUIManager->Get_CanvasDataVector_Ref())))
			{
				ImGui::EndChild();
				ImGui::PopID();
				return;
			}
			m_pUIManager->Remake_UIObjects();
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();

	////////////////////////////
	// [2] Clear Card
	ImGui::BeginChild("ClearCard", ImVec2(fCardW, fCardH), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Clear");
	ImGui::Text("Remove All Objects");
	ImGui::Spacing();

	if (DrawBottomFullButton("Clear All"))
		m_pUIManager->Clear_UIObjects();

	ImGui::EndChild();

	ImGui::SameLine();

	////////////////////////////
	// [3] Save Card
	ImGui::BeginChild("SaveCard", ImVec2(fCardW, fCardH), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Save");
	ImGui::Text("Save UI Info to JSON.");
	ImGui::Spacing();

	if (DrawBottomFullButton("Save UI"))
		CUIData_Repository::GetInstance()->Save_UIData(L"../../Resources/Data/UIData/Data.json");

	ImGui::EndChild();
	ImGui::PopID();
}

void CUI_Maker::SetUp_Level()
{
	ImGui::PushID("SetUpLevel");

	ImGui::SeparatorText("SetUp Level");

	ImGui::BeginChild("LevelCard", ImVec2(0, 76), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Select the working level for preview / spawn.");
	ImGui::Spacing();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Level");
	ImGui::SameLine(90);

	ImGui::SetNextItemWidth(-1);
	ImGui::Combo("##Current_Selected_Level", &m_iCurSelectLevelID, m_szArrClientLevelType, (int)m_vecClientLevelType.size());

	ImGui::EndChild();

	ImGui::PopID();
}

void CUI_Maker::Make_Canvas()
{
	ImGui::PushID("CanvasSection");

	ImGui::SeparatorText("Canvas");

	ImGui::BeginChild("CanvasCard", ImVec2(0, 240), true);

	ImGui::TextDisabled("Create a canvas by tag. Tags must be unique.");
	ImGui::Spacing();

	ImGui::TextUnformatted("CanvasTag :");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("##CanvasTag", &m_strCanvasTag);

	ImGui::SameLine();

	if (ImGui::Button("Create Canvas"))
		m_isCreateCanvas = TRUE;

	ImGui::Spacing();
	ImGui::Separator();

	const int32_t iNumCanvas = m_pUIManager->Get_NumCanvas();
	int32_t iCurCanvas = m_pUIManager->Get_CurCanvasIndex();

	if (0 < iNumCanvas)
	{
		if (nullptr == m_pUIManager->Safe_Access_Canvas(iCurCanvas))
		{
			m_pUIManager->Safe_Change_Canvas(0);
			iCurCanvas = m_pUIManager->Get_CurCanvasIndex();
		}

		if (nullptr != m_pUIManager->Safe_Access_Canvas(iCurCanvas))
		{
			ImGui::TextDisabled("Select CanvasTag To Edit");
			ImGui::BeginChild("Canvas List", ImVec2(0, 70), true);
			for (int32_t i = 0; i < iNumCanvas; ++i)
			{
				auto* pCanvas = m_pUIManager->Safe_Access_Canvas(i);
				if (nullptr == pCanvas)
					continue;

				bool selected = (m_pUIManager->Get_CurCanvasIndex() == i);
				if (ImGui::Selectable(pCanvas->strTag.c_str(), selected))
					m_pUIManager->Safe_Change_Canvas(i);
			}
			ImGui::EndChild();

			{
				const float fSpacing = ImGui::GetStyle().ItemSpacing.x;
				const float fAvailW = ImGui::GetContentRegionAvail().x;
				const float fBtnW = (fAvailW - fSpacing) * 0.5f;

				if (ImGui::Button("Setting By Custom Size", ImVec2(fBtnW, 0.f)))
				{
					m_isCustomSize = TRUE;
					m_isViewportSize = FALSE;
				}

				ImGui::SameLine();

				if (ImGui::Button("Setting By Viewport Size", ImVec2(fBtnW, 0.f)))
				{
					m_isCustomSize = FALSE;
					m_isViewportSize = TRUE;
				}
			}

			Input_Canvas_TransformInfo();

			if (ImGui::Button("Delete Canvas"))
				m_pUIManager->Safe_Remove_CanvasData();
		}
	}
	else
	{
		ImGui::TextDisabled("No Canvas.");
	}

	ImGui::EndChild();

	if (m_isCreateCanvas)
	{
		if (m_strCanvasTag == "")
		{
			MSG_BOX("CUI_Maker::Make_Canvas, Empty Tag");
			m_isCreateCanvas = FALSE;
		}
		else
		{
			const auto* pCanvasVec = m_pUIManager->Safe_Access_CanvasVector();
			_bool isDuplicated = FALSE;

			for (int32_t i = 0; i < m_pUIManager->Get_NumCanvas(); ++i)
			{
				if (m_strCanvasTag == (*pCanvasVec)[i].strTag)
				{
					isDuplicated = TRUE;
					break;
				}
			}

			if (isDuplicated)
			{
				MSG_BOX("CUI_Maker::Make_Canvas, Tag Already Created");
				m_isCreateCanvas = FALSE;
			}
			else
			{
				CANVAS_DATA tCanvasData = {};
				tCanvasData.strTag = m_strCanvasTag;
				m_pUIManager->Safe_Add_CanvasData(tCanvasData);

				m_strCanvasTag = "";
				m_isCreateCanvas = FALSE;
			}
		}
	}
	ImGui::PopID();
}

void CUI_Maker::Make_Layer()
{
	ImGui::PushID("EditLayers");
	ImGui::SeparatorText("Layers");

	ImGui::BeginChild("LayersCard", ImVec2(0, 240.f), true);

	ImGui::TextDisabled("Create a layer by tag. Tags must be unique.");
	ImGui::Spacing();

	{
		const float fSpacing = ImGui::GetStyle().ItemSpacing.x;

		ImGui::AlignTextToFramePadding();
		ImGui::Text("LayerTag");
		ImGui::SameLine(90.f);

		const float fAvailAfterLabel = ImGui::GetContentRegionAvail().x;
		const float fBtnW = ImMin(140.f, fAvailAfterLabel * 0.28f);
		float fInputW = fAvailAfterLabel - fSpacing - fBtnW;
		if (fInputW < 10.f) fInputW = 10.f;

		ImGui::SetNextItemWidth(fInputW);
		ImGui::InputText("##LayerTag", &m_strLayerTag);

		ImGui::SameLine();

		///////////////////
		// 생성 버튼
		if (ImGui::Button("Create", ImVec2(fBtnW, 0.f)))
		{
			m_isCreateLayer = TRUE;
			if (m_isCreateLayer)
			{
				if (m_strLayerTag == "")
				{
					MSG_BOX("CUI_Maker::Make_Layers, Empty Tag");
					m_isCreateLayer = FALSE;
					return;
				}

				const auto* pLayerVec = m_pUIManager->Safe_Access_LayerVector();
				for (int32_t i = 0; i < m_pUIManager->Get_NumLayer(); ++i)
				{
					if (m_strLayerTag == (*pLayerVec)[i].strTag)
					{
						MSG_BOX("CUI_Maker::Make_Layers, This LayerTag Already Exist in Currnet Canvas");
						m_isCreateLayer = FALSE;
						return;
					}
				}

				if (m_isCreateLayer)
				{
					LAYER_DATA tData = {};
					tData.strTag = m_strLayerTag;
					m_pUIManager->Safe_Add_LayerData(tData);
					m_strLayerTag = "";
					m_isCreateLayer = FALSE;
				}
			}
		}
	}

	////////////////////
	// 레이어 리스트
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	const int32_t iNumLayer = m_pUIManager->Get_NumLayer();
	int32_t iCurLayer = m_pUIManager->Get_CurLayerIndex();

	if (0 < iNumLayer)
	{
		if (nullptr == m_pUIManager->Safe_Access_Layer(iCurLayer))
		{
			m_pUIManager->Safe_Change_Layer(0);
			iCurLayer = m_pUIManager->Get_CurLayerIndex();
		}

		if (nullptr != m_pUIManager->Safe_Access_Layer(iCurLayer))
		{
			ImGui::TextDisabled("Select Layer");
			ImGui::BeginChild("LayerList", ImVec2(0, 92.f), true);

			for (int32_t i = 0; i < iNumLayer; ++i)
			{
				auto* pLayer = m_pUIManager->Safe_Access_Layer(i);
				if (nullptr == pLayer)
					continue;

				const bool selected = (m_pUIManager->Get_CurLayerIndex() == i);
				if (ImGui::Selectable(pLayer->strTag.c_str(), selected))
					m_pUIManager->Safe_Change_Layer(i);
			}

			ImGui::EndChild();
		}
	}
	else
	{
		ImGui::TextDisabled("No Layer.");
	}

	ImGui::EndChild();
	ImGui::PopID();
}

void CUI_Maker::Make_UI()
{
	ImGui::PushID("UISection");
	ImGui::SeparatorText("UI");

	// Canvas / Layers 처럼 "카드 1개" 안에 전부 넣기
	ImGui::BeginChild("UICard", ImVec2(0, 240.f), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Create / manage UI in current layer.");
	ImGui::Spacing();

	////////////////////////////
	// Create Row
	{
		const float fLabelW = 90.f;
		const float fSpacing = ImGui::GetStyle().ItemSpacing.x;
		const float fBtnW = 110.f;

		ImGui::AlignTextToFramePadding();
		ImGui::Text("UI Name");
		ImGui::SameLine(fLabelW);

		// 여기서 GetContentRegionAvail()을 잡아야 버튼 잘림이 안 납니다(라벨 영역 제외된 상태)
		const float fAvailW = ImGui::GetContentRegionAvail().x;
		const float fInputW = fAvailW - fSpacing - fBtnW;

		ImGui::SetNextItemWidth(fInputW);
		ImGui::InputText("##UIName", &m_strUIName);

		ImGui::SameLine();

		if (ImGui::Button("Create", ImVec2(fBtnW, 0.f)))
		{
			m_isCreateUI = TRUE;
			m_iRectTransformIndex = 4;

			if (m_strUIName == "")
			{
				MSG_BOX("CUI_Maker::Make_UI_Btn, Empty Tag");
				m_isCreateUI = FALSE;
			}
			else
			{
				_bool isDuplicate = FALSE;

				// Safe_Access_UIVector()가 비면 nullptr 반환한다는 전제
				auto* pUIVec = m_pUIManager->Safe_Access_UIVector();
				if (nullptr != pUIVec)
				{
					for (const auto& ui : (*pUIVec))
					{
						if (m_strUIName == ui.strName)
						{
							isDuplicate = TRUE;
							break;
						}
					}
				}

				if (isDuplicate)
				{
					MSG_BOX("CUI_Maker::Make_UI_Btn, Tag Already Created");
					m_isCreateUI = FALSE;
				}
				else
				{
					GENERIC_UI_DATA tData = {};
					tData.strName = m_strUIName;
					m_pUIManager->Safe_Add_UIData(tData);
					m_strUIName = "";
					m_isCreateUI = FALSE;
				}
			}
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	////////////////////////////
	// UI List (카드 안에 포함)
	ImGui::TextDisabled("Select UI");
	ImGui::BeginChild("UIList", ImVec2(0, 120.f), true);

	auto* pUIVec = m_pUIManager->Safe_Access_UIVector();
	if (nullptr != pUIVec)
	{
		const int32_t iNumUI = (int32_t)pUIVec->size();
		for (int32_t i = 0; i < iNumUI; ++i)
		{
			auto* pUI = m_pUIManager->Safe_Access_UI(i);
			if (nullptr == pUI)
				continue;

			bool selected = (m_pUIManager->Get_CurUIIndex() == i);
			if (ImGui::Selectable(pUI->strName.c_str(), selected))
				m_pUIManager->Safe_Change_UI(i);
		}
	}
	else
	{
		ImGui::TextDisabled("No UI.");
	}

	ImGui::EndChild();   // UIList
	ImGui::EndChild();   // UICard
	ImGui::PopID();
}

_bool CUI_Maker::Scrub_Float(const _char* label, const _char* Id, OUT _float* pValue, float fValuePerPixel, float fValuePerPixel_fast, float fStep, float fStep_fast, float fSize)
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
			if (ImGui::GetIO().KeyShift) scale = 0.1f;
			if (ImGui::GetIO().KeyCtrl)  scale = fValuePerPixel_fast;

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

void CUI_Maker::Input_Canvas_TransformInfo()
{
	/* 내맘대로 만들겠다 */
	if (m_isCustomSize)
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());

		/* Width / Height */
		Scrub_Float("Width :", "CanvasSizeX", &pCanvas->fWidth, 0.1f, 20.f, 0.1f, 1.0f, 120.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Height :", "CanvasSizeY", &pCanvas->fHeight, 0.1f, 20.f, 0.1f, 1.0f, 120.f);

		/* Pos X / Y / Z */
		Scrub_Float("X :", "CanvasPosX", &pCanvas->fPosX, 0.1f, 20.f, 0.1f, 1.0f, 100.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Y :", "CanvasPosY", &pCanvas->fPosY, 0.1f, 20.f, 0.1f, 1.0f, 100.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Z :", "CanvasPosZ", &pCanvas->fPosZ, 0.1f, 20.f, 0.1f, 1.0f, 100.f);

		pCanvas->isUsingViewport = FALSE;
	}
	/* 뷰포트 기준으로 캔버스를 만들겠다 */
	else if (m_isViewportSize)
	{
		auto* pData = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());

		pData->fWidth = m_pToolManager->Get_CurViewportSize().x;
		pData->fHeight = m_pToolManager->Get_CurViewportSize().y;
		pData->fPosX = 0.f;
		pData->fPosY = 0.f;
		pData->fPosZ = 0.f;
		pData->isUsingViewport = TRUE;
	}
}

CUI_Maker* CUI_Maker::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUI_Maker* pInstance = new CUI_Maker(pLabel, pOwner, pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUI_Maker::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Maker::Free()
{
	Safe_Release(m_pToolManager);
	Safe_Release(m_pUIManager);
	Super::Free();
}


