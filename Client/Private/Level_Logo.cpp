#include "pch.h"
#include "Level_Loading.h"
#include "Level_Logo.h"
//=================
// Manager
//=================
#include "UI_Manager.h"

//=================
// Builder
//=================
#include "Builder_UI.h"
#include "Builder_Example.h"
#include "BuilderSystem.h"
#include "Builder_Map.h"
#include "EffectBuilder.h"
#include "DataStruct_Effect.h"
#include "DataDocument_Effect.h"
#include "DataDocument_Map.h"

//=================
// Object
//=================
#include "Player.h"
#include "CameraMan_Targeter.h"
#include "Effect.h"
#include "EffectObject.h"
#include "Physics_LandScape.h"

//=================
// UI
//=================
#include "DataDocument_UI.h"
#include "DataStruct_UI.h"
#include "Canvas.h"
#include "GenericUI.h"

//=================
// Component
//=================
#include "Bounds.h"

#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Builders()))
		return E_FAIL;

	if (FAILED(Build_Files()))
		return E_FAIL;

	if (FAILED(Ready_Camera_Layer(g_wszDynamicCameraLayer)))
		return E_FAIL;

	if (FAILED(Ready_Player_Layer(g_wszPlayerLayer)))
		return E_FAIL;

	if (FAILED(Ready_DevMap()))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(g_wszUILayer)))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
		return E_FAIL;

	if (FAILED(Ready_Octree()))
		return E_FAIL;

	if (FAILED(Ready_Camera_Setting(iLevelID)))
		return E_FAIL;

	m_eCursorMode = ECursorMode::LockedHiddenCenter;
	m_pGameInstance->Request_CursorMode(m_eCursorMode);

	CLOG_TRACE(L"테스트, Logo Awake() 확인");
	CLOG_INFO(L"테스트, Logo Awake() 확인");
	CLOG_WARN(L"테스트, Logo Awake() 확인");
	CLOG_ERROR(L"테스트, Logo Awake() 확인");
	return S_OK;
}

void CLevel_Logo::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// TODO : 어디다 두지?
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

HRESULT CLevel_Logo::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Builders()
{
	if (FAILED(Ready_Builder(DTO::ECategory::MAP,CBuilder_Map::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::LOGO)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::STATIC)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::EFFECT, EffectBuilder::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::LOGO)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Build_Files()
{
	//if (FAILED(Build_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::EFFECT, "Attack_1")))
	//	return E_FAIL;

	// For. Example
	//if (FAILED(Build_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::MAP, "asdf")))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	{
		CGameObject* pResult = { nullptr };

		CPlayer::PLAYER_DESC playerDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		playerDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
		playerDesc.wstrBodyModelTag = L"Prototype_Component_Model_Master";
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(18.f,12.f,19.f));
		playerDesc.pTransform_Desc = &transformDesc;
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_MainPlayer",
			ENUM_TO_UINT(ELevelType::LOGO),
			wstrLayerTag, &playerDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_UI_Layer(const wstring& wstrLayerTag)
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::UI;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;

	std::filesystem::path strFolderPath = L"../../Resources/Data/UIData/Logo/";
	vector<path> vecfiles;

	if (std::filesystem::exists(strFolderPath))
	{
		for (auto iter : std::filesystem::directory_iterator(strFolderPath))
		{
			if (iter.is_regular_file())
				vecfiles.push_back(iter.path().stem());

			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Camera_Layer(const wstring& wstrLayerTag)
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
			ENUM_TO_UINT(ELevelType::LOGO),
			wstrLayerTag, &goDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Lights()
{
	{
		LIGHT_DESC desc = {};
		desc.eType = LIGHT_TYPE::DIRECTIONAL;
		desc.vDirection = Vec3{ 1.f, -1.f, 1.f };
		desc.vDiffuse = Vec4(0.7f, 0.7f, 0.7f, 1.f);
		desc.vAmbient = Vec4(0.3f, 0.3f, 0.3f, 1.f);
		desc.vSpecular = Vec4(1.f, 1.f, 1.f, 1.f);

		if (FAILED(m_pGameInstance->Add_Light(desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_DevMap()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::MAP;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(iLevelID, eCategory)))
		return E_FAIL;

	std::filesystem::path FilePath = L"../../Resources/Data/MapData/LevelData/DevLevel/DevMap.json";
	vector<path> vecfiles;

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory , FilePath )))
		return E_FAIL;

	if (FAILED(Build_File(iLevelID,eCategory,FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Camera_Setting(const _uint iLevelIndex)
{
	CGameObject* pMainCamera = m_pGameInstance->Get_GameObject_Front(iLevelIndex, g_wszDynamicCameraLayer);
	m_pGameInstance->Add_Camera(CameraType::DYNAMIC, g_MainActorCameraName, static_cast<CCameraMan*>(pMainCamera));
	m_pGameInstance->Change_MainCamera(CameraType::DYNAMIC, g_MainActorCameraName);
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Front(iLevelIndex, g_wszPlayerLayer);
	m_pGameInstance->Change_Target(pPlayer);
	m_pGameInstance->Ready_Frustrum();
	return S_OK;
}

HRESULT CLevel_Logo::Ready_Octree()
{
	// 순회하며 OCTREE BOX 사이즈 검출
	auto* pList = m_pGameInstance->Get_GameObject_List(ENUM_TO_UINT(ELevelType::LOGO), g_wszStaticObjectLayer);
	
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


CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CLevel_Logo::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	m_pGameInstance->Clear_Lights();
	Super::Free();
}
