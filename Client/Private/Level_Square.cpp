#include "pch.h"
#include "Level_Square.h"
//=================
// Manager
//=================
#include "UI_Manager.h"

//=================
// Data Struct
//=================
#include "DataStruct_Effect.h"
#include "DataStruct_Map.h"
#include "DataStruct_UI.h"

//=================
// Builder
//=================
#include "Builder_UI.h"
#include "Builder_UIPrefabs.h"
#include "Builder_Example.h"
#include "BuilderSystem.h"
#include "Builder_Map.h"
#include "Builder_Effect.h"

#include "NPC_Citizen.h"
#include "CitizenData.h"
//=================
// Document
//=================
#include "DataDocument_Effect.h"
#include "DataDocument_Map.h"
#include "DataDocument_UI.h"


//=================
// UI
//=================
#include "Canvas.h"
#include "GenericUI.h"

//=================
// Component
//=================
#include "Bounds.h"
#include "PhysicsCCT.h"


//=================
// Game Object
//=================
#include "Player.h"
#include "CameraMan_Targeter.h"
#include "Effect.h"
#include "EffectObject.h"
#include "Physics_LandScape.h"
#include "Monster_Dog.h"
#include "Monster_Dog_Body.h"
#include "Boss_Xibi.h"
#include "Boss_Xibi_Body.h"
#include "PlayerSkillObj_Headers.h"

//=================
// Game Instance
//=================
#include "GameInstance.h"
#include "QuestManager.h"
#include "DialogueManager.h"

CLevel_Square::CLevel_Square(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice , pDeviceContext)
	, m_fAccTime{0.f}
{
}

HRESULT CLevel_Square::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Dissolve()))
		return E_FAIL;


	if (FAILED(Build_Prototype()))
	{
		MSG_BOX("CLevel_Square::Initialize, Build_Prototype Create Failed");
		return E_FAIL;
	}

	if (FAILED(Build_Files()))
	{
		MSG_BOX("CLevel_Square::Initialize, Build_Files Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("CLevel_Square::Initialize, Ready_Lights Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Player_Layer(g_wszPlayerLayer)))
	{
		MSG_BOX("CLevel_Square::Initialize, Ready_Player_Layer Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Camera_Layer(g_wszDynamicCameraLayer)))
	{
		MSG_BOX("CLevel_Square::Initialize, Ready_Camera_Layer Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("CLevel_Square::Initialize, Ready_Map Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_CitizenData()))
	{
		MSG_BOX("CLevel_Square::Initialize, Ready CitizenData Failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Square::Awake(const _uint iLevelID)
{
	CDialogueManager::GetInstance()->Initialize();

	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Octree()))
		return E_FAIL;

	if (FAILED(Ready_Camera_Setting(iLevelID)))
		return E_FAIL;
	
	if (FAILED(Setting_Citizen()))
		return E_FAIL;
	

	m_eCursorMode = ECursorMode::LockedHiddenCenter;
	m_pGameInstance->Request_CursorMode(m_eCursorMode);

	CQuestManager::GetInstance()->Start_Quest(3, 2);

	if (FAILED(m_pGameInstance->Set_Layer_UnscaledDomain(m_pGameInstance->Get_CurrentLevelIndex(), g_wszUILayer)))
		return E_FAIL;

	{
		UI_LEVEL_FADE_PREFAB_DATA Desc = {};
		Desc.fDelay = 1.f;
		Desc.fDuration = 2.f;
		Desc.isEased = false;
		Desc.fEaseValue = 2.f;
		Desc.isFadeIn = true;
		Desc.fEndDelay = 0.f;
		Desc.isChangeLevel = false;
		CUI_Manager::GetInstance()->Request_LevelChange_With_Fade(Desc);
	}

	m_pGameInstance->Play_OneShot(0, TO_HASH("SQUARE_BGM"), 0.5f);


	if (FAILED(m_pGameInstance->Bake_StaticShadow(m_pGameInstance->Get_MapMinMaxBounding())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::MAP, CBuilder_Map::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::SQUARE)))))
		return E_FAIL;

	if (FAILED(Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::SQUARE)))))
		return E_FAIL;

	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::SQUARE)))))
		return E_FAIL;

	if (FAILED(Ready_Builder(DTO::ECategory::UI_PREFAB, CBuilder_UIPrefabs::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::SQUARE)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Build_Files()
{
	ELevelType eLevelType = ELevelType::SQUARE;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

#pragma region EFFECT
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(iLevelID, eCategory)))
		return E_FAIL;

	std::filesystem::path strEffectFolderPath = L"../../Resources/Data/EffectData/";

	if (std::filesystem::exists(strEffectFolderPath))
	{
		for (const auto& entry : std::filesystem::recursive_directory_iterator(strEffectFolderPath))
		{
			if (std::filesystem::is_regular_file(entry.path()))
			{
				// 확장자가 .json인 것만 골라내기
				if (entry.path().extension() == ".json")
				{
					if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, entry.path())))
						return E_FAIL;

					if (FAILED(Build_File(iLevelID, eCategory, entry.path().stem().string())))
						return E_FAIL;
				}
			}
		}
	}
