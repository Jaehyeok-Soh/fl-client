#include "pch.h"
#include "UI_Inspector.h"
#include "ImGui_ToolManager.h"
#include "ImGui_UIManager.h"
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

	m_VecOwnerTag.reserve(ENUM_TO_UINT(DTO::EUISubClassType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(DTO::EUISubClassType::END); ++i)
		m_VecOwnerTag.push_back(DTO::UISubClasstypeToString(static_cast<DTO::EUISubClassType>(i)));

	m_VecShaderPassTag.reserve(ENUM_TO_UINT(
		::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(EUIShaderPass::END); ++i)
		m_VecShaderPassTag.push_back(UIShaderPassToString(static_cast<EUIShaderPass>(i)));

	m_VecTextSubClassTag.reserve(ENUM_TO_UINT(DTO::EUITextSubClassType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(DTO::EUITextSubClassType::END); ++i)
		m_VecTextSubClassTag.push_back(DTO::UITextSubClassTypeToString(static_cast<DTO::EUITextSubClassType>(i)));

	m_VecDImageSubClassTag.reserve(ENUM_TO_UINT(DTO::EUIDImageSubClassType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(DTO::EUIDImageSubClassType::END); ++i)
		m_VecDImageSubClassTag.push_back(DTO::UIDImageSubTypeToString(static_cast<DTO::EUIDImageSubClassType>(i)));
	
	m_VecFontPivotTypes.reserve(ENUM_TO_UINT(EFontPivotType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(EFontPivotType::END); ++i)
		m_VecFontPivotTypes.push_back(DTO::FontPivotTypeToString(static_cast<EFontPivotType>(i)));

	m_VecFonstShaderTypes.reserve(ENUM_TO_UINT(EFontShaderType::END));
	for (uint32_t i = 0; i < ENUM_TO_UINT(EFontShaderType::END); ++i)
		m_VecFonstShaderTypes.push_back(DTO::FontShaderTypeToString(static_cast<EFontShaderType>(i)));

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
		SetUp_ShaderPass();
		
		if (Begin_Card("SetUp Default Setting", "##SetUp_Default_Setting", 100.f))
		{
			SetUp_Class();
			SetUp_Owner();
		}
		End_Card();


		if (m_pSelectedUI->Get_UIClassType() == DTO::EUIClassType::UI_TEXT)
		{
			SetUp_TextData();
		}
		else if (m_pSelectedUI->Get_UIClassType() == DTO::EUIClassType::DYNAMIC_IMAGE)
		{
			SetUp_DImageData();
		}
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
	ImGui::BeginChild("RectTransformCard", ImVec2(0, 150.f), true, ImGuiWindowFlags_NoScrollbar);
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
	ImGui::BeginChild("RectTransformValuesCard", ImVec2(0, 300.f), true, ImGuiWindowFlags_NoScrollbar);

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
		if (Scrub_Float("UIPosZ", "##UIObjectPosZ", m_pSelectedUI->Get_PosZ_Ptr(), 0.1f, 20.f, 0.1f, 10.0f, 100.f))
			m_pUIManager->Request_SortUI();

		ImGui::EndTable();
	}

	ImGui::Spacing();

	// Scale / Rotate
	{
		_float fRotate = m_pSelectedUI->Get_Rotate();
		_float fDeg = DirectX::XMConvertToDegrees(fRotate);
		
		// degree로 편집 (min==max => 제한 없음)
		if (ImGui::DragFloat("##UIRotateDeg", &fDeg, 0.1f, 0.f, 0.f, "%.2f deg"))
		{
			const _float fRad = DirectX::XMConvertToRadians(fDeg);
			m_pSelectedUI->Set_Rotate(fRad);
		}
		ImGui::SameLine();
		if (ImGui::Button("0.0##RotateReset"))
			m_pSelectedUI->Set_Rotate(0.f);
	}

	ImGui::InputFloat("Alpha", &m_pSelectedUI->Get_AlphaRatio_Ref());
	ImGui::Checkbox("Visible", &m_pSelectedUI->Get_InitVisible());
	ImGui::SameLine();
	ImGui::Checkbox("Interact", &m_pSelectedUI->Get_InitInteractable());
	ImGui::SameLine();
	ImGui::Checkbox("Activate", &m_pSelectedUI->Get_InitActivate());

	if (ImGui::Button("All Visible"))
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
		if (nullptr != pCanvas)
		{
			for (auto* pUI : *(pCanvas->Safe_Access_UI_Vector()))
			{
				if (nullptr == pUI)
					continue;

				pUI->Set_Visible();
			}
		}
	}
	if (ImGui::Button("All InVisible"))
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
		if (nullptr != pCanvas)
		{
			for (auto* pUI : *(pCanvas->Safe_Access_UI_Vector()))
			{
				if (nullptr == pUI)
					continue;

				pUI->Set_Invisible();
			}
		}
	}
	if (ImGui::Button("All Activate"))
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
		if (nullptr != pCanvas)
		{
			for (auto* pUI : *(pCanvas->Safe_Access_UI_Vector()))
			{
				if (nullptr == pUI)
					continue;

				pUI->Set_Activate();
			}
		}
	}
	if (ImGui::Button("All InActivate"))
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
		if (nullptr != pCanvas)
		{
			for (auto* pUI : *(pCanvas->Safe_Access_UI_Vector()))
			{
				if (nullptr == pUI)
					continue;

				pUI->Set_InActivate();
			}
		}
	}

	if (ImGui::Button("All Interact"))
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
		if (nullptr != pCanvas)
		{
			for (auto* pUI : *(pCanvas->Safe_Access_UI_Vector()))
			{
				if (nullptr == pUI)
					continue;

				pUI->Set_Interactable();
			}
		}
	}
	if (ImGui::Button("All NonInteract"))
	{
		auto* pCanvas = m_pUIManager->Safe_Access_Canvas(m_pUIManager->Get_CurCanvasIndex());
		if (nullptr != pCanvas)
		{
			for (auto* pUI : *(pCanvas->Safe_Access_UI_Vector()))
			{
				if (nullptr == pUI)
					continue;

				pUI->Set_NonInteractable();
			}
		}
	}
	ImGui::EndChild();
	ImGui::PopID();
}

