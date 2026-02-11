#include "pch.h"
#include "Panel_FileExplore.h"
#include "Folder.h"
#include "File.h"
#include "MapToolManager.h"
#include "MapObject.h"

CPanel_FileExplore::CPanel_FileExplore(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext), m_pImFileBrowser{}
	, m_vecFiles{}, m_pRootFolder{ nullptr }, m_szFileName{}, m_szKey{}, m_szFindFileName{}
	, m_pMapToolManager(CMapToolManager::GetInstance())
{
	Safe_AddRef(m_pMapToolManager);
	m_vecFiles.clear();
}


HRESULT CPanel_FileExplore::Initialize(const wchar_t* pRootFloaderPath, vector<string> vecShowExtName)
{
	m_pRootFolder = CFolder::Create(nullptr,pRootFloaderPath,m_pDevice,m_pDeviceContext);
	if (m_pRootFolder == nullptr) return E_FAIL;

	return S_OK;
}


HRESULT CPanel_FileExplore::Render(CToolObject* pGo)
{

	ImGui::Begin("File Explore");

	/* Main_Window */
	FloderWindow();

	FileWindow();

	FileFindWindow();

	ImGui::End();

	return S_OK;
}

void CPanel_FileExplore::Update(const _float fTimeDelta)
{
	return;
}

void CPanel_FileExplore::FloderWindow()
{
	ImGui::Begin("Floder Window");

	if (m_pRootFolder == nullptr)
	{
		ImGui::BulletText("Please Connect Floder...");
		ImGui::End();
	}
	else
	{
		for (const auto& Floders : m_pRootFolder->GetFloders())
			Draw_TreeFiles(Floders.second);

	}

	ImGui::End();
}

void CPanel_FileExplore::FileWindow()
{
	ImGui::Begin("File Window");

	if (m_vecFiles.empty())
	{
		ImGui::TextDisabled("File is Empty In Folder....");
	}
	else
	{
		// 1. 그리드 배치를 위한 컬럼 설정 (창 너비에 따라 자동 조절)
		float padding = 16.0f;
		float thumbnailSize = 64.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1) columnCount = 1;

		if (ImGui::BeginTable("FileGrid", columnCount))
		{
			int iCount = 0;
			for (auto& File : m_vecFiles)
			{
				ImGui::TableNextColumn();
				ImGui::PushID(iCount);

				FileInfo tInfo = File->Get_FileInfo();
				string strName = Engine_Utils::ToString(tInfo.wstrFileNameEXT);


				bool isSelected = false;
				ImGui::BeginGroup();

				ImGui::Button("FILE", ImVec2(thumbnailSize, thumbnailSize));

				string popupId = "FileOptionPopup##" + std::to_string(iCount);

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - thumbnailSize - ImGui::GetStyle().ItemSpacing.y);
				if (ImGui::Selectable("##Selectable", isSelected, ImGuiSelectableFlags_AllowOverlap, ImVec2(thumbnailSize, thumbnailSize + 20.0f)))
				{
					// 클릭 시 로직 (파일 선택 등)
				}

				if (ImGui::BeginPopupContextItem(popupId.c_str()))
				{
					if (ImGui::Selectable("Make Model"))
					{
						Render_MakeModelFilePath(tInfo.wstrFileFullPath);
					}
					ImGui::Separator();

					ImGui::EndPopup();
				}

				// 파일 이름 표시 (말줄임표 기능 포함)
				ImGui::TextWrapped(strName.c_str());
				ImGui::EndGroup();

				ImGui::PopID();
				iCount++;
			}
			ImGui::EndTable();
		}
	}

	ImGui::End();
}


