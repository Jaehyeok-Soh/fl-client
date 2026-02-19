#include "pch.h"
#include "Panel_AnimModelFile.h"
#include "Folder.h"
#include "File.h"
#include "MapToolManager.h"
#include "MapObject.h"
#include "Animation_Defines.h"

CPanel_AnimModelFile::CPanel_AnimModelFile(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
{
}

HRESULT CPanel_AnimModelFile::Initialize()
{
	RefreshModelDir();

	return S_OK;
}

HRESULT CPanel_AnimModelFile::Render(CToolObject* pGo)
{
	ImGui::Begin("File Explore");

	/* Main_Window */

	DirectoryWindow();

	FileWindow();

	ImGui::End();

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
	clipper.Begin(static_cast<_uint>(m_files.size()));
	while (clipper.Step())
	{
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			ImGui::Text("%d: %s", i, m_files[i].filename().string().c_str());
	}

	ImGui::End();
}

CPanel_AnimModelFile::DIR CPanel_AnimModelFile::RefreshModelDir()
{
	m_tRootDirectory.directory = m_rootPath;

	for (fs::directory_entry iter : fs::directory_iterator(m_tRootDirectory.directory))
	{
		if (iter.is_directory())
			m_tRootDirectory.directories.push_back(SearchDir(iter));
	}

	return m_tRootDirectory;
}

CPanel_AnimModelFile::DIR CPanel_AnimModelFile::SearchDir(fs::directory_entry directory)
{
	CPanel_AnimModelFile::DIR result;

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
	_bool hasAnimations = { false };
	_bool hasMesh = { false };
	_bool hasSkel = { false };
	_bool hasMtrl = { false };

	hasAnimations = CheckResource(dir, "Animation", ".clip");
	hasMesh = CheckResource(dir, "Model", ".mesh");
	hasSkel = CheckResource(dir, "Model", ".skel");
	hasMtrl = dir.directory.stem() == "Material";
	hasMtrl = CheckResource(dir, "Material", ".json") && CheckResource(dir, "Material", ".png");

	if (hasAnimations && hasMesh && hasSkel && hasMtrl)
	{
		if (ImGui::SmallButton("Load anim model"))
			CGameInstance::GetInstance()->Broadcast<LoadAnimModel>(dir.directory);
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
