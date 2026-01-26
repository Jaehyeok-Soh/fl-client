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

	m_pSelectedUI = m_pUIManager->Safe_Access_UI(m_pUIManager->Get_CurUIIndex());
	SetUp_Public_Info();

	ImGui::End();
	return S_OK;
}

void CUI_Inspector::SetUp_Public_Info()
{
	Input_RectTransform();
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

void CUI_Inspector::Input_Canvas_TransformInfo()
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
	auto* pData = m_pUIManager->Safe_Access_UI(m_pUIManager->Get_CurUIIndex());
	Scrub_Float("Width :", "UISizeX", &pData->fWidth, 0.1f, 20.f, 0.1f, 10.0f, 100.f);
	ImGui::SameLine(0.f, 16.f);
	Scrub_Float("Height :", "UISizeY", &pData->fHeight, 0.1f, 20.f, 0.1f, 10.0f, 100.f);

	/* Pos X / Y / Z */
	Scrub_Float("X :", "UIPosX", &pData->fPosX, 0.1f, 20.f, 0.1f, 10.0f, 100.f);
	ImGui::SameLine(0.f, 16.f);
	Scrub_Float("Y :", "UIPosY", &pData->fPosY, 0.1f, 20.f, 0.1f, 10.0f, 100.f);
	ImGui::SameLine(0.f, 16.f);
	Scrub_Float("Z :", "UIPosZ", &pData->fPosZ, 0.1f, 20.f, 0.1f, 10.0f, 100.f);

	LAYER_DATA* pLayer = m_pUIManager->Safe_Access_Layer(m_pUIManager->Get_CurLayerIndex());
	if (nullptr != pLayer)
	{
		if (!pLayer->vecUIObjects.empty())
			pLayer->vecUIObjects[m_pUIManager->Get_CurUIIndex()]->Set_Position(pData->fPosX, pData->fPosY, pData->fPosZ);
	}
}

_bool CUI_Inspector::Scrub_Float(const _char* label, const _char* Id, OUT _float* pValue, float fValuePerPixel, float fValuePerPixel_fast, float fStep, float fStep_fast, float fSize)
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