HRESULT CPanel_FileExplore::Render_MakeModelFilePath(const wstring& wstrPath)
{
	if (wstrPath.empty()) return E_FAIL;

	path FullPath = wstrPath;
	wstring wstrExt = FullPath.extension();

	path basePath = std::filesystem::current_path();

	if ( wstrExt  != g_wszMeshExtension) return S_OK;

	CMapObject::MAPOBJECT_DESC tMapObjectDesc{};
	tMapObjectDesc.eClientLevelType = m_pMapToolManager->Get_MakeMapObejctClientLevelType();
	tMapObjectDesc.eClientMakePath = m_pMapToolManager->Get_MakeMapObjectClientMakePath();
	tMapObjectDesc.eMapObjectDrawType = m_pMapToolManager->Get_MakeMapObjectDrawType();
	tMapObjectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
	tMapObjectDesc.tUsingModelInfo.wstrPath = std::filesystem::relative(FullPath,basePath);
	tMapObjectDesc.tUsingModelInfo.wstrName = path(wstrPath).filename().stem();
	tMapObjectDesc.eState = CMapObject::EState::Preview;
	tMapObjectDesc.isLoaded = false;
	tMapObjectDesc.isUELoaded = false;
	tMapObjectDesc.wstrLayerTag = g_wszMapObjectLayer;
	SRT_DATA tData{};
	tMapObjectDesc.vecSRTs.push_back(tData);

	m_pMapToolManager->Set_BrushScale(tData.vScale);
	m_pMapToolManager->Set_BrushRotation(tData.vQuat);

	m_pMapToolManager->Make_MapObject(&tMapObjectDesc, true);




	return S_OK;
}


void CPanel_FileExplore::Draw_TreeFiles(CFolder* pTreeFloder)
{
	if (pTreeFloder == nullptr) return;
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	string strFloderName = Engine_Utils::ToString(pTreeFloder->GetFloderInfo().wstrFloderName);

	bool isOpend = ImGui::TreeNodeEx(strFloderName.c_str(), flags);

	if (ImGui::IsItemClicked())
	{
		m_vecFiles = pTreeFloder->GetFiles();
		m_strCurPath = Engine_Utils::ToString(pTreeFloder->GetFloderInfo().wstrFloderFullPath);
	}

	/* 본인 맵 Floder를 또 순회한다 */
	if (isOpend)
	{
		for (const auto& Floders : pTreeFloder->GetFloders())
		{
			Draw_TreeFiles(Floders.second);
		}
		ImGui::TreePop();
	}
}

void CPanel_FileExplore::FileFindWindow()
{
	ImGui::Begin("Find File Window");

	ImGui::InputText("Find File Name" , m_szFindFileName, MAX_PATH);
	if (ImGui::Button("Find"))
	{
		m_vecFindFilePathList.clear();
		m_vecFindFilePathList = m_pRootFolder->Find_File(Engine_Utils::ToWString(m_szFindFileName));
		memset(m_szFindFileName,0,MAX_PATH);
	}
	
	if (m_vecFindFilePathList.empty())
		ImGui::Text(" Search File Is None.. ");
	else
	{
		_int iIndex{};


		for (auto& FilePath : m_vecFindFilePathList)
		{
			ImGui::PushID(iIndex);

			path pathFile = path(FilePath);
			string strFilePath = path(pathFile).string();
			string strFileName = pathFile.filename().string();

			ImGui::TextWrapped(strFileName.c_str());
			ImVec2 p = ImGui::GetCursorScreenPos();
			if (ImGui::IsItemHovered())
			{
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
					ImGui::GetColorU32(ImGuiCol_HeaderHovered)
				);
			}

			string popupId = "FileOptionPopup##" + std::to_string(iIndex);

			if (ImGui::BeginPopupContextItem(popupId.c_str()))
			{
				if (ImGui::Selectable("Make Model"))
				{
					Render_MakeModelFilePath(pathFile);
				}
				ImGui::Separator();

				ImGui::EndPopup();
			}
			iIndex++;
			ImGui::PopID();
		}
	}


	ImGui::End();

}


CPanel_FileExplore* CPanel_FileExplore::Create(const wchar_t* pRootFloaderPath 
	, vector<string> vecShowExtName  ,const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CPanel_FileExplore* pPanel = new CPanel_FileExplore(pLabel,pOwner,pDevice,pDeviceContext);
	if (FAILED(pPanel->Initialize(pRootFloaderPath, vecShowExtName)))
	{
		Safe_Release(pPanel);
		MSG_BOX("File Explore is faield to Create");
		return nullptr;
	}

	return pPanel;
}


void CPanel_FileExplore::Free()
{
	Super::Free();

	
	Safe_Delete(m_pImFileBrowser);

	for (auto& File : m_vecFiles)
		Safe_Release(File);

	Safe_Release(m_pRootFolder);
	Safe_Release(m_pMapToolManager);


	return;
}

