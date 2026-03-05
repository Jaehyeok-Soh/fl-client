#include "pch.h"
#include "ImGui_Dockspace_MenuBar.h"
#include "Engine_Utils.h"
#include "Level_Effect.h"
#include "GameObject.h"
#include "Level_Loading.h"
#include "DataDocument_Example.h"
#include "DataStruct_Example.h"
#include "GameInstance.h"

// Effect
#include "DataDocument_Effect.h"
#include "DataStruct_Effect.h"
#include "Effect.h"
#include "CEFfectObject.h"

// UI
#include "DataStruct_UI.h"
#include "DataDocument_UI.h"
#include "ImGui_UIManager.h"
#include "ToolCanvas.h"
#include "Builder_UI.h"

// Map
#include "DataStruct_Map.h"
#include "DataDocument_Map.h"
#include "MapToolManager.h"
#include "Builder_Map.h"
#include "MapObject.h"

// BuilderSystem
#include "BuilderSystem.h"



CImGui_Dockspace_MenuBar::CImGui_Dockspace_MenuBar(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pLabel, pDevice, pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT	CImGui_Dockspace_MenuBar::Initialize()
{
	if (FAILED(Ready_Builder()))
		return E_FAIL;

	return S_OK;
}

HRESULT	CImGui_Dockspace_MenuBar::Ready_Builder()
{
	/* ReadyBuilder */

	m_pBuilderSystem = CBuilderSystem::Create();
	if (m_pBuilderSystem == nullptr)  return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::MAP, CBuilder_Map::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::MAP)))))
		return E_FAIL;
	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::UI)))))
		return E_FAIL;

	return S_OK;
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

void CImGui_Dockspace_MenuBar::Clear_FileDialog()
{
	ELevelType eCurentLevel = static_cast<ELevelType>(m_pGameInstance->Get_CurrentLevelIndex());
	switch (eCurentLevel)
	{
	case Tool::ELevelType::LOADING:

		break;
	case Tool::ELevelType::MAP: break;
	case Tool::ELevelType::ANIMATION: break;
	case Tool::ELevelType::EFFECT:
		Clear_EffectData();
		break;
	case Tool::ELevelType::CAMERA: break;

	case Tool::ELevelType::UI: break;
	case Tool::ELevelType::ASSET_CONVERT: 		break;
	}
}

void CImGui_Dockspace_MenuBar::Clear_EffectData()
{
	m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::EFFECT), L"Effect_Layer");
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
	ELevelType eCurentLevel = static_cast<ELevelType>(m_pGameInstance->Get_CurrentLevelIndex());
	switch (eCurentLevel)
	{
	case Tool::ELevelType::LOADING:
		break;
	case Tool::ELevelType::MAP:
		Load_MapData(wstrFilePath);
		break;
	case Tool::ELevelType::ANIMATION:
		Load_AnimationData(wstrFilePath);
		break;
	case Tool::ELevelType::EFFECT:
		Load_EffectData(wstrFilePath);
		break;
	case Tool::ELevelType::CAMERA:
		Load_CameraData(wstrFilePath);
		break;
	case Tool::ELevelType::UI:
		Load_UIData(wstrFilePath);
		break;
	case Tool::ELevelType::ASSET_CONVERT:
		break;
	}
}

void CImGui_Dockspace_MenuBar::Save_MapData(const wstring& wstrFilePath)
{
	/* 내가 저장시킬 최종 카테고리 ex Map / Effect / UI 중에 map 을 선택 */
	DTO::ECategory eCategory	= DTO::ECategory::MAP;

	ELevelType eLevelType		= ELevelType::MAP;
	_uint iLevelID				= ENUM_TO_UINT(eLevelType);

 	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return;

	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, wstrFilePath);
	if (pDocument == nullptr)
		return;

	/* 여러개의 layer를 한꺼번에 저장하고 싶다면 Layer Requset ExportData를 집어넣는다 */
	Request_ExportData(eLevelType, DTO::ECategory::MAP, g_wszMapObjectLayer , pDocument);

	m_pGameInstance->Save_File_Json(iLevelID, DTO::ECategory::MAP, wstrFilePath);
}

void CImGui_Dockspace_MenuBar::Save_AnimationData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Save_EffectData(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::EFFECT;
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(iLevelID, eCategory)))
		return;

	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, wstrFilePath);
	if (pDocument == nullptr)
		return;

	// Effect Container 객체 Layer
	wstring ContainerObjectLayerTag = L"Effect_Layer";

	Request_ExportData(ELevelType::EFFECT, eCategory, ContainerObjectLayerTag, pDocument);

	m_pGameInstance->Save_File_Json(iLevelID, eCategory, wstrFilePath);

}

void CImGui_Dockspace_MenuBar::Save_CameraData(const wstring& wstrFilePath)
{

}