#pragma endregion

	eLevelType = ELevelType::SQUARE;
	iLevelID = ENUM_TO_UINT(eLevelType);


	eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;
	std::filesystem::path strUIFolderPath = L"../../Resources/Data/UIData/Static/";
	for (auto& iter : std::filesystem::recursive_directory_iterator(strUIFolderPath))
	{
		if (!iter.is_regular_file())
			continue;

		if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
			return E_FAIL;

		if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
			return E_FAIL;
	}

	eCategory = DTO::ECategory::UI_PREFAB;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;
	strUIFolderPath = L"../../Resources/Data/UIData/Prefab/";
	for (auto& iter : std::filesystem::recursive_directory_iterator(strUIFolderPath))
	{
		if (!iter.is_regular_file())
			continue;

		if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
			return E_FAIL;

		if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Square::Ready_Lights()
{
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = Vec3{ 0.3f, -1.f, 0.f };
		desc.vDirection.Normalize();
		desc.vDiffuse = Vec4(0.7f, 0.7f, 0.7f, 1.f);
		desc.vAmbient = Vec4(0.3f, 0.3f, 0.3f, 1.f);
		desc.vSpecular = desc.vDiffuse;

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	// FogDesc
	{
		SHADER_FOG_DESC& desc = m_pGameInstance->Get_FogParamDesc();
		desc.vColor = Vec4(0.35f, 0.35f, 0.35f, 1.f);
		desc.vHighColor = Vec4(0.31f, 0.31f, 0.31f, 1.f);

		// Distance
		desc.fFogStart = 100.f;
		desc.fFogEnd = 430.f;
		desc.fFogDensity = 0.f;
		desc.fFogMaxOpacity = 0.2f;

		// Height
		desc.fFogBaseHeight = -9.f;
		desc.fFogHeightFalloff = 0.08f;
		desc.fFogHeightDensity = 0.015f;

		// Noise
		desc.fFogNoiseScale = 0.15f;
		desc.fFogNoiseSpeed = 0.2f;
		m_pGameInstance->Commit_FogParam();
	}
	return S_OK;
}

HRESULT CLevel_Square::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	_uint iLevelIndex = ENUM_TO_UINT(ELevelType::SQUARE);

	// TODO : 만약 플레이어가 늘어난다면 레이어 추가 체크 필수
	if (CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), wstrLayerTag))
	{
		CGameObject::GAMEOBJECT_REINIT_DESC desc{};
		pPlayer->Reinitialize(&desc);
		return S_OK;
	}

	/* Player 최초 생성 */
	{
		// Moon skil E
		{
			CMoon_SkillE_Obj::GAMEOBJECT_DESC desc{};
			//TRANSFORM_DESC
			CTransform::TRANSFORM_DESC tTransDesc = {};
			tTransDesc.fMovePerSec = 20.f;
			desc.pTransform_Desc = &tTransDesc;

			if (FAILED(m_pGameInstance->Regist_Pool(
				0,
				g_wszPool_MoonSkillE,
				g_wszSkillObjectLayer,
				0,
				g_wszMoonSkillE__Prototype_Tag,
				&desc,
				30)))
				return E_FAIL;
		}

		// Moon skil Q attack
		{
			CMoon_SkillQAttack_Obj::SKILLOBJECT_SPAWN_DESC desc{};
			desc.fSpeed = 50.f;
			desc.fLifeTime = 12.5f;
			desc.iFlags = ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);

			if (FAILED(m_pGameInstance->Regist_Pool(
				0,
				g_wszPool_MoonSkillQAttack,
				g_wszSkillObjectLayer,
				0,
				g_wszMoonSkillQAttack_Prototype_Tag,
				&desc,
				10)))
				return E_FAIL;
		}

		CGameObject* pResult = { nullptr };

		CPlayer::PLAYER_DESC playerDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		playerDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::SQUARE);
		playerDesc.wstrBodyModelTag = L"Prototype_Component_Model_Moon";
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(15.f, 15.f, 15.f));
		playerDesc.pTransform_Desc = &transformDesc;
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_MainPlayer",
			ENUM_TO_UINT(ELevelType::STATIC),
			wstrLayerTag, &playerDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Square::Ready_Camera_Layer(const wstring& wstrLayerTag)
{
	{
		CGameObject* pResult = { nullptr };
		CCameraMan_Targeter::GAMEOBJECT_DESC goDesc = {};
		CTransform::TRANSFORM_DESC TransformDesc = {};
		CCamera::CAMERA_DESC CameraDesc = {};

		CameraDesc.eProjectionType = EProjectionType::PERSPECTIVE;
		CameraDesc.fFov = ::XMConvertToRadians(60.f);
		CameraDesc.fViewWidth = (_float)g_iWinSizeX;
		CameraDesc.fViewHeight = (_float)g_iWinSizeY;
		CameraDesc.fNear = 0.1f;
		CameraDesc.fFar = 1000.f;

		goDesc.pTransform_Desc = &TransformDesc;
		goDesc.pCamera_Desc = &CameraDesc;
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_CameraManTargeter",
			ENUM_TO_UINT(ELevelType::SQUARE),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Square::Ready_Map()
{
	ELevelType		eLevelType = ELevelType::SQUARE;
	DTO::ECategory	eCategory = DTO::ECategory::MAP;
	_uint			iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return E_FAIL;

	/* Dev Map */
	std::filesystem::path FilePath = L"../../Resources/Data/MapData/LevelData/Square/Square.json";

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Square::Ready_Octree()
{
	// 순회하며 OCTREE BOX 사이즈 검출
	auto* pList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::SQUARE), g_wszStaticObjectLayer);

	if (pList == nullptr) return S_OK;

	// Registe에 필요한 Object, Bound 버퍼 reserve
	vector<CGameObject*> vecWillReigstObject;
	vector<BoundingBox*> vecWillRegistBounds;
	vecWillReigstObject.reserve(pList->size());
	vecWillRegistBounds.reserve(pList->size());
	{
		Vec3 vMin{ FLT_MAX, FLT_MAX, FLT_MAX };
		Vec3 vMax{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
		// 사이즈 검출 및 버퍼에 밀어넣기
		for (auto* pElement : *pList)
		{
			CBounds* pBounds = pElement->Get_Component<CBounds>();
			if (pBounds == nullptr)
				continue;

			vecWillReigstObject.push_back(pElement);
			vecWillRegistBounds.push_back(pBounds->Get_WolrdAABB());
			const BoundingBox& AABB = *pBounds->Get_WolrdAABB();

			Vec3 vElementMinMax[2] =
			{
				AABB.Center - AABB.Extents,
				AABB.Center + AABB.Extents
			};

			Engine_Utils::Merge_MinMax(vElementMinMax, vMin, vMax);
		}

		// 안맞으면 FAIL
		if (vecWillRegistBounds.size() != vecWillReigstObject.size())
			return E_FAIL;

		// RootBox 생성
		const _float fMargin = 50.f;

		vMin -= Vec3(fMargin, fMargin, fMargin);
		vMax += Vec3(fMargin, fMargin, fMargin);

		const Vec3 vFinalCenter = (vMin + vMax) * 0.5f;
		const Vec3 vFinalExtents = (vMax - vMin) * 0.5f;

#ifdef _DEBUG
		string strLog{
			"RootBound Center = X: " + std::to_string(vFinalCenter.x) + "/ Y: " + std::to_string(vFinalCenter.y) + "/ Z: " + std::to_string(vFinalCenter.z)
		};
		CLOG_INFO(strLog);
		strLog = {
			"RootBound Extents = X: " + std::to_string(vFinalExtents.x) + "/ Y: " + std::to_string(vFinalExtents.y) + "/ Z: " + std::to_string(vFinalExtents.z)
		};
		CLOG_INFO(strLog);
#endif


		// RootBounds 생성
		OCTREE_DESC desc{};
		desc.rootBounds = BoundingBox(vFinalCenter, vFinalExtents);
		if (FAILED(m_pGameInstance->Ready_Octree(desc)))
			return E_FAIL;
	}

	// 이제 버퍼를 순회하며 옥트리에 등록
	for (size_t i = 0; i < vecWillReigstObject.size(); ++i)
	{
		if (FAILED(m_pGameInstance->Register_Octree(
			vecWillReigstObject[i],
			RENDER_CATEGORY::NONEBLEND,
			*vecWillRegistBounds[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Square::Ready_CitizenData()
{
	_uint iCurLevelIndex = ENUM_TO_UINT(ELevelType::SQUARE);

	auto iter = DTO::CitizenWayPointOriginData::mapCitizenWapointDatas.find(LevelTypeToString(ENUM_TO_UINT(ELevelType::SQUARE)));
	if (iter == DTO::CitizenWayPointOriginData::mapCitizenWapointDatas.end())
	{
		MSG_BOX("로드 된 Citizen Way Point Data가 없습니다");
		return S_OK;
	}

	const vector<DTO::CITIZEN_DATA>& vecData = DTO::CitizenPresetData::vecDatas;

	/* Preset 모델 개수만큼 생성 */
	CNPC_Citizen::NPC_CITIZEN_DESC tDesc{};
	static_cast<CNPC_Base::NPC_DESC&>(tDesc) = CNPC_Citizen::Get_PreSetDesc(iCurLevelIndex);
	tDesc.isWalking = true;


	/* Preset 개수만큼 */
	m_pGameInstance->Regist_Pool(ENUM_TO_UINT(ELevelType::SQUARE), L"Pool_Citizen", g_wszNPCCitizenPoolLayer, ENUM_TO_UINT(ELevelType::STATIC),
		g_wszNPC_Citizen_Prototype_Tag,&tDesc, (_uint)vecData.size());


	/* WayPoint 의 개수 만큼 Pool 개수 생성 */

	return S_OK;
}

HRESULT CLevel_Square::Setting_Citizen()
{
	/* 등록된 NPC 재생시키기 */
	CNPC_Citizen::NPC_CITIZEN_POOL_DESC tPoolDesc{};

	tPoolDesc.tMoveData = { DTO::Get_RandomCitizenMoveData(LevelTypeToString(ENUM_TO_UINT(ELevelType::SQUARE))) };

	if (tPoolDesc.tMoveData.pWayPointData == nullptr)	/* WayPoint가 없다면 Failed */
		return E_FAIL;

	m_pGameInstance->Request_AddObject(ENUM_TO_UINT(ELevelType::SQUARE) , L"Pool_Citizen", ENUM_TO_UINT(ELevelType::SQUARE) , &tPoolDesc);

	return S_OK;
}

void CLevel_Square::Check_Citizen()
{
	_uint iLevelIndex = ENUM_TO_UINT(ELevelType::SQUARE);
	const list<CGameObject*>* pList = m_pGameInstance->Get_GameObject_List(iLevelIndex, g_wszNPCCitizenPoolLayer);


	/* 해당 라인에 NPC를 보내준다 */

	for (auto& pObj : *pList)
	{
		if (!pObj)
			continue;
		_bool isArrive = static_cast<CNPC_Citizen*>(pObj)->Get_IsArrive();

		if (isArrive)
		{
			m_pGameInstance->Request_DeleteGameObject(iLevelIndex,pObj);
		}
	}


	return;
}

HRESULT CLevel_Square::Ready_Camera_Setting(const _uint iLevelIndex)
{
	CGameObject* pMainCamera = m_pGameInstance->Get_GameObject_Front(iLevelIndex, g_wszDynamicCameraLayer);
	m_pGameInstance->Add_Camera(CameraType::DYNAMIC, g_MainActorCameraName, static_cast<CCameraMan*>(pMainCamera));
	m_pGameInstance->Change_MainCamera(CameraType::DYNAMIC, g_MainActorCameraName);
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	m_pGameInstance->Change_Target(pPlayer);
	m_pGameInstance->Ready_Frustrum();
	return S_OK;
}

HRESULT CLevel_Square::Ready_Dissolve()
{
	return m_pGameInstance->Ready_DissolveSetting();
}

CLevel_Square* CLevel_Square::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Square* pInstance = new CLevel_Square(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Square::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Square::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	
	
	m_fAccTime += fTimeDelta;


	if (m_fAccTime >= 5.5f)
	{
		Setting_Citizen();
		m_fAccTime = 0.f;
	}

	Check_Citizen();


//
//	static _uint s_iCount = { 0 };
//	if (m_pGameInstance->KeyButton_Down(DIK_LALT))
//	{
//#ifdef _DEBUG
//		s_iCount = (s_iCount + 1) % 3;
//#else
//		s_iCount = (s_iCount + 1) % 2;
//#endif
//		if (s_iCount == 0)
//		{
//			m_eCursorMode = ECursorMode::LockedHiddenCenter;
//		}
//		else if (s_iCount == 1)
//		{
//			m_eCursorMode = ECursorMode::VisibleClipped;
//		}
//#ifdef _DEBUG
//		else
//		{
//			m_eCursorMode = ECursorMode::VisibleFree;
//		}
//#endif
//		m_pGameInstance->Request_CursorMode(m_eCursorMode);
//	}
}

HRESULT CLevel_Square::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CLevel_Square::Free()
{
	Super::Free();
}