#include "pch.h"
#include "UI_Maker.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "ToolCanvas.h"
#include "ToolUI.h"
#include "Engine_Utils.h"

#include "DataStruct_UI.h"
#include "GameInstance.h"

CUI_Maker::CUI_Maker(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:Super(pLabel, pOwner, pDevice, pDeviceContext),
	m_pToolManager(CImGui_ToolManager::GetInstance()),
	m_pUIManager(CImGui_UIManager::GetInstance()),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pToolManager);
	Safe_AddRef(m_pUIManager);
	Safe_AddRef(m_pGameInstance);
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
	SetUp_Level();
	Make_Canvas();

	if (0 < m_pUIManager->Get_NumCanvas())
	{
		Make_UI();
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
	{
		m_pUIManager->Clear();
	}

	ImGui::EndChild();

	ImGui::SameLine();

	////////////////////////////
	// [3] Save Card
	ImGui::BeginChild("SaveCard", ImVec2(fCardW, fCardH), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Save");
	ImGui::Text("Save UI Info to JSON.");
	ImGui::Spacing();

	if (DrawBottomFullButton("Save UI"))
		int a = 0;

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
		CToolCanvas* pCanvas = m_pUIManager->Safe_Access_Canvas(iCurCanvas);
		/* 커서가 이상할 때 */
		if (nullptr == pCanvas)
		{
			m_pUIManager->Safe_Change_Canvas(0);
			iCurCanvas = 0;
		}
		else
		{
			/* 캔버스 목록 보여주는 코드 ======================================= */
			ImGui::TextDisabled("Select CANVAS To Edit");
			ImGui::BeginChild("CANVAS List", ImVec2(0, 70), true);
			for (int32_t i = 0; i < iNumCanvas; ++i)
			{
				auto* pCanvas = m_pUIManager->Safe_Access_Canvas(i);
				if (nullptr == pCanvas)
					continue;

				bool selected = (m_pUIManager->Get_CurCanvasIndex() == i);
				if (ImGui::Selectable(pCanvas->Get_Tag().c_str(), selected))
					m_pUIManager->Safe_Change_Canvas(i);
			}
			ImGui::EndChild();

			const float fSpacing = ImGui::GetStyle().ItemSpacing.x;
			const float fAvailW = ImGui::GetContentRegionAvail().x;
			const float fBtnW = (fAvailW - fSpacing) * 0.5f;


			Input_Canvas_TransformInfo();
		}
	}
	else
	{
		ImGui::TextDisabled("No Canvas.");
	}
	ImGui::EndChild();

	/* 위에서 Create 버튼을 눌렀다면 */
	if (m_isCreateCanvas)
	{
		if (m_strCanvasTag == "")
		{
			MSG_BOX("CUI_Maker::Make_Canvas, Empty Tag");
			m_isCreateCanvas = FALSE;
		}
		else
		{
			vector<CToolCanvas*>* pCanvasVec = m_pUIManager->Safe_Access_CanvasVector();
			_bool isDuplicated = FALSE;

			for (int32_t i = 0; i < m_pUIManager->Get_NumCanvas(); ++i)
			{
				if (m_strCanvasTag == (*pCanvasVec)[i]->Get_Tag())
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
				CToolCanvas::TOOLCANVAS_DESC Desc = {};
				Desc.strTag = m_strCanvasTag;
				Desc.iLevelIndex = { static_cast<uint32_t>(ELevelType::UI) };
				Desc.iClientLevelIndex = m_iCurSelectLevelID;
				Desc.fX = static_cast<_float>(g_iWinSizeX) / 2.f;
				Desc.fY = static_cast<_float>(g_iWinSizeY) / 2.f;
				Desc.fHeight = static_cast<_float>(g_iWinSizeY);
				Desc.fWidth = static_cast<_float>(g_iWinSizeX);
				Desc.iEditorSizeX = g_iWinSizeX;
				Desc.iEditorSizeY = g_iWinSizeY;

				_wstring wstrLayerTag = Engine_Utils::ToWString(m_strCanvasTag) + L"_Layer";
				CGameObject* pResult =
					CGameInstance::GetInstance()->Add_GameObject(Desc.iLevelIndex, g_wszPrototypeTagCanvas, Desc.iLevelIndex, wstrLayerTag, &Desc);

				if (nullptr == pResult)
				{
					m_strCanvasTag = "";
					m_isCreateCanvas = FALSE;
					MSG_BOX("CUI_Maker::Make_Canvas, Canvas Create Failed");
				}
				else
				{
					auto* pCanvas = dynamic_cast<CToolCanvas*>(pResult);
					if (nullptr != pCanvas)
					{
						if(FAILED(m_pUIManager->Safe_Add_Canvas(pCanvas)))
							ImGui::PopID();
							
						if(FAILED(m_pUIManager->Safe_Add_CanvasCache(m_strCanvasTag, pCanvas)))
							ImGui::PopID();
					}
				}
				m_strCanvasTag = "";
				m_isCreateCanvas = FALSE;
			}
		}
	}
	ImGui::PopID();
}

