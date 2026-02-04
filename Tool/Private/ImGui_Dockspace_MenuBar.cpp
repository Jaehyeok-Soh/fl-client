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
	Request_ExportData(eLevelType, DTO::ECategory::MAP, g_wszStaticModelLayer	, pDocument);
	Request_ExportData(eLevelType, DTO::ECategory::MAP, g_wszInstanceModelLayer , pDocument);

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
	wstring ContainerObjectLayerTag = L"Effect";

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

	if (okay.size() > 0)
		MSG_BOX("Okay");
	else
		MSG_BOX("Failed");

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
		pDesc._Effect_SimulationType = (E_SIMULATION_SPACE)pData._Effect_SimulationType;
		pDesc.wstrLayerTag = L"Effect";
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
			// 1. Transform 데이터 복원 (Matrix -> Vec3)
			Vec3 vScale, vPos;
			Quat vQuat;
			Part.vWorldMatrix.Decompose(vScale, vQuat, vPos);

			CEffectObject::Effect_Desc pEffectDesc = {};
			CTransform::TRANSFORM_DESC transformDesc = {};
			transformDesc.TranslationMatrix = Matrix::CreateScale(vScale);
			transformDesc.ScaleMatrix = Matrix::CreateTranslation(vPos);
			transformDesc.RotationMatrix = Matrix::CreateFromQuaternion(vQuat);
			transformDesc.fRotatePerSec = 1.f;
			transformDesc.fMovePerSec = 1.f;

			//===========    부모 행렬 연결 및 기본 설정   ==============
			pEffectDesc.pMatParent = &(pEffectContainer->Get_Component<CTransform>()->Get_WorldMatrix());
			pEffectDesc.pTransform_Desc = &transformDesc;
			pEffectDesc.wstrLayerTag = L"Effect_Parts";
			pEffectDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::EFFECT);

			// ==========   이펙트 타입 및 시스템 설정 ==============
			pEffectDesc.eEffectSystemType = (E_EffectSystemType)Part.eEffectSystemType;
			pEffectDesc.eEffectParticleType = (E_PARTICLETYPE)Part.eEffectParticleType;
			pEffectDesc.eEffectType = (E_EFFECTTYPE)Part.eEffectType;
			pEffectDesc._Effect_ShapeType = (E_SHAPETYPE)Part._Effect_ShapeType;

			// ==========    리소스 태그 복원  ===============
			pEffectDesc._Effect_Model_Tag = Part._Effect_Model_Tag;
			pEffectDesc._Effect_DiffuseTexture_Tag = Part._Effect_DiffuseTexture_Tag;
			pEffectDesc._Effect_NoiseTexture_Tag = Part._Effect_NoiseTexture_Tag;
			pEffectDesc._Effect_MaskingTexture_Tag = Part._Effect_MaskingTexture_Tag;
			pEffectDesc._Effect_GradationTexture_Tag = Part._Effect_GradationTexture_Tag;
			pEffectDesc._Effect_TrailTexture_Tag = Part._Effect_TrailTexture_Tag;
			pEffectDesc._Effect_NormalTexture_Tag = Part._Effect_NormalTexture_Tag;

			// =====    셰이더 설정   =====================
			pEffectDesc._Effect_Shader_Tag = Part._Effect_Shader_Tag;
			pEffectDesc._Effect_ShaderPass = Part._Effect_ShaderPass;

			// ======   수치 및 컬러 데이터 복원   ===========
			pEffectDesc._Effect_ScrollSpeed = Part._Effect_ScrollSpeed;
			pEffectDesc._Effect_DistortionScale = Part._Effect_DistortionScale;
			pEffectDesc._Effect_StartScale = Part._Effect_StartScale;
			pEffectDesc._Effect_EndScale = Part._Effect_EndScale;
			pEffectDesc._Effect_Color = Part._Effect_Color;
			pEffectDesc._Effect_DiscardValue = Part._Effect_DiscardValue;
			pEffectDesc._Effect_Range = Part._Effect_Range;
			pEffectDesc._Effect_ParticleSize = Part._Effect_ParticleSize;

			// =====   스프라이트 및 애니메이션 설정   ========
			pEffectDesc._Effect_bUseSprite = Part._Effect_bUseSprite;
			pEffectDesc._Effect_TileCount = { Part._Effect_TileCount.x, Part._Effect_TileCount.y };
			pEffectDesc._Effect_bPlayAnim = Part._Effect_bPlayAnim;
			pEffectDesc._Effect_AnimSpeed = Part._Effect_AnimSpeed;
			pEffectDesc.m_iCurSpriteNumber = Part.m_iCurSpriteNumber;

			//=======   파티클 시스템 상세 설정   ========
			pEffectDesc._Effect_Duration = Part._Effect_Duration;
			pEffectDesc._Effect_Looping = Part._Effect_Looping;
			pEffectDesc._Effect_IsRandomSeed = Part._Effect_IsRandomSeed;
			pEffectDesc._Effect_StartDelay = Part._Effect_StartDelay;
			pEffectDesc._Effect_LifeTime = Part._Effect_LifeTime;
			pEffectDesc._Effect_PlayBackSpeed = Part._Effect_PlayBackSpeed;
			pEffectDesc._Effect_StartSpeed = Part._Effect_StartSpeed;
			pEffectDesc._Effect_MaxParticle = Part._Effect_MaxParticle;
			pEffectDesc._Effect_RateOverTime = Part._Effect_RateOverTime;
			pEffectDesc._Effect_RateOverDistance = Part._Effect_RateOverDistance;

			// ===========  및 렌더링 플래그   =============
			pEffectDesc._Effect_TextureFlag = Part._Effect_TextureFlag;
			pEffectDesc._Effect_RenderFlag = Part._Effect_RenderFlag;
			pEffectDesc._Effect_SamplerStateFlag = Part._Effect_SamplerStateFlag;

			pEffectDesc._Effect_Tool_DiffuseTexture = Part._Effect_Tool_DiffuseTexture;
			pEffectDesc._Effect_Tool_NoiseTexture = Part._Effect_Tool_NoiseTexture;
			pEffectDesc._Effect_Tool_MaskingTexture = Part._Effect_Tool_MaskingTexture;
			pEffectDesc._Effect_Tool_GradationTexture = Part._Effect_Tool_GradationTexture;

			// ===========   빌보드 및 스크롤 옵션 상태 복원   ===========
			pEffectDesc._Effect_Tool_UseBillboard = Part._Effect_Tool_UseBillboard;
			pEffectDesc._Effect_Tool_UseScroll = Part._Effect_Tool_UseScroll;
			pEffectDesc._Effect_Tool_RightScroll = Part._Effect_Tool_RightScroll;
			pEffectDesc._Effect_Tool_DownScroll = Part._Effect_Tool_DownScroll;

			// ===========   샘플러 스테이트 인덱스 복원   ===========
			pEffectDesc._Effect_Tool_DiffuseSamplerState_Flag = Part._Effect_Tool_DiffuseSamplerState_Flag;
			pEffectDesc._Effect_Tool_NoiseSamplerState_Flag = Part._Effect_Tool_NoiseSamplerState_Flag;
			pEffectDesc._Effect_Tool_MaskingSamplerState_Flag = Part._Effect_Tool_MaskingSamplerState_Flag;
			pEffectDesc._Effect_Tool_GradationSamplerState_Flag = Part._Effect_Tool_GradationSamplerState_Flag;

			static_cast<Effect*>(pEffectContainer)->Add_Part(index,
				ENUM_TO_UINT(ELevelType::EFFECT),
				L"Prototype_GameObject_Effect_Parts",
				&pEffectDesc
			);

			//========  생성된 파츠 이름 설정  =============

			_uint iLastIdx = (_uint)static_cast<Effect*>(pEffectContainer)->Get_PartList().size() - 1;
			static_cast<Effect*>(pEffectContainer)->Get_Part<CEffectObject>(iLastIdx)->Set_Name(Part.EffectPartsName);

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
