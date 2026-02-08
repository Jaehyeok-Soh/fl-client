#include "pch.h"
#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "Engine_Utils.h"
#include "ToolCanvas.h"
#include "ToolUI.h"
#include "Texture.h"
#include "UIProgress_Component.h"
#include "GameInstance.h"

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

	m_VecClassTag.reserve(ENUM_TO_UINT(DTO::EUIClassType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(DTO::EUIClassType::END); ++i)
		m_VecClassTag.push_back(DTO::UIClassTypeToString(static_cast<DTO::EUIClassType>(i)));

	m_VecOwnerTag.reserve(ENUM_TO_UINT(DTO::EUIOwnerType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(DTO::EUIOwnerType::END); ++i)
		m_VecOwnerTag.push_back(DTO::UIOwnertypeToString(static_cast<DTO::EUIOwnerType>(i)));

	return S_OK;
}

void CUI_Inspector::Update(const _float fTimeDelta)
{

}

HRESULT CUI_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);

	/*Setting_Texture();*/
	m_pSelectedUI = m_pUIManager->Safe_Access_UI(m_pUIManager->Get_CurUIIndex());
	if (nullptr != m_pSelectedUI)
	{
		m_pSelectedUI->Set_HitTest();
		SetUp_Public_Info();
		Input_TextureTag();
		
		if (Begin_Card("SetUp Default Setting", "##SetUp_Default_Setting", 100.f))
		{
			SetUp_Class();
			SetUp_Owner();
		}
		End_Card();

		SetUp_Component();

		if(m_pSelectedUI->Get_Script_Component(L"UIProgress_Component"))
			SetUp_UIProgress();
	}

	ImGui::End();
	return S_OK;
}

void CUI_Inspector::SetUp_Public_Info()
{
	Input_RectTransform();
}

void CUI_Inspector::Input_RectTransform()
{
	ImGui::PushID("RectTransform");
	ImGui::SeparatorText("Rect Transform");
	ImGui::BeginChild("RectTransformCard", ImVec2(0, 168.f), true, ImGuiWindowFlags_NoScrollbar);
	ImGui::TextDisabled("Anchor / pivot preset (3x3).");
	ImGui::Spacing();

	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Current");
		ImGui::SameLine(90.f);
		ImGui::TextDisabled(RectTransformToString(static_cast<ERectTransform>(m_iRectTransformIndex)).c_str());
	}

	ImGui::Spacing();
	if (ImGui::BeginTable("##RectGrid33", 3, ImGuiTableFlags_SizingStretchSame))
	{
		for (int i = 0; i < 9; ++i)
		{
			ImGui::TableNextColumn();

			const bool isSelected = (static_cast<uint32_t>(m_pSelectedUI->Get_RectTransformType())== i);

			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
			}

			ImGui::SetNextItemWidth(-FLT_MIN);
			if (ImGui::Button(RectTransformToString(static_cast<ERectTransform>(i)).c_str(), ImVec2(-FLT_MIN, 36.f)))
				m_pSelectedUI->Set_RectTransformType(static_cast<ERectTransform>(i));

			if (isSelected)
				ImGui::PopStyleColor(3);
		}
		ImGui::EndTable();
	}

	ImGui::EndChild();


	////////////////////////////
	// Transform / Size Card
	ImGui::Spacing();
	ImGui::BeginChild("RectTransformValuesCard", ImVec2(0, 112.f), true, ImGuiWindowFlags_NoScrollbar);

	ImGui::TextDisabled("Size and position (local).");
	ImGui::Spacing();

	// Size (Width / Height) : 2 columns
	if (ImGui::BeginTable("##RectSizeTable", 3, ImGuiTableFlags_SizingStretchSame))
	{
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		Scrub_Float("Width", "##UIObjectWidth", m_pSelectedUI->Get_WIdth_Ptr(), 0.1f, 20.f, 0.1f, 10.0f, 100.f);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		Scrub_Float("Height", "##UIObjectHeight", m_pSelectedUI->Get_Height_Ptr(), 0.1f, 20.f, 0.1f, 10.0f, 100.f);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		
		ImGui::EndTable();
	}
	ImGui::Spacing();
	// Position (X / Y / Z) : 3 columns
	if (ImGui::BeginTable("##RectPosTable", 3, ImGuiTableFlags_SizingStretchSame))
	{
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		Scrub_Float("UIPosX", "##UIObjectPosX", m_pSelectedUI->Get_PosX_Ptr(), 0.1f, 20.f, 0.1f, 10.0f, 100.f);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		Scrub_Float("UIPosY", "##UIObjectPosY", m_pSelectedUI->Get_PosY_Ptr(), 0.1f, 20.f, 0.1f, 10.0f, 100.f);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN);
		Scrub_Float("UIPosZ", "##UIObjectPosZ", m_pSelectedUI->Get_PosZ_Ptr(), 0.1f, 20.f, 0.1f, 10.0f, 100.f);

		ImGui::EndTable();
	}

	ImGui::InputFloat("Alpha", &m_pSelectedUI->Get_Alpha_Ref());
	ImGui::Checkbox("Visible", &m_pSelectedUI->Get_InitVisible());

	ImGui::EndChild();
	ImGui::PopID();
}

