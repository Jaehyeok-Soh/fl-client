#include "pch.h"
#include "UIData_Repository.h"
#include "Tool_Defines.h"
#include "ImGui_UIManager.h"
#include "FileUtils.h"
#include "DataStruct_UI.h"
#include "DataDocument_UI.h"
#include "ToolCanvas.h"
#include "ToolLayer.h"
#include "ToolUI.h"
#include "GameInstance.h"
#define UIDATAFILE_PATH L"../../Resources/Data/UIData/"

NS_BEGIN(Tool)
IMPLEMENT_SINGLETON(CUIData_Repository)

CUIData_Repository::CUIData_Repository()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CUIData_Repository::Load_UIData()
{
	//CFileUtils* pFileUtil = CFileUtils::Create();

	//if (FAILED(pFileUtil->Open(UIDATAFILE_PATH, FileMode::READ)))
	//{
	//	MSG_BOX("CUIData_Repository::Load_UIData, open failed");
	//	return E_FAIL;
	//}

	//std::string text = {};
	//pFileUtil->ReadAllText(text);

	//order_json j = json::parse(text);
	//m_vecUIDTO = j.get<vector<CANVAS_DATA>>();
	//OutRef = j.get<vector<CANVAS_DATA>>();
	//Safe_Release(pFileUtil);


	ELevelType eLevelType = ELevelType::UI;
	DTO::ECategory eCategory = DTO::ECategory::UI;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, UIDATAFILE_PATH)))
		return E_FAIL;

	const CDataDocumentBase* pBase = m_pGameInstance->Get_Document(iLevelID, eCategory, "Data");
	CDataDocumentBase* pTest = const_cast<CDataDocumentBase*>(pBase);
	CDataDocument_UI* phi = static_cast<CDataDocument_UI*>(pTest);

	auto& okay = phi->Get_ListByType(ENUM_TO_UINT(DTO::EUIType::CANVAS));
	auto& okay2 = phi->Get_ListByType(ENUM_TO_UINT(DTO::EUIType::LAYER));
	auto& okay3 = phi->Get_ListByType(ENUM_TO_UINT(DTO::EUIType::GENERICUI));

	MSG_BOX("불러오기 완료");
	return S_OK;
}

HRESULT CUIData_Repository::Save_UIData()
{
	ELevelType eLevelType = ELevelType::UI;
	DTO::ECategory eCategory = DTO::ECategory::UI;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	/* 새 DataStore랑 CDataDocument_UI전용 LoadFile_Json이랑 CDataDocument_UI::Create를 iLevelID - eCategory 조합에 매핑한다. */
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
	{
		MSG_BOX("CUIData_Repository::Save_UIData, Document Regist Failed");
		return E_FAIL;
	}

	/* 런타임 데이터를 저장용으로 변경 */
	{
		const auto* pCanvasVec = CImGui_UIManager::GetInstance()->Safe_Access_CanvasVector();
		if (nullptr != pCanvasVec)
		{
			/* Canvas 저장 */
			for (auto* pCanvas : *pCanvasVec)
			{
				/* Canvas 별로 Json을 파일을 나누겠다 */
				_wstring wstrFilePath = UIDATAFILE_PATH + Engine_Utils::ToWString(pCanvas->Get_Tag()) + L".json";
				/* 아까 매핑한 iLevelID - eCategory 조합에서 DataStore를 찾고 FilePath의 파일이름을 Stem으로 뽑아서 Key로 DataDocument_UI를 Create해서 등록 */
				CDataDocumentBase* pDocBase = m_pGameInstance->Ensure_Document(iLevelID, eCategory, wstrFilePath);
				if (pDocBase == nullptr)
					return E_FAIL;

				CDataDocument_UI* pDoc = static_cast<CDataDocument_UI*>(pDocBase);
				if (FAILED(pDoc->Try_Add(pCanvas->Get_Data())))
					return E_FAIL;


				/* Canvas에 저장된 Layer 저장 */
				auto* pLayerVec = pCanvas->Safe_Access_LayerObject_Vector_Ptr();
				if (nullptr == pLayerVec)
					goto SAVE_FILE;
				for (auto* pLayer : *pLayerVec)
				{
					if (FAILED(pDoc->Try_Add(pLayer->Get_Data())))
						return E_FAIL;


					/* Layer에 저장된 UI 저장 */
					auto* pUIVec = pLayer->Safe_Access_UIObject_Vector_Ptr();
					if (nullptr == pUIVec)
						goto SAVE_FILE;
					for (auto* pUI : *pUIVec)
					{
						if (FAILED(pDoc->Try_Add(pUI->Get_Data())))
							return E_FAIL;
					}
				}

			SAVE_FILE:
				if (FAILED(m_pGameInstance->Save_File_Json(iLevelID, eCategory, wstrFilePath)))
					return E_FAIL;
			}
		}
	}

	MSG_BOX("저장 완료");
	return S_OK;
}

void CUIData_Repository::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

NS_END