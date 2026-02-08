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

	ImVec2 ButtonSize = ImVec2(64, 64);


	if (m_vecFiles.empty())
	{
		ImGui::Text("File is Empty In Floder....");
	}

	else
	{
		int iCount = 0;
		int iColumnMax = 4;

		for (auto& File : m_vecFiles)
		{
			FileInfo tInfo = File->Get_FileInfo();
			string strName = Engine_Utils::ToString(tInfo.wstrFileNameEXT);


			ImGui::TextWrapped(strName.c_str());

			ImVec2 p = ImGui::GetCursorScreenPos(); // 현재 커서 위치 저장

			if (ImGui::IsItemHovered())
			{
				ImGui::GetWindowDrawList()->AddRectFilled(
					ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
					ImGui::GetColorU32(ImGuiCol_HeaderHovered)
				);
			}

			string popupId = "FileOptionPopup##" + strName + std::to_string(iCount);

			if (ImGui::BeginPopupContextItem(popupId.c_str()))
			{
				Render_FileMoustRightButton(File->Get_FileInfo().wstrFileFullPath);
				ImGui::EndPopup();
			}

			iCount++;
		}
	}

	ImGui::End();
}


HRESULT CPanel_FileExplore::Render_FileMoustRightButton(const wstring& wstrPath)
{
	if (wstrPath.empty()) return E_FAIL;

	path FullPath = wstrPath;
	wstring wstrExt = FullPath.extension();



	if ( wstrExt  != g_wszMeshExtension) return S_OK;


	if (ImGui::Selectable("Make Model"))
	{
		//CStaticModel::STATICMODEL_DESC tDesc{};
		//tDesc.tData.tUsingModelInfo.wstrName = path(wstrPath).filename().stem();
		//tDesc.tData.tUsingModelInfo.wstrPath = wstrPath;
		//tDesc.isLoaded = false;
		//tDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::MAP);
		//tDesc.eState = CMapObject::EState::Preview;
		//tDesc.wstrLayerTag = g_wszStaticModelLayer;
		//m_pMapToolManager->Make_MapObject(EMapObject_Type::STATICMODEL ,&tDesc);
	}

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
		_uint iIndex{};
		//char  szBeginPopupContextItem[MAX_PATH];

		for (auto& FilePath : m_vecFindFilePathList)
		{
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

			string strID = "PopupContextItem##" + std::to_string(iIndex);

			if (ImGui::BeginPopupContextItem(strID.c_str()))
			{
				Render_FileMoustRightButton(FilePath);
				ImGui::EndPopup();
			}
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

