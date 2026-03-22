#include "pch.h"
#include "Panel_AnimModelFile.h"
#include "AnimTool_Manager.h"

CPanel_AnimModelFile::CPanel_AnimModelFile(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext),
	m_pAnimToolManager(CAnimTool_Manager::GetInstance())
{
}

HRESULT CPanel_AnimModelFile::Initialize()
{
	RefreshModelDir();

	return S_OK;
}

HRESULT CPanel_AnimModelFile::Render(CToolObject* pGo)
{
	DirectoryWindow();

	FileWindow();

	ButtonsWindow();

	return S_OK;
}

void CPanel_AnimModelFile::Update(const _float fTimeDelta)
{
}

void CPanel_AnimModelFile::DirectoryWindow()
{
	ImGui::Begin("Directories");

	m_iResourceTreeID = { 0 };
	if (ImGui::TreeNode("Resource Status"))
	{
		if (m_tRootDirectory.directories.empty())
		{
			ImGui::PushID(m_iResourceTreeID);
			if (ImGui::TreeNode("", "None", m_iResourceTreeID++))
			{
				ImGui::Text("Resource Empty");
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		else
		{
			for (auto& dir : m_tRootDirectory.directories)
				SetDirectoryTree(dir, dir.directory);
		}
		ImGui::TreePop();
	}

	ImGui::End();
}

void CPanel_AnimModelFile::FileWindow()
{
	ImGui::Begin("Files");

	ImGuiListClipper clipper;
	clipper.Begin((int)m_files.size());
	while (clipper.Step())
	{
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			ImGui::Text("%d: %s", i, m_files[i].filename().string().c_str());
	}

	ImGui::End();
}

void CPanel_AnimModelFile::ButtonsWindow()
{
	ImGui::Begin("File Buttons");

	if (ImGui::Button("Load Events"))
	{
		OpenLoadModal();
	}

	ImGui::SameLine();

	if (ImGui::Button("Save Events"))
	{
		OpenSaveModal();
	}

	// Load modal
	RenderLoadModal();

	// Save modal
	RenderSaveModal();

	ImGui::End();
}

void CPanel_AnimModelFile::DrawPreTransformMatrix(string id, ANIM_SRT& pretransform)
{
	ImGui::PushID(&pretransform);

	if (ImGui::CollapsingHeader(id.c_str()))
	{
		if (ImGui::Button("Reset PreTransform"))
		{
			pretransform.vScale = Vec3(1.f, 1.f, 1.f);
			pretransform.vRot = Vec3(0.f, 0.f, 0.f);
			pretransform.vTranslation = Vec3(0.f, 0.f, 0.f);
		}

		ImGui::DragFloat3("Scale", &pretransform.vScale.x, 0.01f, 0.01f, 10.f);
		ImGui::DragFloat3("Rotation", &pretransform.vRot.x, 0.01f, 0.01f);
		ImGui::DragFloat3("Translation", &pretransform.vTranslation.x, 0.01f, 0.01f);
	}

	ImGui::PopID();
}

void CPanel_AnimModelFile::OpenFileDialog(char* buffer, const char* filter)
{
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE)
	{
		// 선택한 경로를 버퍼에 복사
		strcpy_s(buffer, 256, ofn.lpstrFile);
	}
}

void CPanel_AnimModelFile::SaveFileDialog(char* buffer, const char* filter)
{
	OPENFILENAMEA ofn;
	char szFile[260] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = filter;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrDefExt = "json";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameA(&ofn) == TRUE)
	{
		// 선택한 경로를 버퍼에 복사
		strcpy_s(buffer, 256, ofn.lpstrFile);
	}
}

void CPanel_AnimModelFile::OpenLoadModal()
{
	// 모달을 열기 전에 상태 초기화 (경로 비우기 등)
	m_tLoadOptions.Reset();

	// 팝업 열기 트리거
	ImGui::OpenPopup("Load Data Manager");
}

void CPanel_AnimModelFile::OpenSaveModal()
{
	// 모달을 열기 전에 상태 초기화 (경로 비우기 등)
	m_tLoadOptions.Reset();

	// 팝업 열기 트리거
	ImGui::OpenPopup("Save Data Manager");
}

