#include "pch.h"
#include "Panel_MapDataController.h"

/* 진짜 맵파일 로드 */
#include "MapFile_Manager.h"

/* 로우 데이터 뽑아서 새로 만들어주는역할 + 새로만든 Json파일로 모델 뛰우기 */
#include "UEMapdataParser.h"
#include "UEMapdataLoader.h"

USING(Tool)

CPanel_MapDataController::CPanel_MapDataController(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CImGui_Panel(pLabel, pOwner, pDevice, pDeviceContext)
{
}

HRESULT CPanel_MapDataController::Render(CToolObject* pGo)
{
	ImGui::Begin(m_strLabel.c_str());


	if (ImGui::Button(" Load Map Data ", ImVec2(128, 64)))
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
			if (result.find(L"_Parsed") != std::wstring::npos)
			{
				CUEMapDataLoader* pMapRowDataLoader =CUEMapDataLoader::Create(m_pDevice,m_pDeviceContext);
				pMapRowDataLoader->Make_StaticModel(result,g_wszStaticModelLayer,g_wszColMeshLayer);
				Safe_Release(pMapRowDataLoader);
			}
			else
			{
				/* Parsed.json이 아니라면 변환기 */
				CUEMapdataParser::MAPPARSER_DESC tDesc{};
				tDesc.wstrPath = ofn.lpstrFile;
				CUEMapdataParser* pMapDataLoader = CUEMapdataParser::Create(tDesc);
				if(FAILED(pMapDataLoader->Read_Mapdata(true)))
					return E_FAIL;
				if (FAILED(pMapDataLoader->Write_Mapdata()))
					return E_FAIL;
				Safe_Release(pMapDataLoader);
			}
		}
	}

	ImGui::End();

	return S_OK;
}

void CPanel_MapDataController::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

CPanel_MapDataController* CPanel_MapDataController::Create(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	return  new CPanel_MapDataController(pLabel, pOwner, pDevice, pDeviceContext);
}

void CPanel_MapDataController::Free()
{
	Super::Free();
}