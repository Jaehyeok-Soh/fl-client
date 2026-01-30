#include "pch.h"
#include "ImGui_Dockspace_MenuBar.h"
#include "Engine_Utils.h"
#include "Level_Effect.h"
#include "GameObject.h"
#include "Level_Loading.h"
#include "DataDocument_Example.h"
#include "DataStruct_Example.h"
#include "GameInstance.h"

CImGui_Dockspace_MenuBar::CImGui_Dockspace_MenuBar(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pDevice, pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Dockspace_MenuBar::Render(CToolObject* pGo)
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File##Dockspace"))
		{
			if (ImGui::MenuItem("New##Menubar")) { }
			if (ImGui::MenuItem("Open##Menubar")) { Open_FileDialog(); }
			if (ImGui::MenuItem("Save##Menubar")) { Save_FileDialog(); }
			if (ImGui::MenuItem("Clear##Menubar")) {}

			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

	return S_OK;
}

void CImGui_Dockspace_MenuBar::Open_FileDialog()
{
	OPENFILENAMEW ofn{};
	_tchar szFile[MAX_PATH] = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (::GetOpenFileNameW(&ofn) == TRUE)
	{
		wstring result = szFile;
		Load_Data(result);
	}
}

void CImGui_Dockspace_MenuBar::Save_FileDialog()
{
	OPENFILENAMEW ofn{};
	_tchar szFile[MAX_PATH] = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Json Files (*.json)\0*.json\0All Files (*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.lpstrDefExt = L"json";

	if (::GetSaveFileNameW(&ofn) == TRUE)
	{
		Save_Data(szFile);
	}
}

void CImGui_Dockspace_MenuBar::Save_Data(const wstring& wstrFilePath)
{
	ELevelType eCurentLevel = static_cast<ELevelType>(m_pGameInstance->Get_CurrentLevelIndex());
	switch (eCurentLevel)
	{
	case Tool::ELevelType::LOADING:

		break;
	case Tool::ELevelType::MAP:
		Save_MapData(wstrFilePath);
		break;
	case Tool::ELevelType::ANIMATION:
		Save_AnimationData(wstrFilePath);
		break;
	case Tool::ELevelType::EFFECT:
		Save_EffectData(wstrFilePath);
		break;
	case Tool::ELevelType::CAMERA:
		Save_CameraData(wstrFilePath);
		break;
	case Tool::ELevelType::UI:
		Save_UIData(wstrFilePath);
		break;
	case Tool::ELevelType::ASSET_CONVERT:
		break;
	}
}

void CImGui_Dockspace_MenuBar::Load_Data(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::MAP;
	DTO::ECategory eCategory = DTO::ECategory::MAP;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Example>(iLevelID, eCategory)))
		return;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, wstrFilePath)))
		return;

	// 여기까지 성공하면 로드가 된것! 아래 코드는 그냥 테스트용

	const CDataDocumentBase* pBase = m_pGameInstance->Get_Document(iLevelID, eCategory, "asdf");
	CDataDocumentBase* pTest = const_cast<CDataDocumentBase*>(pBase);
	CDataDocument_Example* phi = static_cast<CDataDocument_Example*>(pTest);
	auto& okay = phi->Get_ListByType(ENUM_TO_UINT(DTO::EMapType::STATICMODEL));
	auto& okay2 = phi->Get_ListByType(ENUM_TO_UINT(DTO::EMapType::LIGHT));
	_int a = 10;
	if (okay.size() > 0 && okay2.size() > 0)
		MSG_BOX("Okay");
}

void CImGui_Dockspace_MenuBar::Save_MapData(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::MAP;
	DTO::ECategory eCategory = DTO::ECategory::MAP;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);
 	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Example>(iLevelID, eCategory)))
		return;

	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, wstrFilePath);
	if (pDocument == nullptr)
		return;

	CDataDocument_Example* pDoc = static_cast<CDataDocument_Example*>(pDocument);
	{
		DTO::TExample_LightData lightData{};
		lightData.strTag = "Light1";
		lightData.iValue = 2;
		lightData.iValue2 = 1.f;
		lightData.iValue3 = -2;
		if (FAILED(pDoc->Try_Add(lightData)))
			return;
	}
	{
		DTO::TExample_StaticModelData staticModelData{};
		staticModelData.strTag = "model1";
		staticModelData.iValue = 3;
		staticModelData.iValue2 = 2.f;
		staticModelData.iValue3 = -3;
		if (FAILED(pDoc->Try_Add(staticModelData)))
			return;
	}
	m_pGameInstance->Save_File_Json(iLevelID, eCategory, wstrFilePath);
	
	//ELevelType eLevelType = ELevelType::MAP;
	//_uint iLevelID = ENUM_TO_UINT(eLevelType);
	//CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, DTO::ECategory::MAP, wstrFilePath);
	//if (pDocument == nullptr)
	//	return;

	//Request_ExportData(eLevelType, DTO::ECategory::MAP, g_wszStaticModelLayer, pDocument);

	//m_pGameInstance->Save_File_Json(iLevelID, DTO::ECategory::MAP, wstrFilePath);
}

void CImGui_Dockspace_MenuBar::Save_AnimationData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Save_EffectData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Save_CameraData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Save_UIData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Request_ExportData(ELevelType eLevelID, DTO::ECategory eCategory, const wstring& wstrLayerTag, CDataDocumentBase* pDocument)
{
	if (list<CGameObject*>* pGameObjectList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(eLevelID), wstrLayerTag))
	{
		if (pGameObjectList->size() > 0)
		{
			for (auto itr = pGameObjectList->begin();
				itr != pGameObjectList->end();
				++itr)
			{
				(*itr)->Export_Data(eCategory, pDocument);
			}
		}
	}
}

CImGui_Dockspace_MenuBar* CImGui_Dockspace_MenuBar::Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return new CImGui_Dockspace_MenuBar(pLabel, pDevice, pDeviceContext);
}

void CImGui_Dockspace_MenuBar::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}