void CImGui_Dockspace_MenuBar::Save_UIData(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::UI;
	DTO::ECategory eCategory = DTO::ECategory::UI;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	const _wstring& wstrKey = Engine_Utils::ToWString(
		CImGui_UIManager::GetInstance()->Safe_Access_Canvas(CImGui_UIManager::GetInstance()->Get_CurCanvasIndex())->Get_Tag());

	/* Folder 기준을 Static, Logo 같은 Client Level이나 Prefab으로 저장할거라 선택한 파일의 부모경로 ~~/Static 까지 받아서 Canvas 이름으로 Json 파일 경로를 생성 */
	_wstring wstrfinalPath = std::filesystem::path(wstrFilePath).parent_path().wstring() + L"\\" + wstrKey + L".json";
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return;

	/* Canvas Name.json 파일 */
	CDataDocumentBase* pDocument = m_pGameInstance->Ensure_Document(iLevelID, eCategory, wstrfinalPath);
	if (pDocument == nullptr)
		return;

	Request_ExportData(eLevelType, eCategory, wstrKey + L"_Layer", pDocument);
	m_pGameInstance->Save_File_Json(iLevelID, eCategory, wstrfinalPath);
}

void CImGui_Dockspace_MenuBar::Load_MapData(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::MAP;
	DTO::ECategory eCategory = DTO::ECategory::MAP;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, wstrFilePath)))
		return;

	m_pBuilderSystem->Build_File(ENUM_TO_UINT(ELevelType::MAP),DTO::ECategory::MAP,path(wstrFilePath).filename().stem().string());

	//// 여기까지 성공하면 로드가 된것! 아래 코드는 그냥 테스트용
	//const CDataDocumentBase* pBase = m_pGameInstance->Get_Document(iLevelID, eCategory,path(wstrFilePath).filename().stem().string());
	//const CDataDocument_Map* pTest = static_cast<const CDataDocument_Map*>(pBase);
	//const auto okay = pTest->Get_ListByType(ENUM_TO_UINT(DTO::EMapType::STATICMODEL));
	//if (okay.empty())
	//	MSG_BOX("Empty");
	//else
	//	MSG_BOX("Succsed");
}

void CImGui_Dockspace_MenuBar::Load_AnimationData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Load_EffectData(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::EFFECT;
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path filePath(wstrFilePath);
	string FileKey = filePath.stem().string();

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(iLevelID, eCategory)))
		return;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, wstrFilePath)))
		return;

	// 여기까지 성공하면 로드가 된것! 아래 코드는 그냥 테스트용
	const CDataDocumentBase* pBase = m_pGameInstance->Get_Document(iLevelID, eCategory, FileKey);
	const CDataDocument_Effect* pTest = static_cast<const CDataDocument_Effect*>(pBase);

	const auto okay = pTest->Get_ListByType(ENUM_TO_UINT(DTO::EEffectType::EFFECT_CONTAINER));

	if (okay.size() > 0) {

		MSG_BOX("Okay");
	}
	else
	{
		MSG_BOX("Failed");
	}

	// 객체 로딩중 .. 
	CToolObject* pEffectContainer = { nullptr };
	for (auto& ObjectData : okay)
	{
		DTO::TEFFECT_ContainerData pData = static_cast<CEFFECT_CONTAINER*>(ObjectData)->Get_Data();

		// 부모 객체 생성 과정
			// worldmatrix 분해해서 transform에 굳이굳이 넣기.
		Vec3 vScale, vPos, vRot;
		Quat vQuat;
		pData.vWorldMatrix.Decompose(vScale, vQuat, vPos);

		CTransform::TRANSFORM_DESC pTransDesc = {};
		pTransDesc.fMovePerSec = 1.f;
		pTransDesc.fRotatePerSec = 1.f;
		pTransDesc.ScaleMatrix = Matrix::CreateScale(vScale);
		pTransDesc.RotationMatrix = Matrix::CreateFromQuaternion(vQuat);
		pTransDesc.TranslationMatrix = Matrix::CreateTranslation(vPos);

		Effect::EFFECT_CONTAINERDESC pDesc = {};
		pDesc._Effect_SimulationType = (DTO::E_SIMULATION_SPACE)pData._Effect_SimulationType;
		pDesc.wstrLayerTag = L"Effect_Layer";
		pDesc.iLevelIndex = iLevelID;
		pDesc.pTransform_Desc = &pTransDesc;

		// =============== CREATE	EFFECT ==================

		if (!(pEffectContainer = static_cast<CToolObject*>(m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::EFFECT),
			L"Prototype_GameObject_Effect",
			ENUM_TO_UINT(ELevelType::EFFECT),
			pDesc.wstrLayerTag, &pDesc))))
		{
			MSG_BOX("생성 실패 : Effect Object - Particle System");
			return;
		}
		else
		{
			// 이름 정해주기
			pEffectContainer->Set_Name(pData.EffectContainerName);
		}

		// =============== CREATE   PARTS ===================
	_uint index = 0;
	for (DTO::TEFFECT_PartsData& Part : pData._ChildData)
	{
		// Transform 데이터 복원
		Vec3 vScale, vPos;
		Quat vQuat;
		Part.vWorldMatrix.Decompose(vScale, vQuat, vPos);

		CEffectObject::Effect_Desc pEffectDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		transformDesc.ScaleMatrix = Matrix::CreateScale(vScale);
		transformDesc.RotationMatrix = Matrix::CreateFromQuaternion(vQuat);
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(vPos);
		transformDesc.fRotatePerSec = 1.f;
		transformDesc.fMovePerSec = 1.f;

		pEffectDesc.pMatParent = &(pEffectContainer->Get_Component<CTransform>()->Get_WorldMatrix());
		pEffectDesc.pTransform_Desc = &transformDesc;
		pEffectDesc.wstrLayerTag = L"Effect_Parts";
		pEffectDesc.iLevelIndex = iLevelID;

		// DTO 데이터를 툴 전용 Data 구조체로 일괄 복사
		pEffectDesc.Data = Part;

		// 타입 캐스팅 보정 (필요한 경우만 명시적 캐스팅)
		// 수명 및 리셋을 위한 초기화 플래그 세팅
		pEffectDesc.Data._Effect_TimeFlag = 2; // RESET 상태로 로딩

		// 파츠 생성 및 이름 복구
		// DTO의 eEffectParticleType에 따라 적절한 프로토타입으로 생성합니다.
		wstring strPrototype = L"Prototype_GameObject_Effect_Part_Particle";

		// (ForceField 타입이 따로 있다면 여기서 분기 처리)
		if (Part.eEffectSystemType == ENUM_TO_UINT(DTO::E_EffectSystemType::ForceField))
			strPrototype = L"Prototype_GameObject_Effect_Part_ForceField";

		static_cast<Effect*>(pEffectContainer)->Add_Part(index,
			ENUM_TO_UINT(ELevelType::EFFECT),
			strPrototype,
			&pEffectDesc
		);

		// 이름 재설정 (DTO에 저장된 PartsName 적용)
		_uint iLastIdx = (_uint)static_cast<Effect*>(pEffectContainer)->Get_PartList().size() - 1;
		auto pNewPart = static_cast<Effect*>(pEffectContainer)->Get_Part<CEffectObject>(iLastIdx);
		if (pNewPart)
			pNewPart->Set_Name(Part.EffectPartsName);

		index++;
	}
	}

}

