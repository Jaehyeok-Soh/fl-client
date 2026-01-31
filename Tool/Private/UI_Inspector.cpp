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

	Folder_Search("../../Resources/Textures/UI");
	File_Search("../../Resources/Textures/UI");

	return S_OK;
}

void CUI_Inspector::Update(const _float fTimeDelta)
{
}

HRESULT CUI_Inspector::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str(), nullptr, m_Flag);

	Setting_Texture();

	m_pSelectedUI = m_pUIManager->Safe_Access_UI(m_pUIManager->Get_CurUIIndex());
	if (nullptr != m_pSelectedUI)
	{
		SetUp_Public_Info();
		Input_TextureTag();
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
	ImGui::BeginChild("RectTransformCard", ImVec2(-FLT_MIN, 168.f), true, ImGuiWindowFlags_NoScrollbar);
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
	ImGui::BeginChild("RectTransformValuesCard", ImVec2(-FLT_MIN, 112.f), true, ImGuiWindowFlags_NoScrollbar);

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

				CTexture* pTexture = dynamic_cast<CTexture*>(dynamic_cast<CComponent*>(
					CGameInstance::GetInstance()->Clone_Prototype(EPrototypeType::COMPONENT, static_cast<uint32_t>(ELevelType::UI),L"Texture_" + wstrFolderName)));
				if (nullptr == pTexture)
					return;

				m_pSelectedUI-> Change_Component<CTexture>(pTexture);
				m_pSelectedUI->Set_TextureIndex(iFileIndex);
				//m_pUIManager->Safe_Access_UIData_Ptr(m_pUIManager->Get_CurUIIndex())->strTextureTag = Engine_Utils::ToString( L"Texture_" + wstrFolderName);
				//m_pUIManager->Safe_Access_UIData_Ptr(m_pUIManager->Get_CurUIIndex())->iTextureIndex = iFileIndex;
			}
		}
	}

}

HRESULT CUI_Inspector::Setting_Texture()
{	
	// ===========   Diffse Texture  ============
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Diffuse Texture"); ImGui::SameLine();

	if (ImGui::Button("Open Texture Selector##Diffuse_Texture"))
		ImGui::OpenPopup("TextureSelector##Diffuse_Texture");

	ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_Appearing);

	if (ImGui::BeginPopupModal("TextureSelector##Diffuse_Texture", NULL))
	{
		ImGui::BeginChild("FolderList", ImVec2(180, 0), true);
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Category");
			ImGui::Separator();

			for (auto& folderName : m_TextureFolderNames)
			{
				// 현재 선택된 폴더면 하이라이트 효과
				if (ImGui::Selectable(folderName.c_str(), m_strSelectedFolder == folderName))
				{
					m_strSelectedFolder = folderName;
				}
			}
		}
		ImGui::EndChild();

		ImGui::SameLine();

		// --- 우측 : 선택된 폴더 내 텍스처 그리드 창 ---
		ImGui::BeginChild("TextureGrid", ImVec2(0, 0), true);
		{
			ImGui::Text("Folder: %s", m_strSelectedFolder.empty() ? "None" : m_strSelectedFolder.c_str());
			ImGui::Separator();

			if (!m_strSelectedFolder.empty() && m_TextureMap.count(m_strSelectedFolder))
			{
				auto& fileList = m_TextureMap[m_strSelectedFolder];
				int columns = 5; // 한 줄에 5장씩

				for (int i = 0; i < fileList.size(); ++i)
				{
					string fullPath = fileList[i].first;
					string fileName = fileList[i].second;

					// 리소스 매니저에서 텍스처 가져오기 (태그 명명 규칙 확인해봐!)
					wstring textureTag = L"Texture_" + Engine_Utils::ToWString(fileName);
					CTextureBase* pTexture = CGameInstance::GetInstance()->Get_Resource<CTextureBase>(textureTag);
					ID3D11ShaderResourceView* pSRV = (pTexture) ? pTexture->Get_SRV() : nullptr;

					ImGui::PushID(i);
					ImGui::BeginGroup();

					if (pSRV)
					{
						// 이미지 버튼 크기를 64x64 정도로 키움
						if (ImGui::ImageButton("##texBtn", (ImTextureID)pSRV, ImVec2(64, 64)))
						{
							ImGui::EndGroup(); ImGui::PopID();
							ImGui::CloseCurrentPopup();
							break;
						}
					}
					else
					{
						ImGui::Button("No Res", ImVec2(64, 64));
					}

					// 파일명이 너무 길면 잘라서 출력
					string display = (fileName.length() > 10) ? fileName.substr(0, 8) + ".." : fileName;
					ImGui::Text(display.c_str());

					ImGui::EndGroup();
					ImGui::PopID();

					// 가로로 5장 배치 로직
					if ((i + 1) % columns != 0) ImGui::SameLine(0, 10.f);

					Safe_Release(pTexture);
				}
			}
		}
		ImGui::EndChild();

		ImGui::Separator();
		if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }

		ImGui::EndPopup();
	}

	ImGui::Spacing();
	return S_OK;
}

HRESULT CUI_Inspector::File_Search(const string& RootPath)
{
	// 기존 데이터 초기화 (필요시)
	m_TextureMap.clear();
	m_TextureFolderNames.clear();

	namespace fs = std::filesystem;

	if (!fs::exists(RootPath)) return E_FAIL;

	for (auto& iter : fs::recursive_directory_iterator(RootPath))
	{
		// 파일인 경우에만 처리
		if (iter.is_regular_file())
		{
			auto fullPath = iter.path();
			string strFullPath = Engine_Utils::ToString(fullPath);
			string fileName = fullPath.filename().string();
			string pureFileName = Engine_Utils::GetFileNameWithoutExtension(strFullPath);

			// 파일이 속한 바로 위 폴더 이름 추출
			string folderName = fullPath.parent_path().filename().string();

			string ext = fullPath.extension().string();
			for (auto& c : ext) c = tolower(c);

			if (ext == ".png" || ext == ".dds" || ext == ".tga" || ext == ".jpg")
			{
				m_TextureMap[folderName].push_back(make_pair(strFullPath, pureFileName));

				if (find(m_TextureFolderNames.begin(), m_TextureFolderNames.end(), folderName) == m_TextureFolderNames.end())
				{
					m_TextureFolderNames.push_back(folderName);
				}
			}
		}
	}
	return S_OK;
}

HRESULT CUI_Inspector::Folder_Search(const string& Path)
{
	for (auto iter = std::filesystem::recursive_directory_iterator(Path);
		iter != std::filesystem::recursive_directory_iterator();
		++iter)
	{
		// 아 여기에 화살표 그으면 되는구나.

		int depth = iter.depth();
		auto fullpath = iter->path();
		auto FolderName = iter->path().filename();

		if (iter->is_regular_file())
		{
			string m_sFileName = Engine_Utils::GetFileNameWithoutExtension(Engine_Utils::ToString(fullpath));
			m_TextureFileNames.push_back(make_pair(Engine_Utils::ToString(fullpath), m_sFileName));

		}
	}

	return S_OK;
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


