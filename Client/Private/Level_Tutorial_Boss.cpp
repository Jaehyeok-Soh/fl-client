#include "pch.h"
#include "Level_Tutorial_Boss.h"


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


//=================
// Game Instance
//=================
#include "GameInstance.h"

CLevel_Tutorial_Boss::CLevel_Tutorial_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice , pDeviceContext)
{
}

HRESULT CLevel_Tutorial_Boss::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Build_Prototype Create Failed");
		return E_FAIL;
	}

	if (FAILED(Build_Files()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Build_Files Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Lights()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Ready_Lights Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Player_Layer(g_wszPlayerLayer)))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Ready_Player_Layer Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Camera_Layer(g_wszDynamicCameraLayer)))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Ready_Camera_Layer Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_Map()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Initialize, Ready_Map Create Failed");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::MAP, CBuilder_Map::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::TUTORIAL_BOSS)))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Build_Files()
{
	ELevelType eLevelType = ELevelType::TUTORIAL_BOSS;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

#pragma region EFFECT
	DTO::ECategory eCategory = DTO::ECategory::EFFECT;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(iLevelID, eCategory)))
		return E_FAIL;
	std::filesystem::path strUIFolderPath = L"../../Resources/Data/EffectData/";
	if (std::filesystem::exists(strUIFolderPath))
	{
		for (auto iter : std::filesystem::directory_iterator(strUIFolderPath))
		{
			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}
#pragma endregion

	eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;

	strUIFolderPath = L"../../Resources/Data/UIData/Static/";
	if (std::filesystem::exists(strUIFolderPath))
	{
		for (auto iter : std::filesystem::directory_iterator(strUIFolderPath))
		{
			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Ready_Lights()
{
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = Vec3{ 1.f, -1.f, 1.f };
		desc.vDiffuse = Vec4(0.7f, 0.7f, 0.7f, 1.f);
		desc.vAmbient = Vec4(0.3f, 0.3f, 0.3f, 1.f);
		desc.vSpecular = desc.vDiffuse;

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::STATICPOINT;
		desc.vDiffuse = Vec4(0.5f, 0.3f, 0.7f, 1.f);
		desc.vAmbient = Vec4(0.2f, 0.1f, 0.3f, 1.f);
		desc.vSpecular = desc.vDiffuse;
		desc.vPosition = Vec4(21.f, 18.f, 0.f, 1.f);
		desc.fRange = 10.f;

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::STATICPOINT;
		desc.vDiffuse = Vec4(0.3f, 0.6f, 0.4f, 1.f);
		desc.vAmbient = Vec4(0.1f, 0.3f, 0.2f, 1.f);
		desc.vSpecular = desc.vDiffuse;
		desc.vPosition = Vec4(21.f, 14.5f, 25.f, 1.f);
		desc.fRange = 10.f;

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Ready_Camera_Layer(const wstring& wstrLayerTag)
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
			ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CLevel_Tutorial_Boss::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	/* Player 최초 생성 */
	{
		CGameObject* pResult = { nullptr };

		CPlayer::PLAYER_DESC playerDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		playerDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
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


HRESULT CLevel_Tutorial_Boss::Ready_Camera_Setting(const _uint iLevelIndex)
{
	CGameObject* pMainCamera = m_pGameInstance->Get_GameObject_Front(iLevelIndex, g_wszDynamicCameraLayer);
	m_pGameInstance->Add_Camera(CameraType::DYNAMIC, g_MainActorCameraName, static_cast<CCameraMan*>(pMainCamera));
	m_pGameInstance->Change_MainCamera(CameraType::DYNAMIC, g_MainActorCameraName);
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Front(/* static */ 0, g_wszPlayerLayer);
	if (pPlayer == nullptr)
		return E_FAIL;

	m_pGameInstance->Change_Target(pPlayer);
	m_pGameInstance->Ready_Frustrum();

	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Ready_Octree()
{
	// 순회하며 OCTREE BOX 사이즈 검출
	auto* pList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS), g_wszStaticObjectLayer);

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

HRESULT CLevel_Tutorial_Boss::Ready_Map()
{
	ELevelType eLevelType = ELevelType::TUTORIAL_BOSS;
	DTO::ECategory eCategory = DTO::ECategory::MAP;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return E_FAIL;

	/* Dev Map */
	std::filesystem::path FilePath = L"../../Resources/Data/MapData/LevelData/Tutorial/Tutorial_Boss.json";


	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Tutorial_Boss::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Octree()))
		return E_FAIL;

	if (FAILED(Ready_Camera_Setting(iLevelID)))
		return E_FAIL;



	m_eCursorMode = ECursorMode::LockedHiddenCenter;
	m_pGameInstance->Request_CursorMode(m_eCursorMode);


	return S_OK;
}

void CLevel_Tutorial_Boss::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	static _uint s_iCount = { 0 };
	if (m_pGameInstance->KeyButton_Down(DIK_LALT))
	{
#ifdef _DEBUG
		s_iCount = (s_iCount + 1) % 3;
#else
		s_iCount = (s_iCount + 1) % 2;
#endif
		if (s_iCount == 0)
		{
			m_eCursorMode = ECursorMode::LockedHiddenCenter;
		}
		else if (s_iCount == 1)
		{
			m_eCursorMode = ECursorMode::VisibleClipped;
		}
#ifdef _DEBUG
		else
		{
			m_eCursorMode = ECursorMode::VisibleFree;
		}
#endif
		m_pGameInstance->Request_CursorMode(m_eCursorMode);
	}

}

HRESULT CLevel_Tutorial_Boss::Render()
{
	if(FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

CLevel_Tutorial_Boss* CLevel_Tutorial_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Tutorial_Boss* pInstance = new CLevel_Tutorial_Boss(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Tutorial_Boss::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Tutorial_Boss::Free()
{
	Super::Free();
}