void CUI_Inspector::Input_TextureTag()
{
	_bool& isUseColorTint = m_pSelectedUI->Get_UseColorTint();
	ImGui::Checkbox("Use ColorTint", &isUseColorTint);

	if (isUseColorTint)
	{
		Vec4& tint = m_pSelectedUI->Get_ColorTint_Ref();

		float fColor[4] = { tint.x, tint.y, tint.z, tint.w };

		if (ImGui::SliderFloat4("Color Tint", fColor, 0.f, 1.f))
		{
			tint.x = fColor[0];
			tint.y = fColor[1];
			tint.z = fColor[2];
			tint.w = fColor[3];
		}
	}
	else
	{
		if (ImGui::Button("Select Texture"))
		{
			OPENFILENAMEW ofn{};
			_tchar szFile[MAX_PATH] = { 0 };

			ofn.lStructSize = sizeof(OPENFILENAMEW);
			ofn.hwndOwner = g_hWnd;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = MAX_PATH; 
			ofn.lpstrFilter =
				L"Image Files (*.png;*.dds)\0*.png;*.dds\0"
				L"Png Files (*.png)\0*.png\0"
				L"Dds Files (*.dds)\0*.dds\0"
				L"All Files (*.*)\0*.*\0\0";
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			if (::GetOpenFileNameW(&ofn) == TRUE)
			{
				_wstring result = szFile; 

				std::filesystem::path f(result);
				if (f.extension().wstring() == L".png" || f.extension().wstring() == L".dds")
				{
					m_pSelectedUI->Set_TextureTag(L"Texture_" + f.stem().wstring());
					m_pSelectedUI->Request_Change_Texture();
				}
			}
		}
	}
}

void CUI_Inspector::SetUp_Class()
{
	if (nullptr == m_pSelectedUI)
		return;

	int cur = (int)m_pSelectedUI->Get_UIClassType();
	cur = (cur < 0) ? 0 : (cur >= (int)m_VecClassTag.size() ? (int)m_VecClassTag.size() - 1 : cur);

	const char* preview = m_VecClassTag.empty() ? "" : m_VecClassTag[cur].c_str();

	bool changed = false;

	if (ImGui::BeginCombo("UI Class", preview))
	{
		for (int i = 0; i < (int)m_VecClassTag.size(); ++i)
		{
			const bool isSelected = (cur == i);
			if (ImGui::Selectable(m_VecClassTag[i].c_str(), isSelected))
			{
				cur = i; 
				m_pSelectedUI->Set_UIClassType(static_cast<DTO::EUIClassType>(i));
				changed = true;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void CUI_Inspector::SetUp_Owner()
{
	if (nullptr == m_pSelectedUI)
		return;

	int cur = (int)m_pSelectedUI->Get_UIOwnerType();
	cur = (cur < 0) ? 0 : (cur >= (int)m_VecOwnerTag.size() ? (int)m_VecOwnerTag.size() - 1 : cur);

	const char* preview = m_VecOwnerTag.empty() ? "" : m_VecOwnerTag[cur].c_str();

	bool changed = false;

	if (ImGui::BeginCombo("UI Owner", preview))
	{
		for (int i = 0; i < (int)m_VecOwnerTag.size(); ++i)
		{
			const bool isSelected = (cur == i);
			if (ImGui::Selectable(m_VecOwnerTag[i].c_str(), isSelected))
			{
				cur = i;
				m_pSelectedUI->Set_UIOwnerType(static_cast<DTO::EUIOwnerType>(i));
				changed = true;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void CUI_Inspector::SetUp_Component()
{
	if(Begin_Card("Add Component", "Card_AddComponent", 100.f))
	{
		if (ImGui::Button("Add Progress Component"))
		{
			m_pSelectedUI->Add_Script_Component(L"UIProgress_Component", CUIProgress_Component::Create(m_pSelectedUI));
			m_pSelectedUI->Get_ComponentFlag() |= DTO::EComponentTypeFlag::PROGRESS_COMPONENT;
		}
	}
	End_Card();
}

void CUI_Inspector::SetUp_UIProgress()
{
	ImGui::SliderFloat("Progress", &m_pSelectedUI->Get_TestProgress_Ref(), 1.f, 0.f);
	const char* items[] = { "L2R", "R2L", "B2T", "T2B" };
	ImGui::Combo("FillDir", &m_pSelectedUI->Get_FillDir_Ref(), items, IM_ARRAYSIZE(items));
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


bool CUI_Inspector::Begin_Card(const char* Label, const char* ID, float fHeight)
{
	ImGui::PushID(ID);

	if (Label && Label[0] != '\0')
		ImGui::SeparatorText(Label);

	m_vLastCardPos = ImGui::GetCursorScreenPos();
	m_vLastCardSize = ImVec2(ImGui::GetContentRegionAvail().x, fHeight);

	auto* dl = ImGui::GetWindowDrawList();
	dl->AddRectFilled(m_vLastCardPos,
		ImVec2(m_vLastCardPos.x + m_vLastCardSize.x, m_vLastCardPos.y + m_vLastCardSize.y),
		IM_COL32(30, 30, 30, 200), 8.0f);

	dl->AddRect(m_vLastCardPos,
		ImVec2(m_vLastCardPos.x + m_vLastCardSize.x, m_vLastCardPos.y + m_vLastCardSize.y),
		IM_COL32(90, 90, 90, 255), 8.0f);

	ImGui::SetCursorScreenPos(ImVec2(m_vLastCardPos.x + 10.0f, m_vLastCardPos.y + 10.0f));
	return ImGui::BeginChild("CardInner", ImVec2(m_vLastCardSize.x - 20.0f, m_vLastCardSize.y - 20.0f),
		false, ImGuiWindowFlags_NoScrollbar);
}

void CUI_Inspector::End_Card()
{
	ImGui::EndChild();

	ImGui::SetCursorScreenPos(m_vLastCardPos);
	ImGui::Dummy(m_vLastCardSize);

	ImGui::PopID();
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


