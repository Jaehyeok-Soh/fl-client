#include "pch.h"
#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "UIData_Repository.h"
#include "Engine_Utils.h"
#include "ToolUI.h"
#include "Texture.h"

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

	//Folder_Search("../../Resources/Textures/UI");
	//File_Search("../../Resources/Textures/UI");

	return S_OK;
}

void CUI_Inspector::Update(const _float fTimeDelta)
{
	if (nullptr != m_pSelectedUI)
		m_pSelectedUI->Set_HitTest();
}

HRESULT CUI_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);

	/*Setting_Texture();*/
	m_pSelectedUI = m_pUIManager->Safe_Access_UI(m_pUIManager->Get_CurUIIndex());
	if (nullptr != m_pSelectedUI)
	{
		SetUp_Public_Info();
		Input_TextureTag();

		SetUp_Func();
		Edit_Action();
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
	ImGui::EndChild();
	ImGui::PopID();
}

void CUI_Inspector::Input_TextureTag()
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
				_wstring wstrFolderName = f.parent_path().filename().wstring();
				uint32_t iFileIndex = std::stoi( f.stem().wstring());

				m_pSelectedUI->Set_TextureIndex(iFileIndex);
			}
		}
	}
}

void CUI_Inspector::Add_Action(DTO::EUIEvent EventType)
{
	if (ImGui::Button("Add Action"))
		ImGui::OpenPopup("##AddActionPopUp");

	if (ImGui::BeginPopup("##AddActionPopUp"))
	{
		ImGui::TextDisabled("Select component to add");
		ImGui::Separator();

		const _string& strSET_VISIBLE = DTO::UIFunctypeToString(DTO::EUIFunc::SET_VISIBLE);
		if (ImGui::Selectable(strSET_VISIBLE.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIFunc::SET_VISIBLE, json{ {"isVisible", true} });

			ImGui::CloseCurrentPopup();
		}

		const _string& strSET_TEXTURE_INDEX = DTO::UIFunctypeToString(DTO::EUIFunc::SET_TEXTURE_INDEX);
		if (ImGui::Selectable(strSET_TEXTURE_INDEX.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIFunc::SET_TEXTURE_INDEX, json{ {"index", 0u} });

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void CUI_Inspector::Edit_Action()
{
	switch (m_eCurEditFunc)
	{
	case DTO::EUIFunc::SET_VISIBLE:
	{
		auto* pVec = m_pSelectedUI->Safe_Access_EventData(m_eCurEditEvent);
		if (pVec && !pVec->empty())
		{
			bool isVisible = (*pVec)[0].Params.value("isVisible", true);

			if (ImGui::Checkbox("isVisible", &isVisible))
				(*pVec)[0].Params["isVisible"] = isVisible;
		}

		break;
	}
	case DTO::EUIFunc::SET_TEXTURE_INDEX:
	{
		auto* pVec = m_pSelectedUI->Safe_Access_EventData(m_eCurEditEvent);
		if (nullptr == pVec || pVec->empty())
			break;

		auto& j = (*pVec)[0].Params;

		int iValue = static_cast<int>(j.value("index", 0u));

		if (ImGui::InputInt("index", &iValue))
		{
			if (iValue < 0) iValue = 0;
			j["index"] = static_cast<uint32_t>(iValue);
		}

		break;
	}
	case DTO::EUIFunc::END:
		break;
	default:
		break;
	}
}

void CUI_Inspector::SetUp_Func()
{
	if (ImGui::Button("Hover Enter Action", ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::HOVER_ENTER;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}
	else if (ImGui::Button("Hover Exit Action",ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::HOVER_EXIT;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}
	else if (ImGui::Button("Hovering Action", ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::HOVERING;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}
	else if (ImGui::Button("None Action", ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::NONE;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}
	else if (ImGui::Button("Press Enter Action", ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::PRESS_ENTER;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}
	else if (ImGui::Button("Press Exit Action", ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::PRESS_EXIT;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}
	else if (ImGui::Button("Pressing Action", ImVec2(0.f, 30.f)))
	{
		m_eCurEditEvent = DTO::EUIEvent::PRESSING;
		m_eCurEditFunc = DTO::EUIFunc::END;
	}

	Add_Action(m_eCurEditEvent);
	Action_List(m_eCurEditEvent);

	if (ImGui::Button("Apply Action", ImVec2(0.f, 0.f)))
	{
		m_pSelectedUI->ReBind_Action();
	}
}

void CUI_Inspector::Action_List(DTO::EUIEvent eType)
{
	ImGui::SeparatorText("Action List");
	const size_t iSelected = ENUM_TO_SZET(m_eCurEditFunc);
	auto* pVec = m_pSelectedUI->Safe_Access_EventData(m_eCurEditEvent);
	
	if (nullptr != pVec)
	{
		ImGui::BeginChild("ItemList", ImVec2(0, 100.f), true);

		if (pVec->empty())
		{
			ImGui::Text("Empty...");
		}
		else
		{
			for (size_t i = 0; i < pVec->size(); ++i)
			{
				const bool selected = (iSelected == i);
				if (ImGui::Selectable((*pVec)[i].strActionKey.c_str(), selected))
					m_eCurEditFunc = DTO::StringToUIFunctype((*pVec)[i].strActionKey);
			}
		}
		ImGui::EndChild();
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


