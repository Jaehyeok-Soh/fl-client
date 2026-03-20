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
#include "Builder_UIPrefabs.h"
#include "Builder_Example.h"
#include "BuilderSystem.h"
#include "Builder_Map.h"
#include "Builder_Effect.h"
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
#include "CinematicCamera.h"
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
#include "PhysicsCCT.h"

#include "GameInstance.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
	{
		MSG_BOX("CLevel_Logo::Initialize, Build_Prototype Create Failed");
		return E_FAIL;
	}

	if (FAILED(Build_Files()))
	{
		return E_FAIL;
	}

	/* 카메라 생성 */
	if (FAILED(Ready_Camera_Layer(g_wszDynamicCameraLayer)))
	{
		MSG_BOX("CLevel_Logo::Initialize, Ready_Camera_Layer Create Failed");
		return E_FAIL;
	}

	if (FAILED(Ready_UI_Layer(g_wszUILayer)))
	{
		MSG_BOX("CLevel_Logo::Initialize, Ready_UI_Layer Create Failed");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Awake(const _uint iLevelID)
{
	if (FAILED(Super::Awake(iLevelID)))
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

HRESULT CLevel_Logo::Build_Prototype()
{
	if (FAILED(Ready_Builder(DTO::ECategory::MAP,CBuilder_Map::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::LOGO)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::UI, CBuilder_UI::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::LOGO)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::UI_PREFAB, CBuilder_UIPrefabs::Create(m_pDevice, m_pDeviceContext, static_cast<_uint>(ELevelType::LOGO)))))
		return E_FAIL;
	if (FAILED(Ready_Builder(DTO::ECategory::EFFECT, CBuilder_Effect::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::LOGO)))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Build_Files()
{
	ELevelType eLevelType = ELevelType::LOGO;
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

	// For. Example
	//if (FAILED(Build_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::MAP, "asdf")))
	//	return E_FAIL;

	eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;
	std::filesystem::path strUIFolderPath = L"../../Resources/Data/UIData/Logo/";

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

HRESULT CLevel_Logo::Ready_UI_Layer(const wstring& wstrLayerTag)
{
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

HRESULT CLevel_Logo::Ready_CinematicCamera()
{

	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC),L"Prototype_GameObject_CinematicCamera",CCinematicCamera::Create(m_pDevice,m_pDeviceContext));
	
	if (FAILED(m_pGameInstance->Register_CinematicCamera(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_CinematicCamera", ENUM_TO_UINT(ELevelType::STATIC), L"CinematicCamera_Layer")))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Lights()
{
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