void CImGui_Dockspace_MenuBar::Load_CameraData(const wstring& wstrFilePath)
{
}

void CImGui_Dockspace_MenuBar::Load_UIData(const wstring& wstrFilePath)
{
	ELevelType eLevelType = ELevelType::UI;
	DTO::ECategory eCategory = DTO::ECategory::UI;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, wstrFilePath)))
		return;

	m_pBuilderSystem->Build_File(iLevelID, eCategory, path(wstrFilePath).filename().stem().string());
}

void CImGui_Dockspace_MenuBar::Request_ExportData(ELevelType eLevelID, DTO::ECategory eCategory, const wstring& wstrLayerTag, CDataDocumentBase* pDocument)
{
	bool isSelectObjectSave = {false};

	CMapToolManager::GetInstance()->Export_SaveSceneData(eCategory,pDocument);

	if (isSelectObjectSave == false)
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

	else
	{
		if (list<CGameObject*>* pGameObjectList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(eLevelID), wstrLayerTag))
		{
			if (pGameObjectList->size() > 0)
			{
				for (auto itr = pGameObjectList->begin();
					itr != pGameObjectList->end();
					++itr)
				{
					CGameObject* pGameObj = *itr;
					if (!pGameObj) continue;
					if (pGameObj->IsDead() == true) continue;
					if (static_cast<CMapObject*>(pGameObj)->Get_MapObjectState() != CMapObject::EState::Select)
					(*itr)->Export_Data(eCategory, pDocument);
				}
			}
		}
	}
}

CImGui_Dockspace_MenuBar* CImGui_Dockspace_MenuBar::Create(const _char* pLabel, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CImGui_Dockspace_MenuBar* pDockspace = new CImGui_Dockspace_MenuBar(pLabel, pDevice, pDeviceContext);

	if (FAILED(pDockspace->Initialize()))
	{
		Safe_Release(pDockspace);
		MSG_BOX(" Dock Space Menu is failed to Create ");
		return nullptr;
	}

	return pDockspace;
}

void CImGui_Dockspace_MenuBar::Free()
{
	Safe_Release(m_pBuilderSystem);
	Safe_Release(m_pGameInstance);
	Super::Free();
}