void CUI_Maker::Make_UI()
{
	ImGui::PushID("UISection");
	ImGui::SeparatorText("UI");
	ImGui::BeginChild("UICard", ImVec2(0, 240.f), true, ImGuiWindowFlags_NoScrollbar);
	ImGui::TextDisabled("Create / manage UI in current layer.");
	ImGui::Spacing();

	{
		const float fLabelW = 90.f;
		const float fSpacing = ImGui::GetStyle().ItemSpacing.x;
		const float fBtnW = 110.f;
		ImGui::AlignTextToFramePadding();
		ImGui::Text("UI Name");
		ImGui::SameLine(fLabelW);
		const float fAvailW = ImGui::GetContentRegionAvail().x;
		const float fInputW = fAvailW - fSpacing - fBtnW;
		ImGui::SetNextItemWidth(fInputW);
		ImGui::InputText("##UIName", &m_strUIName);
		ImGui::SameLine();
		if (ImGui::Button("Create", ImVec2(fBtnW, 0.f)))
		{
			m_isCreateUI = TRUE;
			m_iRectTransformIndex = 4;
		}

		if (m_isCreateUI)
		{
			if ("" == m_strUIName )
			{
				MSG_BOX("CUI_Maker::Make_UI_Btn, Empty Tag");
				m_isCreateUI = FALSE;
			}
			else
			{
				_bool isDuplicate = FALSE;

				auto* pUIVec = m_pUIManager->Safe_Access_UIVector();
				if (nullptr != pUIVec)
				{
					for (auto* pUi : (*pUIVec))
					{
						if (m_strUIName == pUi->Get_Name())
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
					m_strUIName = "";
				}
				else
				{
					CToolUI::TOOLUI_DESC Desc = {};
					Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::UI);
					Desc.fHeight = 100.f;
					Desc.fWidth = 100.f;
					Desc.strName = m_strUIName;
					auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
					if (nullptr != pCanvas)
					{
						Desc.strCanvasName = pCanvas->Get_Tag();
						Desc.pCacheCanvas = pCanvas;
					}

					Desc.iCanvasIndex = m_pUIManager->Get_CurCanvasIndex();
					Desc.isAlpha = TRUE;
					Desc.isInitVisible = TRUE;
					Desc.strInitTextureTag = "Prototype_Component_UI_Menu_Texture";
					Desc.iInitTextureIndex = 1;

					_wstring wstrLayerTag = Engine_Utils::ToWString(pCanvas->Get_Tag()) + L"_Layer";
					CGameObject* pResult =
						CGameInstance::GetInstance()->Add_GameObject(Desc.iLevelIndex, g_wszPrototypeTagUI, Desc.iLevelIndex, wstrLayerTag, &Desc);

					if (nullptr == pResult)
					{
						m_strUIName = "";
						m_isCreateUI = FALSE;
						MSG_BOX("CUI_Maker::Make_Layers, Layer Create Failed");
					}
					else
					{
						auto* pUI = dynamic_cast<CToolUI*>(pResult);
						if (nullptr != pUI) {
							if (FAILED(pCanvas->Safe_Add_UI(pUI)))
							{
								m_strUIName = "";
								m_isCreateUI = FALSE;
								MSG_BOX("CUI_Maker::Make_UI, UI Add Failed");
							}

							if(FAILED(m_pUIManager->Safe_Add_UICache(m_strUIName, pUI)))
							{
								m_strUIName = "";
								m_isCreateUI = FALSE;
								MSG_BOX("CUI_Maker::Make_UI, UI Add Failed");
							}
						}
					}
					m_strUIName = "";
					m_isCreateUI = FALSE;
				}
			}
		}
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::TextDisabled("Select UI");
	ImGui::BeginChild("UIList", ImVec2(0, 120.f), true);
	auto* pUIVec = m_pUIManager->Safe_Access_UIVector();
	const int32_t iNumUI = m_pUIManager->Get_NumUI();

	if (nullptr != pUIVec)
	{
		for (int32_t i = 0; i < iNumUI; ++i)
		{
			auto* pUI = (*pUIVec)[i];
			if (nullptr == pUI)
				continue;

			bool selected = (m_pUIManager->Get_CurUIIndex() == i);
			if (ImGui::Selectable(pUI->Get_Name().c_str(), selected))
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
	auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
	if (nullptr != pCanvas)
	{
		if (ImGui::Button("Reset Pos", ImVec2(0.f, 0.f)))
			pCanvas->Set_Position((_float)g_iWinSizeX * 0.5f, (_float)g_iWinSizeY * 0.5f, 0.5f);
		
		Scrub_Float("X :", "CanvasPosX", pCanvas->Get_PosX_Ptr(), 0.1f, 20.f, 0.1f, 1.0f, 100.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Y :", "CanvasPosY", pCanvas->Get_PosY_Ptr(), 0.1f, 20.f, 0.1f, 1.0f, 100.f);
		ImGui::SameLine(0.f, 16.f);
		Scrub_Float("Z :", "CanvasPosZ", pCanvas->Get_PosZ_Ptr(), 0.1f, 20.f, 0.1f, 1.0f, 100.f);

		UINT iNumViewports = 1;
		D3D11_VIEWPORT Viewports = {};

		m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewports);

		*pCanvas->Get_Width_Ptr() = Viewports.Width;
		*pCanvas->Get_Height_Ptr() = Viewports.Height;
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
	Safe_Release(m_pGameInstance);
	Super::Free();
}