void CPanel_AnimModelFile::RenderLoadModal()
{
	// 모달의 중심을 화면 가운데로 설정 (선택 사항)
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Load Data Manager", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// ---------------------------------------------------------
		// 1. 애니메이션 정보 로드 행
		// ---------------------------------------------------------
		ImGui::Checkbox("##AnimCheck", &m_tLoadOptions.bLoadAnimInfo);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Animation Info", m_tLoadOptions.strAnimPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##AnimBtn"))
		{
			OpenFileDialog(m_tLoadOptions.strAnimPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		// ---------------------------------------------------------
		// 2. 충돌체(Hitbox) 정보 로드 행
		// ---------------------------------------------------------
		ImGui::Checkbox("##HitboxCheck", &m_tLoadOptions.bLoadHitbox);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Hitbox Info", m_tLoadOptions.strHitboxPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##HitboxBtn"))
		{
			OpenFileDialog(m_tLoadOptions.strHitboxPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		// ---------------------------------------------------------
		// 3. 이펙트(Effect) 정보 로드 행
		// ---------------------------------------------------------
		ImGui::Checkbox("##EffectCheck", &m_tLoadOptions.bLoadEffect);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Effect Info", m_tLoadOptions.strEffectPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##EffectBtn"))
		{
			OpenFileDialog(m_tLoadOptions.strEffectPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		// ---------------------------------------------------------
		// 4. 사운드(Sound) 정보 로드 행
		// ---------------------------------------------------------
		ImGui::Checkbox("##SoundCheck", &m_tLoadOptions.bLoadSound);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Sound Info", m_tLoadOptions.strSoundPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##SoundBtn"))
		{
			OpenFileDialog(m_tLoadOptions.strSoundPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		ImGui::Separator();
		ImGui::Spacing();

		// ---------------------------------------------------------
		// 하단 버튼 (확인 / 취소)
		// ---------------------------------------------------------
		float contentWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(contentWidth - 120.f); // 오른쪽 정렬

		// [확인] 버튼
		if (ImGui::Button("OK", ImVec2(50, 0)))
		{
			// 체크된 항목에 대해서만 로드 실행
			//if (m_tLoadOptions.bLoadAnimInfo && strlen(m_tLoadOptions.strAnimPath) > 0)
			//	Load_AnimationData(m_tLoadOptions.strAnimPath); // 구현하신 함수 호출

			if (m_tLoadOptions.bLoadHitbox && strlen(m_tLoadOptions.strHitboxPath) > 0)
				Load_HitboxData(m_tLoadOptions.strHitboxPath);

			if (m_tLoadOptions.bLoadEffect && strlen(m_tLoadOptions.strEffectPath) > 0)
				Load_EffectData(m_tLoadOptions.strEffectPath);

			if (m_tLoadOptions.bLoadSound && strlen(m_tLoadOptions.strSoundPath) > 0)
				Load_SoundData(m_tLoadOptions.strSoundPath);

			ImGui::CloseCurrentPopup(); // 모달 닫기
		}

		ImGui::SameLine();

		// [취소] 버튼
		if (ImGui::Button("Cancel", ImVec2(50, 0)))
		{
			ImGui::CloseCurrentPopup(); // 그냥 닫기
		}

		ImGui::EndPopup();
	}
}

