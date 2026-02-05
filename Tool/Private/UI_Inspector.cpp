#include "pch.h"
#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
#include "UIData_Repository.h"
#include "Engine_Utils.h"
#include "ToolCanvas.h"
#include "ToolLayer.h"
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

		const _string& strSET_VISIBLE = DTO::UIActionTypeToString(DTO::EUIAction::SET_VISIBLE);
		if (ImGui::Selectable(strSET_VISIBLE.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::SET_VISIBLE, json{ {"isVisible", true} });

			ImGui::CloseCurrentPopup();
		}

		const _string& strSET_TEXTURE_INDEX = DTO::UIActionTypeToString(DTO::EUIAction::SET_TEXTURE_INDEX);
		if (ImGui::Selectable(strSET_TEXTURE_INDEX.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::SET_TEXTURE_INDEX, json{ {"uIndex", 0u} });

			ImGui::CloseCurrentPopup();
		}

		const _string& strSTART_LERP_MOVEMENT = DTO::UIActionTypeToString(DTO::EUIAction::START_LERP_MOVEMENT);
		if (ImGui::Selectable(strSTART_LERP_MOVEMENT.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::START_LERP_MOVEMENT, json{
		{ "vTargetPos", { { "x", 0.f }, { "y", 0.f }, { "z", 0.f } } },
		{ "fTargetAlpha", 1.f },
		{ "fDuration", 0.25f },
		{ "isPin", false}
				});

			ImGui::CloseCurrentPopup();
		}

		const _string& strTRIGGER_ALL_CANVAS = DTO::UIActionTypeToString(DTO::EUIAction::TRIGGER_ALL_CANVAS);
		if (ImGui::Selectable(strTRIGGER_ALL_CANVAS.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::TRIGGER_ALL_CANVAS,
				json{ {"iLevelIndex", 0u}, { "strCanvasTag", "" }, {"eAction", DTO::EUIAction::END}, { "jTargetActionParam", json::object() } });

			ImGui::CloseCurrentPopup();
		}
		const _string& strTRIGGER_ALL_LAYER = DTO::UIActionTypeToString(DTO::EUIAction::TRIGGER_ALL_LAYER);
		if (ImGui::Selectable(strTRIGGER_ALL_LAYER.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::TRIGGER_ALL_LAYER, 
				json{ {"iLevelIndex", 0u}, { "strLayerTag", "" }, {"eAction", DTO::EUIAction::END}, { "jTargetActionParam", json::object() } });

			ImGui::CloseCurrentPopup();
		}
		const _string& strTRIGGER_TARGET_UI = DTO::UIActionTypeToString(DTO::EUIAction::TRIGGER_TARGET_UI);
		if (ImGui::Selectable(strTRIGGER_TARGET_UI.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::TRIGGER_TARGET_UI, 
				json{ {"iLevelIndex", 0u}, { "strUITag", "" }, {"eAction", DTO::EUIAction::END}, { "jTargetActionParam", json::object() } });

			ImGui::CloseCurrentPopup();
		}

		const _string& strSTART_RETURN_LERP_MOVEMENT = DTO::UIActionTypeToString(DTO::EUIAction::START_RETURN_LERP_MOVEMENT);
		if (ImGui::Selectable(strSTART_RETURN_LERP_MOVEMENT.c_str()))
		{
			m_pSelectedUI->Bind_Action(EventType, DTO::EUIAction::START_RETURN_LERP_MOVEMENT,json{});

			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void CUI_Inspector::Edit_Action()
{
	json* j = Find_Params(m_eCurEditFunc);
	if (nullptr == j)
		return;

	switch (m_eCurEditFunc)
	{
	case DTO::EUIAction::SET_VISIBLE:Edit_Set_Visible(*j);break;
	case DTO::EUIAction::SET_TEXTURE_INDEX:Edit_Set_Texture_Index(*j);break;
	case DTO::EUIAction::START_LERP_MOVEMENT:Edit_Start_Lerp_Movement(*j);break;
	case DTO::EUIAction::TRIGGER_ALL_CANVAS:Trigger_All_Canvas(*j); break;
	case DTO::EUIAction::TRIGGER_ALL_LAYER: Trigger_All_Layer(*j); break;
	case DTO::EUIAction::TRIGGER_TARGET_UI: Trigger_Target_UI(*j); break;
	case DTO::EUIAction::START_RETURN_LERP_MOVEMENT: Edit_Start_Return_Lerp_Movement(*j); break;
	case DTO::EUIAction::END:
		break;
	default:
		break;
	}
}

void CUI_Inspector::SetUp_Func()
{
	const float h = 20.f;
	const float w = ImGui::GetContentRegionAvail().x;

	if (ImGui::Button("Hover Enter Action", ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::HOVER_ENTER; m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("Hover Exit Action",	ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::HOVER_EXIT;  m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("Hovering Action",	ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::HOVERING;    m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("None Action",		ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::NONE;        m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("Press Enter Action", ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::PRESS_ENTER; m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("Press Exit Action",	ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::PRESS_EXIT;  m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("Pressing Action",	ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::PRESSING;    m_eCurEditFunc = DTO::EUIAction::END; }
	if (ImGui::Button("Invoked Action",		ImVec2(w, h))) { m_eCurEditEvent = DTO::EUIEvent::INVOKED;     m_eCurEditFunc = DTO::EUIAction::END; }

	Add_Action(m_eCurEditEvent);
	Action_List(m_eCurEditEvent);

	if (ImGui::Button("Apply Action", ImVec2(0.f, 0.f)))
	{
		m_pSelectedUI->ReBind_Action();
	}
}

void CUI_Inspector::Action_List(DTO::EUIEvent eType)
{

	ImGui::SeparatorText(DTO::UIEventToString(m_eCurEditEvent).c_str());
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
				if (ImGui::Selectable( DTO::UIActionTypeToString( (*pVec)[i].eAction).c_str(), selected))
					m_eCurEditFunc = DTO::StringToUIActiontype(DTO::UIActionTypeToString((*pVec)[i].eAction));
			}
		}
		ImGui::EndChild();
	}
}

json* CUI_Inspector::Find_Params(const DTO::EUIAction eAction)
{
	auto* pVec = m_pSelectedUI->Safe_Access_EventData(m_eCurEditEvent);
	if (!pVec || pVec->empty())
		return nullptr;

	for (auto& data : *pVec)
	{
		if (data.eAction == eAction)
			return &data.Params;
	}
	return nullptr;
}

void CUI_Inspector::Edit_Set_Visible(json& jParams)
{
	if (!jParams.contains("isVisible"))
		return;

	bool isVisible = jParams.value("isVisible", true);

	if (ImGui::Checkbox("isVisible", &isVisible))
		jParams["isVisible"] = isVisible;
}

void CUI_Inspector::Edit_Set_Texture_Index(json& jParams)
{
	if (!jParams.contains("uIndex"))
		return;

	int iValue = static_cast<int>(jParams.value("uIndex", 0u));
	if (ImGui::InputInt("uIndex", &iValue))
	{
		if (iValue < 0) iValue = 0;
		jParams["uIndex"] = static_cast<uint32_t>(iValue);
	}
}

void CUI_Inspector::Edit_Start_Lerp_Movement(json& jParams)
{
	if (!jParams.contains("vTargetPos") || !jParams["vTargetPos"].is_object())
		jParams["vTargetPos"] = json{ {"x", 0.f}, {"y", 0.f}, {"z", 0.f} };

	auto& jPos = jParams["vTargetPos"];

	float v[3] = {
		jPos.value("x", 0.f),
		jPos.value("y", 0.f),
		jPos.value("z", 0.f)
	};

	if (ImGui::InputFloat3("vTargetPos", v))
	{
		jPos["x"] = v[0];
		jPos["y"] = v[1];
		jPos["z"] = v[2];
	}

	if (ImGui::Button("Apply to Client Aspect"))
	{
		jPos["x"] = jPos["x"] * (1280.f / (_float)g_iWinSizeX);
		jPos["y"] = jPos["y"] * (720.f / (_float)g_iWinSizeY);
		jPos["z"] = jPos["z"] * 1.f;
	}

	if (!jParams.contains("fTargetAlpha"))
		return;
	float fTargetAlpha = jParams.value("fTargetAlpha", 1.f);
	if (ImGui::InputFloat("fTargetAlpha", &fTargetAlpha))
	{
		if (fTargetAlpha < 0.f) fTargetAlpha = 0.f;
		jParams["fTargetAlpha"] = fTargetAlpha;
	}

	if (!jParams.contains("fDuration"))
		return;
	float fDuration = jParams.value("fDuration", 0.f);
	if (ImGui::InputFloat("fDuration", &fDuration))
	{
		if (fDuration < 0.f) fDuration = 0.f;
		jParams["fDuration"] = fDuration;
	}

	if (!jParams.contains("isPin"))
		return;
	bool isPin = jParams.value("isPin", false);
	if (ImGui::Checkbox("isPin", &isPin))
		jParams["isPin"] = isPin;
}

void CUI_Inspector::Trigger_All_Canvas(json& jParams)
{
	ImGui::PushID("TRIGGER_ALL_CANVAS");

	//{
	//	int iLevel = static_cast<int>(jParams.value("iLevelIndex", 0u));
	//	if (ImGui::InputInt("iLevelIndex", &iLevel))
	//	{
	//		if (iLevel < 0) iLevel = 0;
	//		jParams["iLevelIndex"] = static_cast<uint32_t>(iLevel);
	//	}
	//}

	auto* pCanvasVec = m_pUIManager->Safe_Access_CanvasVector();
	if (nullptr != pCanvasVec)
	{
		vector<_string> VecCanvasTag;
		for (auto* pCanvas : *pCanvasVec)
			VecCanvasTag.push_back(pCanvas->Get_Tag());

		if (!jParams.contains("strCanvasTag") || !jParams["strCanvasTag"].is_string())
			jParams["strCanvasTag"] = VecCanvasTag.empty() ? "" : VecCanvasTag[0];

		_string curTag = jParams["strCanvasTag"].get<_string>();

		int curIndex = 0;
		for (int i = 0; i < (int)VecCanvasTag.size(); ++i)
		{
			if (VecCanvasTag[i] == curTag) { curIndex = i; break; }
		}

		const char* preview = VecCanvasTag.empty() ? "" : VecCanvasTag[curIndex].c_str();
		if (ImGui::BeginCombo("strCanvasTag", preview))
		{
			for (int i = 0; i < (int)VecCanvasTag.size(); ++i)
			{
				const bool isSelected = (i == curIndex);
				if (ImGui::Selectable(VecCanvasTag[i].c_str(), isSelected))
				{
					curIndex = i;
					jParams["strCanvasTag"] = VecCanvasTag[i];
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	if (!jParams.contains("eAction"))
		jParams["eAction"] = m_ArrUIAction[0];

	DTO::EUIAction curAct = jParams.value("eAction", m_ArrUIAction[0]);
	DTO::EUIAction newAct = curAct;

	{
		const _string curName = DTO::UIActionTypeToString(curAct);
		if (ImGui::BeginCombo("eAction", curName.c_str()))
		{
			for (auto act : m_ArrUIAction)
			{
				const _string name = DTO::UIActionTypeToString(act);
				const bool isSelected = (act == curAct);
				if (ImGui::Selectable(name.c_str(), isSelected))
					newAct = act;
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	if (!jParams.contains("jTargetActionParam") || !jParams["jTargetActionParam"].is_object())
		jParams["jTargetActionParam"] = json::object();

	if (newAct != curAct)
	{
		jParams["eAction"] = newAct;
		curAct = newAct;

		switch (curAct)
		{
		case DTO::EUIAction::SET_VISIBLE:
			jParams["jTargetActionParam"] = json{ {"isVisible", true} };
			break;
		case DTO::EUIAction::SET_TEXTURE_INDEX:
			jParams["jTargetActionParam"] = json{ {"uIndex", 0u} };
			break;
		case DTO::EUIAction::START_LERP_MOVEMENT:
			jParams["jTargetActionParam"] = json{
				{ "vTargetPos", { { "x", 0.f }, { "y", 0.f }, { "z", 0.f } } },
				{ "fTargetAlpha", 1.f },
				{ "fDuration", 0.25f },
				{ "isPin", false }
			};
			break;
		case DTO::EUIAction::START_RETURN_LERP_MOVEMENT:
			break;
		default:
			jParams["jTargetActionParam"] = json::object();
			break;
		}
	}

	auto& inner = jParams["jTargetActionParam"];

	switch (curAct)
	{
	case DTO::EUIAction::SET_VISIBLE:
		if (!inner.contains("isVisible")) inner["isVisible"] = true;
		Edit_Set_Visible(inner);
		break;

	case DTO::EUIAction::SET_TEXTURE_INDEX:
		if (!inner.contains("uIndex")) inner["uIndex"] = 0u;
		Edit_Set_Texture_Index(inner);
		break;

	case DTO::EUIAction::START_LERP_MOVEMENT:
		if (!inner.contains("vTargetPos") || !inner["vTargetPos"].is_object())
			inner["vTargetPos"] = json{ {"x", 0.f}, {"y", 0.f}, {"z", 0.f} };
		if (!inner.contains("fTargetAlpha")) inner["fTargetAlpha"] = 1.f;
		if (!inner.contains("fDuration")) inner["fDuration"] = 0.25f;
		if (!inner.contains("isPin")) inner["isPin"] = false;
		Edit_Start_Lerp_Movement(inner);
		break;
	case DTO::EUIAction::START_RETURN_LERP_MOVEMENT:
		Edit_Start_Lerp_Movement(inner);
		break;
	default:
		ImGui::TextDisabled("No editable params for this action.");
		break;
	}

	ImGui::PopID();
}

void CUI_Inspector::Trigger_All_Layer(json& jParams)
{
	ImGui::PushID("TRIGGER_ALL_LAYER");

	//{
	//	int iLevel = static_cast<int>(jParams.value("iLevelIndex", 0u));
	//	if (ImGui::InputInt("iLevelIndex", &iLevel))
	//	{
	//		if (iLevel < 0) iLevel = 0;
	//		jParams["iLevelIndex"] = static_cast<uint32_t>(iLevel);
	//	}
	//}

	// 1) Layer Tag
	auto* pCanvasVec = m_pUIManager->Safe_Access_CanvasVector();
	if (nullptr != pCanvasVec)
	{
		vector<_string> VecLayerTag;
		for (auto* pCanvas : *pCanvasVec)
		{
			auto* pLayerVec = pCanvas->Safe_Access_LayerObject_Vector_Ptr();
			if (nullptr == pLayerVec)
				continue;

			for (auto* pLayer : *pLayerVec)
				VecLayerTag.push_back(pLayer->Get_Name());
		}

		if (!jParams.contains("strLayerTag") || !jParams["strLayerTag"].is_string())
			jParams["strLayerTag"] = VecLayerTag.empty() ? "" : VecLayerTag[0];

		_string curTag = jParams["strLayerTag"].get<_string>();

		int curIndex = 0;
		for (int i = 0; i < (int)VecLayerTag.size(); ++i)
		{
			if (VecLayerTag[i] == curTag) { curIndex = i; break; }
		}

		const char* preview = VecLayerTag.empty() ? "" : VecLayerTag[curIndex].c_str();
		if (ImGui::BeginCombo("strLayerTag", preview))
		{
			for (int i = 0; i < (int)VecLayerTag.size(); ++i)
			{
				const bool isSelected = (i == curIndex);
				if (ImGui::Selectable(VecLayerTag[i].c_str(), isSelected))
				{
					curIndex = i;
					jParams["strLayerTag"] = VecLayerTag[i];
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	if (!jParams.contains("eAction"))
		jParams["eAction"] = m_ArrUIAction[0];

	DTO::EUIAction curAct = jParams.value("eAction", m_ArrUIAction[0]);
	DTO::EUIAction newAct = curAct;

	{
		const _string curName = DTO::UIActionTypeToString(curAct);
		if (ImGui::BeginCombo("eAction", curName.c_str()))
		{
			for (auto act : m_ArrUIAction)
			{
				const _string name = DTO::UIActionTypeToString(act);
				const bool isSelected = (act == curAct);

				if (ImGui::Selectable(name.c_str(), isSelected))
					newAct = act;

				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	// 3) Inner Params default 생성/교체
	if (!jParams.contains("jTargetActionParam") || !jParams["jTargetActionParam"].is_object())
		jParams["jTargetActionParam"] = json::object();

	if (newAct != curAct)
	{
		jParams["eAction"] = newAct;
		curAct = newAct;

		switch (curAct)
		{
		case DTO::EUIAction::SET_VISIBLE:
			jParams["jTargetActionParam"] = json{ {"isVisible", true} };
			break;

		case DTO::EUIAction::SET_TEXTURE_INDEX:
			jParams["jTargetActionParam"] = json{ {"uIndex", 0u} };
			break;

		case DTO::EUIAction::START_LERP_MOVEMENT:
			jParams["jTargetActionParam"] = json{
				{ "vTargetPos", { { "x", 0.f }, { "y", 0.f }, { "z", 0.f } } },
				{ "fTargetAlpha", 1.f },
				{ "fDuration", 0.25f },
				{ "isPin", false }
			};
			break;
		case DTO::EUIAction::START_RETURN_LERP_MOVEMENT:
			break;
		default:
			jParams["jTargetActionParam"] = json::object();
			break;
		}
	}

	// 4) Inner Params 편집 (기존 Edit_* 재사용)
	auto& inner = jParams["jTargetActionParam"];

	switch (curAct)
	{
	case DTO::EUIAction::SET_VISIBLE:
		if (!inner.contains("isVisible")) inner["isVisible"] = true;
		Edit_Set_Visible(inner);
		break;

	case DTO::EUIAction::SET_TEXTURE_INDEX:
		if (!inner.contains("uIndex")) inner["uIndex"] = 0u;
		Edit_Set_Texture_Index(inner);
		break;

	case DTO::EUIAction::START_LERP_MOVEMENT:
		if (!inner.contains("vTargetPos") || !inner["vTargetPos"].is_object())
			inner["vTargetPos"] = json{ {"x", 0.f}, {"y", 0.f}, {"z", 0.f} };
		if (!inner.contains("fTargetAlpha")) inner["fTargetAlpha"] = 1.f;
		if (!inner.contains("fDuration")) inner["fDuration"] = 0.25f;
		if (!inner.contains("isPin")) inner["isPin"] = false;
		Edit_Start_Lerp_Movement(inner);
		break;
	case DTO::EUIAction::START_RETURN_LERP_MOVEMENT:
		Edit_Start_Lerp_Movement(inner);
		break;
	default:
		ImGui::TextDisabled("No editable params for this action.");
		break;
	}
	ImGui::PopID();
}

void CUI_Inspector::Trigger_Target_UI(json& jParams)
{
	ImGui::PushID("TRIGGER_TARGET_UI");

	//{
	//	int iLevel = static_cast<int>(jParams.value("iLevelIndex", 0u));
	//	if (ImGui::InputInt("iLevelIndex", &iLevel))
	//	{
	//		if (iLevel < 0) iLevel = 0;
	//		jParams["iLevelIndex"] = static_cast<uint32_t>(iLevel);
	//	}
	//}

	// 1) UI Tag
	auto* pCanvasVec = m_pUIManager->Safe_Access_CanvasVector();
	if (nullptr != pCanvasVec)
	{
		vector<_string> VecUITag;
		for (auto* pCanvas : *pCanvasVec)
		{
			auto* pLayerVec = pCanvas->Safe_Access_LayerObject_Vector_Ptr();
			if (nullptr == pLayerVec)
				continue;

			for (auto* pLayer : *pLayerVec)
			{
				auto* pUIvec = pLayer->Safe_Access_UIObject_Vector_Ptr();
				if (nullptr == pUIvec)
					continue;

				for (auto* pUI : *pUIvec)
					VecUITag.push_back(pUI->Get_Name());
			}
		}

		if (!jParams.contains("strUITag") || !jParams["strUITag"].is_string())
			jParams["strUITag"] = VecUITag.empty() ? "" : VecUITag[0];

		_string curTag = jParams["strUITag"].get<_string>();

		int curIndex = 0;
		for (int i = 0; i < (int)VecUITag.size(); ++i)
		{
			if (VecUITag[i] == curTag) { curIndex = i; break; }
		}

		const char* preview = VecUITag.empty() ? "" : VecUITag[curIndex].c_str();
		if (ImGui::BeginCombo("strUITag", preview))
		{
			for (int i = 0; i < (int)VecUITag.size(); ++i)
			{
				const bool isSelected = (i == curIndex);
				if (ImGui::Selectable(VecUITag[i].c_str(), isSelected))
				{
					curIndex = i;
					jParams["strUITag"] = VecUITag[i];
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	if (!jParams.contains("eAction"))
		jParams["eAction"] = m_ArrUIAction[0];

	DTO::EUIAction curAct = jParams.value("eAction", m_ArrUIAction[0]);
	DTO::EUIAction newAct = curAct;

	{
		const _string curName = DTO::UIActionTypeToString(curAct);
		if (ImGui::BeginCombo("eAction", curName.c_str()))
		{
			for (auto act : m_ArrUIAction)
			{
				const _string name = DTO::UIActionTypeToString(act);
				const bool isSelected = (act == curAct);

				if (ImGui::Selectable(name.c_str(), isSelected))
					newAct = act;

				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	// 3) Inner Params default 생성/교체
	if (!jParams.contains("jTargetActionParam") || !jParams["jTargetActionParam"].is_object())
		jParams["jTargetActionParam"] = json::object();

	if (newAct != curAct)
	{
		jParams["eAction"] = newAct;
		curAct = newAct;

		switch (curAct)
		{
		case DTO::EUIAction::SET_VISIBLE:
			jParams["jTargetActionParam"] = json{ {"isVisible", true} };
			break;

		case DTO::EUIAction::SET_TEXTURE_INDEX:
			jParams["jTargetActionParam"] = json{ {"uIndex", 0u} };
			break;

		case DTO::EUIAction::START_LERP_MOVEMENT:
			jParams["jTargetActionParam"] = json{
				{ "vTargetPos", { { "x", 0.f }, { "y", 0.f }, { "z", 0.f } } },
				{ "fTargetAlpha", 1.f },
				{ "fDuration", 0.25f },
				{ "isPin", false }
			};
			break;
		
		case DTO::EUIAction::START_RETURN_LERP_MOVEMENT:
			break;

		default:
			jParams["jTargetActionParam"] = json::object();
			break;
		}
	}

	// 4) Inner Params 편집 (기존 Edit_* 재사용)
	auto& inner = jParams["jTargetActionParam"];

	switch (curAct)
	{
	case DTO::EUIAction::SET_VISIBLE:
		if (!inner.contains("isVisible")) inner["isVisible"] = true;
		Edit_Set_Visible(inner);
		break;

	case DTO::EUIAction::SET_TEXTURE_INDEX:
		if (!inner.contains("uIndex")) inner["uIndex"] = 0u;
		Edit_Set_Texture_Index(inner);
		break;

	case DTO::EUIAction::START_LERP_MOVEMENT:
		if (!inner.contains("vTargetPos") || !inner["vTargetPos"].is_object())
			inner["vTargetPos"] = json{ {"x", 0.f}, {"y", 0.f}, {"z", 0.f} };
		if (!inner.contains("fTargetAlpha")) inner["fTargetAlpha"] = 1.f;
		if (!inner.contains("fDuration")) inner["fDuration"] = 0.25f;
		if (!inner.contains("isPin")) inner["isPin"] = false;
		Edit_Start_Lerp_Movement(inner);
		break;

	case DTO::EUIAction::START_RETURN_LERP_MOVEMENT:
		Edit_Start_Return_Lerp_Movement(inner);
		break;
	default:
		ImGui::TextDisabled("No editable params for this action.");
		break;
	}

	ImGui::PopID();
}

void CUI_Inspector::Edit_Start_Return_Lerp_Movement(json& jParams)
{

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


