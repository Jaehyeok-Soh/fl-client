#include "Tool_Defines.h"
#include "Engine_Utils.h"
#include "Level_Effect.h"
#include "Cell.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "ImGui_Dockspace_MenuBar.h"

CImGui_Dockspace_MenuBar::CImGui_Dockspace_MenuBar(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pDevice, pDeviceContext)
{
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
		if (ImGui::BeginMenu("View##Dockspace"))
		{
			if (ImGui::MenuItem("Hirerchy##Menubar")) {}
			if (ImGui::MenuItem("Inspector##Menubar")) {}
			if (ImGui::MenuItem("blah##Menubar")) {}
			if (ImGui::MenuItem("blah##Menubar")) {}

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
		// ParsingData
		if (result.ends_with(L"_Parsed.json"))
		{

		}
		// MyData
		else
		{
			//Load_MyJsonData(result);
		}
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
		//Save_Data(szFile);
	}
}

CImGui_Dockspace_MenuBar* CImGui_Dockspace_MenuBar::Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return new CImGui_Dockspace_MenuBar(pLabel, pDevice, pDeviceContext);
}

void CImGui_Dockspace_MenuBar::Free()
{
	Super::Free();
}