void CPanel_AnimModelFile::RenderSaveModal()
{
	// 모달의 중심을 화면 가운데로 설정 (선택 사항)
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Save Data Manager", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		//ImGui::InputText("Tag", m_tLoadOptions.strAnimTag, ImGuiInputTextFlags_None);
		//ImGui::NewLine();
		ImGui::InputInt("Pooling count", &m_tLoadOptions.iPoolingCount);
		ImGui::Separator();

		// ---------------------------------------------------------
		// 1. 애니메이션 정보
		// ---------------------------------------------------------
		ImGui::Checkbox("##AnimCheck", &m_tLoadOptions.bLoadAnimInfo);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Animation Info", m_tLoadOptions.strAnimPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##AnimBtn"))
		{
			SaveFileDialog(m_tLoadOptions.strAnimPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		// ---------------------------------------------------------
		// 2. 충돌체(Hitbox) 정보
		// ---------------------------------------------------------
		ImGui::Checkbox("##HitboxCheck", &m_tLoadOptions.bLoadHitbox);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Hitbox Info", m_tLoadOptions.strHitboxPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##HitboxBtn"))
		{
			SaveFileDialog(m_tLoadOptions.strHitboxPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		// ---------------------------------------------------------
		// 3. 이펙트(Effect) 정보
		// ---------------------------------------------------------
		ImGui::Checkbox("##EffectCheck", &m_tLoadOptions.bLoadEffect);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Effect Info", m_tLoadOptions.strEffectPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##EffectBtn"))
		{
			SaveFileDialog(m_tLoadOptions.strEffectPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		// ---------------------------------------------------------
		// 4. 사운드(Sound) 정보
		// ---------------------------------------------------------
		ImGui::Checkbox("##SoundCheck", &m_tLoadOptions.bLoadSound);
		ImGui::SameLine();
		ImGui::SetNextItemWidth(300.f);
		ImGui::InputText("Sound Info", m_tLoadOptions.strSoundPath, 256, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();
		if (ImGui::Button("...##SoundBtn"))
		{
			SaveFileDialog(m_tLoadOptions.strSoundPath, "Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0");
		}

		ImGui::Separator();
		ImGui::Spacing();

		// ---------------------------------------------------------
		// 하단 버튼 (확인 / 취소)
		// ---------------------------------------------------------
		float contentWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(contentWidth - 120.f); // 오른쪽 정렬

		// [확인] 버튼
		if (ImGui::Button("OK", ImVec2(50, 0)))
		{
			// 체크된 항목에 대해서만 로드 실행
			//if (m_tLoadOptions.bLoadAnimInfo && strlen(m_tLoadOptions.strAnimPath) > 0)
			//	Load_AnimationData(m_tLoadOptions.strAnimPath); // 구현하신 함수 호출

			if (m_tLoadOptions.bLoadHitbox && strlen(m_tLoadOptions.strHitboxPath) > 0)
				Save_HitboxData(m_tLoadOptions.strHitboxPath);

			if (m_tLoadOptions.bLoadEffect && strlen(m_tLoadOptions.strEffectPath) > 0)
				Save_EffectData(m_tLoadOptions.strEffectPath);

			if (m_tLoadOptions.bLoadSound && strlen(m_tLoadOptions.strSoundPath) > 0)
				Save_SoundData(m_tLoadOptions.strSoundPath);

			ImGui::CloseCurrentPopup(); // 모달 닫기
		}

		ImGui::SameLine();

		// [취소] 버튼
		if (ImGui::Button("Cancel", ImVec2(50, 0)))
		{
			ImGui::CloseCurrentPopup(); // 그냥 닫기
		}

		ImGui::EndPopup();
	}
}

void CPanel_AnimModelFile::Load_HitboxData(fs::path path)
{
	m_pAnimToolManager->Load_AttackOverlap(path);
}

void CPanel_AnimModelFile::Save_HitboxData(fs::path path)
{
	m_pAnimToolManager->Save_AttackOverlap(path, m_tLoadOptions.strAnimTag, m_tLoadOptions.iPoolingCount);
}

void CPanel_AnimModelFile::Load_EffectData(fs::path path)
{
	m_pAnimToolManager->Load_EffectEvent(path);
}

void CPanel_AnimModelFile::Save_EffectData(fs::path path)
{
	m_pAnimToolManager->Save_EffectEvent(path, m_tLoadOptions.strAnimTag, m_tLoadOptions.iPoolingCount);
}

void CPanel_AnimModelFile::Load_SoundData(fs::path path)
{
	m_pAnimToolManager->Load_SoundEvent(path);
}

void CPanel_AnimModelFile::Save_SoundData(fs::path path)
{
	m_pAnimToolManager->Save_SoundEvent(path);
}

DIR CPanel_AnimModelFile::RefreshModelDir()
{
	m_tRootDirectory.directory = m_rootPath;

	for (fs::directory_entry iter : fs::directory_iterator(m_tRootDirectory.directory))
	{
		if (iter.is_directory())
			m_tRootDirectory.directories.push_back(SearchDir(iter));
	}

	return m_tRootDirectory;
}

DIR CPanel_AnimModelFile::SearchDir(fs::directory_entry directory)
{
	DIR result;

	result.directory = directory.path();

	for (auto& dir : fs::directory_iterator(directory))
	{
		if (dir.is_directory())
			result.directories.push_back(SearchDir(dir)); // recursive
		else
			result.files.push_back(dir.path());
	}

	return result;
}

void CPanel_AnimModelFile::SetDirectoryTree(DIR dir, fs::path parent)
{
	bool hasChildDir = !dir.directories.empty();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (!hasChildDir)
	{
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}

	ImGui::PushID(m_iResourceTreeID++);

	bool isOpen = ImGui::TreeNodeEx((void*)(intptr_t)m_iResourceTreeID, flags, "%s", dir.directory.filename().string().c_str());

	if (ImGui::IsItemClicked())
	{
		m_tCurrentDirectory = dir;
		m_files = dir.GetFiles();
	}

	if (isOpen && hasChildDir)
	{
		CheckAnimModel(dir, parent);

		for (auto& subDir : dir.directories)
		{
			SetDirectoryTree(subDir, dir.directory);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}
void CPanel_AnimModelFile::CheckAnimModel(DIR dir, fs::path parent)
{
	_bool hasAnimations = CheckResource(dir, "Animation", ".clip");
	_bool hasMesh = CheckResource(dir, "Model", ".mesh");
	_bool hasSkel = CheckResource(dir, "Model", ".skel");
	_bool hasMtrl = CheckResource(dir, "Material", ".json");

	if (hasAnimations && hasMesh && hasSkel && hasMtrl)
	{
		DrawPreTransformMatrix("AnimModel PreTransform Matrix", m_tAnimSrt);

		if (ImGui::SmallButton("Load anim model"))
			CGameInstance::GetInstance()->Broadcast<LoadAnimModel>(dir.directory, m_tAnimSrt);
		vector<fs::path> allProjectMeshes;

		std::function<void(const DIR&)> findAllMeshes = [&](const DIR& current) {
			for (const auto& file : current.files) {
				if (file.extension() == ".mesh")
					allProjectMeshes.push_back(file);
			}
			for (const auto& subDir : current.directories) {
				findAllMeshes(subDir);
			}
			};

		findAllMeshes(m_tRootDirectory);

		ImGui::Separator();
		ImGui::Spacing();
		if (!allProjectMeshes.empty())
		{
			static map<string, int> selectedIdxMap;
			string nodeKey = dir.directory.string();

			ImGui::SetNextItemWidth(200.f);
			string comboID = "##GlobalPartCombo_" + nodeKey;

			if (selectedIdxMap[nodeKey] >= allProjectMeshes.size()) selectedIdxMap[nodeKey] = 0;
			string previewName = allProjectMeshes[selectedIdxMap[nodeKey]].filename().string();

			if (ImGui::BeginCombo(comboID.c_str(), previewName.c_str()))
			{
				for (int i = 0; i < (int)allProjectMeshes.size(); ++i)
				{
					bool isSelected = (selectedIdxMap[nodeKey] == i);
					string displayName = allProjectMeshes[i].parent_path().filename().string() + "/" + allProjectMeshes[i].filename().string();

					if (ImGui::Selectable(displayName.c_str(), isSelected))
						selectedIdxMap[nodeKey] = i;
				}
				ImGui::EndCombo();
			}

			{
				ImGui::SetNextItemWidth(70.f); // 원하는 픽셀 길이
				ImGui::InputInt("Socket Bone Index", &m_iSocketBoneIdx);
				ImGui::SameLine();
				ImGui::Checkbox("Combine Matrix", &m_bCombine);
				ImGui::SameLine();
				ImGui::Checkbox("Static", &m_bWeaponStatic);
				ImGui::SameLine();
				ImGui::SetNextItemWidth(70.f); // 원하는 픽셀 길이
				ImGui::InputInt("RootBoneIdx", &m_iRootBoneIdx);
			}

			DrawPreTransformMatrix("PartModel PreTransform Matrix", m_tPartSrt);
			
			if (ImGui::SmallButton("Load Part Weapon"))
			{
				fs::path targetPath = allProjectMeshes[selectedIdxMap[nodeKey]];
				CGameInstance::GetInstance()->Broadcast<LoadAnimModelPart>(targetPath, m_tPartSrt, m_iSocketBoneIdx, m_bCombine, m_bWeaponStatic, m_iRootBoneIdx);
			}
		}
	}
}

_bool CPanel_AnimModelFile::CheckResource(DIR dir, string dirName, string extents)
{
	for (auto& subDir : dir.directories)
	{
		if (subDir.directory.stem() == dirName)
		{
			for (auto& file : subDir.files)
			{
				if (file.extension() == extents)
					return true;
			}
		}
	}

	return false;
}

CPanel_AnimModelFile* CPanel_AnimModelFile::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_AnimModelFile* pInstance = new CPanel_AnimModelFile(pLabel, pOwner, pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CPanel_AnimModelFile is faield to Create");
		return nullptr;
	}

	return pInstance;
}

void CPanel_AnimModelFile::Free()
{
	Super::Free();
}