void CUI_Inspector::Input_TextureTag(CToolUI::EUITextureSlot eSlot)
{
	_wstring wCurTag;
	_wstring Label = L"";
	const _char* pBtnLabel = "";

	switch (eSlot)
	{
	case CToolUI::EUITextureSlot::DEFAULT:
		wCurTag = m_pSelectedUI->Get_TextureTag();
		Label = L"Cur Texture : ";
		pBtnLabel = "Select Texture##DEFAULT";
		break;

	case CToolUI::EUITextureSlot::NOISE:
		wCurTag = m_pSelectedUI->Get_NoiseTextureTag();
		Label = L"Cur Noise : ";
		pBtnLabel = "Select Texture##NOISE";
		break;

	case CToolUI::EUITextureSlot::ALPHA_MASK:
		wCurTag = m_pSelectedUI->Get_AlphaMaskTextureTag();
		Label = L"Cur AlphaMask : ";
		pBtnLabel = "Select Texture##ALPHA_MASK";
		break;

	case CToolUI::EUITextureSlot::GLOW:
		wCurTag = m_pSelectedUI->Get_GlowTextureTag();
		Label = L"Cur Glow : ";
		pBtnLabel = "Select Texture##GLOW";
		break;

	default:
		break;
	}

	_string str = Engine_Utils::ToString(Label + wCurTag);
	ImGui::TextDisabled(str.c_str());

	if (ImGui::Button(pBtnLabel))
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
				const _wstring wNewTag = L"Texture_" + f.stem().wstring();

				switch (eSlot)
				{
				case CToolUI::EUITextureSlot::DEFAULT:
					m_pSelectedUI->Set_TextureTag(wNewTag);
					m_pSelectedUI->Request_Change_Texture();
					break;

				case CToolUI::EUITextureSlot::NOISE:
					m_pSelectedUI->Set_NoiseTextureTag(wNewTag);
					m_pSelectedUI->Request_Change_NoiseTexture();
					break;

				case CToolUI::EUITextureSlot::ALPHA_MASK:
					m_pSelectedUI->Set_AlphaMaskTextureTag(wNewTag);
					m_pSelectedUI->Request_Change_AlphaMaskTexture();
					break;

				case CToolUI::EUITextureSlot::GLOW:
					m_pSelectedUI->Set_GlowTextureTag(wNewTag);
					m_pSelectedUI->Request_Change_GlowTexture();
					break;
				default:
					break;
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

	int cur = (int)m_pSelectedUI->Get_UISubClassType();
	cur = (cur < 0) ? 0 : (cur >= (int)m_VecOwnerTag.size() ? (int)m_VecOwnerTag.size() - 1 : cur);

	const char* preview = m_VecOwnerTag.empty() ? "" : m_VecOwnerTag[cur].c_str();

	bool changed = false;

	if (ImGui::BeginCombo("UI Sub Class", preview))
	{
		for (int i = 0; i < (int)m_VecOwnerTag.size(); ++i)
		{
			const bool isSelected = (cur == i);
			if (ImGui::Selectable(m_VecOwnerTag[i].c_str(), isSelected))
			{
				cur = i;
				m_pSelectedUI->Set_UISubClassType(static_cast<DTO::EUISubClassType>(i));
				changed = true;
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void CUI_Inspector::SetUp_TextData()
{
	if (Begin_Card("SetUp TextData", "Card_TextData", 250.f))
	{
		_int cur = static_cast<_int>(m_pSelectedUI->Get_UITextSubClassType());
		cur = (cur < 0) ? 0 : (cur >= static_cast<_int>(m_VecTextSubClassTag.size()) ? static_cast<_int>(m_VecTextSubClassTag.size() - 1) : cur);
		const _char* subClassPreview = m_VecTextSubClassTag.empty() ? "" : m_VecTextSubClassTag[cur].c_str();
		_bool changed = false;

		if (ImGui::BeginCombo("Select Text SubClass Type", subClassPreview))
		{
			for (size_t i = 0; i < m_VecTextSubClassTag.size(); ++i)
			{
				const _bool isSelected = (cur == i);
				if (ImGui::Selectable(m_VecTextSubClassTag[i].c_str(), isSelected))
				{
					cur = static_cast<_int>(i);
					m_pSelectedUI->Set_UITextSubClassType(static_cast<DTO::EUITextSubClassType>(i));
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		_int curFontShader = static_cast<_int>(m_pSelectedUI->Get_FontShaderType());
		curFontShader = (curFontShader < 0) ? 0 : (curFontShader >= static_cast<_int>(m_VecFonstShaderTypes.size()) ? static_cast<_int>(m_VecFonstShaderTypes.size() - 1) : curFontShader);
		const _char* fontShaderPreview = m_VecFonstShaderTypes.empty() ? "" : m_VecFonstShaderTypes[curFontShader].c_str();

		if (ImGui::BeginCombo("FontShaderType", fontShaderPreview))
		{
			for (size_t i = 0; i < m_VecFonstShaderTypes.size(); ++i)
			{
				const _bool isSelected = (curFontShader == static_cast<_int>(i));

				if (ImGui::Selectable(m_VecFonstShaderTypes[i].c_str(), isSelected))
				{
					curFontShader = static_cast<_int>(i);
					m_pSelectedUI->Set_FontShaderType(static_cast<EFontShaderType>(i));
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		_string strText = Engine_Utils::ToString(m_pSelectedUI->Get_Text());
		ImGui::InputText("Text", &strText);
		m_pSelectedUI->Set_Text(Engine_Utils::ToWString(strText));

		auto& vColor = m_pSelectedUI->Get_FontColor();
		float col[4] = { vColor.x, vColor.y, vColor.z, vColor.w };
		if (ImGui::ColorEdit4("FontColor", col))
			vColor = Vec4{ col[0], col[1], col[2], col[3] };

		const auto& vecFontNames = m_pUIManager->Get_FontNames();
		_string strFontName = m_pSelectedUI->Get_FontName();

		const char* preview = strFontName.empty() ? "" : strFontName.c_str();

		if (ImGui::BeginCombo("FontName", preview))
		{
			for (size_t i = 0; i < vecFontNames.size(); ++i)
			{
				const bool isSelected = (strFontName == vecFontNames[i]);

				if (ImGui::Selectable(vecFontNames[i].c_str(), isSelected))
				{
					strFontName = vecFontNames[i];
					m_pSelectedUI->Set_FontName(strFontName);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		EFontPivotType ePivot = m_pSelectedUI->Get_FontPivotType();
		const char* prePivotType = DTO::FontPivotTypeToString(ePivot);

		if (ImGui::BeginCombo("FontPivotType", prePivotType))
		{
			for (uint32_t i = 0; i < ENUM_TO_UINT(EFontPivotType::END); ++i)
			{
				const bool isSelected = (ENUM_TO_UINT(ePivot) == i);

				if (ImGui::Selectable(m_VecFontPivotTypes[i].c_str(), isSelected))
				{
					ePivot = static_cast<EFontPivotType>(i);
					m_pSelectedUI->Set_FontPivotType(ePivot);
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}


		_float fScale = m_pSelectedUI->Get_FontScale();
		if (ImGui::InputFloat("FontScale", &fScale))
			m_pSelectedUI->Set_FontScale(fScale);

		_float fRotateRad = m_pSelectedUI->Get_FontRotate();
		_float fRotateDeg = DirectX::XMConvertToDegrees(fRotateRad);

		if (ImGui::SliderFloat("FontRotate", &fRotateDeg, 0.f, 360.f, "%.1f deg"))
		{
			fRotateRad = DirectX::XMConvertToRadians(fRotateDeg);
			m_pSelectedUI->Set_FontRotate(fRotateRad);
		}
	}

	{
		_int i = m_pSelectedUI->Get_iTextParmas0();
		ImGui::InputInt("Set iParam0", &i);
		m_pSelectedUI->Set_iTextParams0(i);
	}

	End_Card();
}

void CUI_Inspector::SetUp_DImageData()
{
	if (Begin_Card("Dynamic Image", "DImage", 100.f))
	{
		if (nullptr == m_pSelectedUI)
			return;

		_int cur = static_cast<_int>(m_pSelectedUI->Get_UIDImageSubClassType());
		cur = (cur < 0) ? 0 : (cur >= static_cast<_int>(m_VecDImageSubClassTag.size()) ? static_cast<_int>(m_VecDImageSubClassTag.size() - 1) : cur);
		
		const _char* preview = m_VecDImageSubClassTag.empty() ? "" : m_VecDImageSubClassTag[cur].c_str();

		_bool changed = false;

		if (ImGui::BeginCombo("Select Dynamic Image SubClass Type", preview))
		{
			for (size_t i = 0; i < m_VecDImageSubClassTag.size(); ++i)
			{
				const _bool isSelected = (cur == i);
				if (ImGui::Selectable(m_VecDImageSubClassTag[i].c_str(), isSelected))
				{
					cur = (_int)i;
					m_pSelectedUI->Set_UIDImageSubClassType(static_cast<DTO::EUIDImageSubClassType>(i));
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		{
			_int i = m_pSelectedUI->Get_iParmas0();
			ImGui::InputInt("Set iParam0", &i);
			m_pSelectedUI->Set_iParams0(i);
		}

	}
	End_Card();
}

void CUI_Inspector::SetUp_ShaderPass()
{
	if(Begin_Card("Set ShaderPass", "Shader_Pass", 250.f))
	{
		if (nullptr == m_pSelectedUI)
			return;

		int cur = (int)m_pSelectedUI->Get_ShaderPass();
		cur = (cur < 0) ? 0 : (cur >= (int)m_VecShaderPassTag.size() ? (int)m_VecShaderPassTag.size() - 1 : cur);

		const char* preview = m_VecShaderPassTag.empty() ? "" : m_VecShaderPassTag[cur].c_str();

		bool changed = false;

		if (ImGui::BeginCombo("Select Shader Pass", preview))
		{
			for (int i = 0; i < (int)m_VecShaderPassTag.size(); ++i)
			{
				const bool isSelected = (cur == i);
				if (ImGui::Selectable(m_VecShaderPassTag[i].c_str(), isSelected))
				{
					cur = i;
					m_pSelectedUI->Set_ShaderPass((i));
					changed = true;
				}

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("No Flip"))
		{
			m_pSelectedUI->Set_Flip(ENUM_TO_UINT(EUIFlip::NONE));
		}
		ImGui::SameLine();
		if (ImGui::Button("Flip X")) {
			m_pSelectedUI->Set_Flip(ENUM_TO_UINT(EUIFlip::FLIP_X));
		}
		ImGui::SameLine();
		if (ImGui::Button("Flip Y")) {
			m_pSelectedUI->Set_Flip(ENUM_TO_UINT(EUIFlip::FLIP_Y));
		}
		ImGui::SameLine();
		if (ImGui::Button("Flip XY")) {
			m_pSelectedUI->Set_Flip(ENUM_TO_UINT(EUIFlip::FLIP_XY));
		}
		Input_TextureTag(CToolUI::EUITextureSlot::DEFAULT);
		switch ((EUIShaderPass)cur)
		{
		case EUIShaderPass::DEFAULT:
		{
			_float fAlphaRatio = m_pSelectedUI->Get_AlphaRatio();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Alpha Ratio", &fAlphaRatio, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_AlphaRatio(fAlphaRatio);

			_float fDelay = m_pSelectedUI->Get_Delay();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Delay", &fDelay, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_Delay(fDelay);

			break;
		}
		case EUIShaderPass::COLOR:
		{
			Vec4 vColorTint = m_pSelectedUI->Get_ColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("Color Tint", (float*)&vColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ColorTint(vColorTint);

			Vec4 vGradiantColorTint = m_pSelectedUI->Get_GradiantColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("GradiantColor Tint", (float*)&vGradiantColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_GradiantColorTint(vGradiantColorTint);

			uint32_t iFillDir = m_pSelectedUI->Get_FillDir();
			const char* dirs[] = { "Right", "Left", "Up", "Down" };
			int dir = (iFillDir > 3u) ? 0 : (int)iFillDir;

			ImGui::SetNextItemWidth(150.f);
			if (ImGui::Combo("Fill Dir", &dir, dirs, IM_ARRAYSIZE(dirs)))
				m_pSelectedUI->Set_FillDir((uint32_t)dir);

			_float fAlphaRatio = m_pSelectedUI->Get_AlphaRatio();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Alpha Ratio", &fAlphaRatio, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_AlphaRatio(fAlphaRatio);

			_float fDelay = m_pSelectedUI->Get_Delay();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Delay", &fDelay, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_Delay(fDelay);
			break;
		}
		case EUIShaderPass::PROGRESS:
		{
			_bool isUseColorTint = m_pSelectedUI->Get_isUseColorTint();
			if (ImGui::Checkbox("Use Color Tint", (bool*)&isUseColorTint))
				m_pSelectedUI->Set_isUseColorTint(isUseColorTint);

			Vec4 vColorTint = m_pSelectedUI->Get_ColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("Color Tint", (float*)&vColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ColorTint(vColorTint);

			Vec4 vGradiantColorTint = m_pSelectedUI->Get_GradiantColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("GradiantColor Tint", (float*)&vGradiantColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_GradiantColorTint(vGradiantColorTint);

			_float fProgress = m_pSelectedUI->Get_ProgressRatio();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Progress Ratio", &fProgress, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ProgressRatio(fProgress);

			uint32_t iFillDir = m_pSelectedUI->Get_FillDir();
			const char* dirs[] = { "Right", "Left", "Up", "Down" };
			int dir = (iFillDir > 3u) ? 0 : (int)iFillDir;

			ImGui::SetNextItemWidth(150.f);
			if (ImGui::Combo("Fill Dir", &dir, dirs, IM_ARRAYSIZE(dirs)))
				m_pSelectedUI->Set_FillDir((uint32_t)dir);

			_float fAlphaRatio = m_pSelectedUI->Get_AlphaRatio();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Alpha Ratio", &fAlphaRatio, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_AlphaRatio(fAlphaRatio);

			_float fDelay = m_pSelectedUI->Get_Delay();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Delay", &fDelay, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_Delay(fDelay);

			break;
		}

		case EUIShaderPass::DISOLVE:
		{
			Input_TextureTag(CToolUI::EUITextureSlot::NOISE);
			Input_TextureTag(CToolUI::EUITextureSlot::ALPHA_MASK);

			_float fProgress = m_pSelectedUI->Get_ProgressRatio();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Progress Ratio", &fProgress, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ProgressRatio(fProgress);

			_bool isUseColorTint = m_pSelectedUI->Get_isUseColorTint();
			if (ImGui::Checkbox("Use Color Tint", (bool*)&isUseColorTint))
				m_pSelectedUI->Set_isUseColorTint(isUseColorTint);

			Vec4 vColorTint = m_pSelectedUI->Get_ColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("Color Tint", (float*)&vColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ColorTint(vColorTint);

			Vec4 vGradiantColorTint = m_pSelectedUI->Get_GradiantColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("GradiantColor Tint", (float*)&vGradiantColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_GradiantColorTint(vGradiantColorTint);

			_float fDelay = m_pSelectedUI->Get_Delay();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Delay", &fDelay, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_Delay(fDelay);
			break;
		}

		case EUIShaderPass::NOISE:
		{
			Input_TextureTag(CToolUI::EUITextureSlot::NOISE);

			Vec4 vColorTint = m_pSelectedUI->Get_ColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("Color Tint", (float*)&vColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ColorTint(vColorTint);

			Vec4 vGradiantColorTint = m_pSelectedUI->Get_GradiantColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("GradiantColor Tint", (float*)&vGradiantColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_GradiantColorTint(vGradiantColorTint);
			break;
		}


		case EUIShaderPass::GLOW:
		{
			Input_TextureTag(CToolUI::EUITextureSlot::NOISE);
			Input_TextureTag(CToolUI::EUITextureSlot::GLOW);

			static bool s_isPlayGlow = false;
			static void* s_pPrevSelected = nullptr;

			if (s_pPrevSelected != m_pSelectedUI)
			{
				s_pPrevSelected = m_pSelectedUI;
				s_isPlayGlow = false;
				m_pSelectedUI->Set_Time(0.f);
			}

			if (s_isPlayGlow)
				m_pSelectedUI->Set_Time(m_pSelectedUI->Get_Time() + (_float)ImGui::GetIO().DeltaTime);

			if (ImGui::Button(s_isPlayGlow ? "Stop##Glow" : "Play##Glow"))
			{
				s_isPlayGlow = !s_isPlayGlow;
				if (!s_isPlayGlow)
					m_pSelectedUI->Set_Time(0.f);
			}
			ImGui::SameLine();
			ImGui::TextDisabled("Time: %.3f", m_pSelectedUI->Get_Time());

			_bool isUseColorTint = m_pSelectedUI->Get_isUseColorTint();
			if (ImGui::Checkbox("Use Color Tint", (bool*)&isUseColorTint))
				m_pSelectedUI->Set_isUseColorTint(isUseColorTint);

			Vec4 vColorTint = m_pSelectedUI->Get_ColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("Color Tint", (float*)&vColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_ColorTint(vColorTint);

			Vec4 vGradiantColorTint = m_pSelectedUI->Get_GradiantColorTint();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat4("GradiantColor Tint", (float*)&vGradiantColorTint, 0.01f, 0.f, 1.f))
				m_pSelectedUI->Set_GradiantColorTint(vGradiantColorTint);

			Vec2 vNoiseUVScale = m_pSelectedUI->Get_NoiseUVScale();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat2("Noise UV Scale", (float*)&vNoiseUVScale, 0.01f, 0.f, 100.f))
				m_pSelectedUI->Set_NoiseUVScale(vNoiseUVScale);

			Vec2 vNoiseUVScroll = m_pSelectedUI->Get_NoiseUVScroll();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat2("Noise UV Scroll", (float*)&vNoiseUVScroll, 0.01f, -10.f, 10.f))
				m_pSelectedUI->Set_NoiseUVScroll(vNoiseUVScroll);

			_float fGlowDistort = m_pSelectedUI->Get_GlowDistort();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Glow Distort", &fGlowDistort, 0.0001f, 0.f, 0.1f))
				m_pSelectedUI->Set_GlowDistort(fGlowDistort);

			_float fGlowPulseSpeed = m_pSelectedUI->Get_GlowPulseSpeed();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Glow Pulse Speed", &fGlowPulseSpeed, 0.01f, 0.f, 50.f))
				m_pSelectedUI->Set_GlowPulseSpeed(fGlowPulseSpeed);

			_float fGlowIntensity = m_pSelectedUI->Get_GlowIntensity();
			ImGui::SetNextItemWidth(150.f);
			if (ImGui::DragFloat("Glow Intensity", &fGlowIntensity, 0.01f, 0.f, 50.f))
				m_pSelectedUI->Set_GlowIntensity(fGlowIntensity);

			break;
		}
		default:
			break;
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


