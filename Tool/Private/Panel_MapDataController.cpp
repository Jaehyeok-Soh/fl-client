#include "pch.h"
#include "Panel_MapDataController.h"

/* 진짜 맵파일 로드 */
#include "MapFile_Manager.h"

/* 로우 데이터 뽑아서 새로 만들어주는역할 + 새로만든 Json파일로 모델 뛰우기 */
#include "UEMapdataParser.h"
#include "UEMapdataLoader.h"
#include "Engine_Utils.h"


USING(Tool)

CPanel_MapDataController::CPanel_MapDataController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
	, m_pUEMapdataParser(CUEMapdataParser::GetInstance())
{
}

HRESULT CPanel_MapDataController::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());

	m_vecConvertedUEMapDataPath = m_pUEMapdataParser->Get_ConvertedFilePathList();

	if (FAILED(Render_Converted_UnrealRawMapData_Button()))
		return E_FAIL;


	ImGui::NewLine();

	if (m_vecConvertedUEMapDataPath.empty())
		ImGui::Text("Converted Data is Emtpy...");

	else
		if (FAILED(Render_ConvertedList()))
			return E_FAIL;


	ImGui::End();

	return S_OK;
}

void CPanel_MapDataController::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

HRESULT CPanel_MapDataController::Render_ConvertedList()
{
	string strSelectPath = Engine_Utils::ToString(m_vecConvertedUEMapDataPath[m_iSelectConvertedUEMapDataPath]);

	path pathSelect{strSelectPath};
	string strFileName = path(strSelectPath).filename().stem().string();

	ImGui::SeparatorText(" Converted Raw Map List ");


	if (ImGui::BeginCombo("Converted Raw Map Data List" , strFileName.c_str()))
	{
		_uint i = 0;
		for (auto& DataPath : m_vecConvertedUEMapDataPath)
		{
			bool isSelected = m_iSelectConvertedUEMapDataPath == i;
			string strCurFileName = path(DataPath).filename().stem().string();
			if (ImGui::Selectable(strCurFileName.c_str(), isSelected))
				m_iSelectConvertedUEMapDataPath = i;
			if (isSelected == true)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}


	ImGui::SeparatorText("Batch Converted Map Data");

	if (ImGui::Button("Batch Converted Map Data"))
	{
		m_pUEMapdataParser->Batch_UnrealRawMapData(Engine_Utils::ToWString(strSelectPath).c_str());
	}

	ImGui::Separator();



	ImGui::SeparatorText("Save Filtering Map Data");

	if (ImGui::Button("Save Filtering Map Data"))
	{
		m_pUEMapdataParser->Save_FilteringRawMapData(Engine_Utils::ToWString(strSelectPath).c_str());
	}

	ImGui::Separator();


	ImGui::SeparatorText("Save Converted Map Data");

	if (ImGui::Button("Save Converted Map Data"))
	{
		m_pUEMapdataParser->Save_ConvertedRawMapData(Engine_Utils::ToWString(strSelectPath).c_str());
	}

	ImGui::Separator();


	return S_OK;
}

HRESULT CPanel_MapDataController::Render_Converted_UnrealRawMapData_Button()
{
	ImGui::SeparatorText("Load Unreal Raw Map Data");

	if (ImGui::Button(" Load Unreal Raw Map Data "))
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
			m_pUEMapdataParser->Convert_UnrealRawMapData(result.c_str());
		}
	}

	ImGui::Separator();

	return S_OK;
}

HRESULT CPanel_MapDataController::Render_Filtering_UnrealRawMapData_Button()
{
	return S_OK;
}

CPanel_MapDataController* CPanel_MapDataController::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return  new CPanel_MapDataController(pLabel, pOwner, pDevice, pDeviceContext);
}

void CPanel_MapDataController::Free()
{
	Super::Free();
}