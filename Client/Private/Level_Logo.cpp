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
#include "Monster_Dummy.h"
#include "Monster_Dummy_Body.h"

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
#include "Level_Square.h"
#include "Level_Tutorial_Village.h"
#include "Level_Tutorial_Boss.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::Initialize()
{
	if (FAILED(Super::Initialize()))
		return E_FAIL;

	if (FAILED(Build_Prototype()))
		return E_FAIL;

	if (FAILED(Build_Files()))
		return E_FAIL;

	/* 플레이어 제일먼저 세팅 */
	if (FAILED(Ready_Player_Layer(g_wszPlayerLayer)))
		return E_FAIL;


	/* 카메라 생성 */
	if (FAILED(Ready_Camera_Layer(g_wszDynamicCameraLayer)))
		return E_FAIL;

	/* 카메라 생성 후 세팅 */
	if (FAILED(Ready_Camera_Setting(ENUM_TO_UINT(ELevelType::LOGO))))
		return E_FAIL;

	if (FAILED(Ready_UI_Layer(g_wszUILayer)))
		return E_FAIL;


	/* 임시 주석처리 */
	//if (FAILED(Ready_Lights()))
	//	return E_FAIL;


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


	// 오브젝트 풀링 테스트
	if (m_pGameInstance->KeyButton_Down(DIK_0))
	{
		m_pGameInstance->Request_AddObject(ENUM_TO_UINT(ELevelType::LOGO), L"POOL_Attack_1", 0, nullptr);
	}

	// GlobalTimeScale 테스트
	{
		if (m_pGameInstance->KeyButton_Down(DIK_9))
		{
			m_pGameInstance->Request_HitStop();
		}
		if (m_pGameInstance->KeyButton_Down(DIK_8))
		{
			m_pGameInstance->Request_SloMo(0.2f, 2.f);
		}
		if (m_pGameInstance->KeyButton_Down(DIK_6))
		{
			m_pGameInstance->Active_SloMo(0.5f);
		}
		if (m_pGameInstance->KeyButton_Down(DIK_7))
		{
			m_pGameInstance->Deactivate_SloMo();

			
		}
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

	// For. Example
	//if (FAILED(Build_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::MAP, "asdf")))
	//	return E_FAIL;

	eCategory = DTO::ECategory::UI;
	if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(iLevelID, eCategory)))
		return E_FAIL;
	strUIFolderPath = L"../../Resources/Data/UIData/Logo/";
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

HRESULT CLevel_Logo::Ready_Player_Layer(const wstring& wstrLayerTag)
{
	/* Player 최초 생성 */


	{
		CGameObject* pResult = { nullptr };

		CPlayer::PLAYER_DESC playerDesc = {};
		CTransform::TRANSFORM_DESC transformDesc = {};
		playerDesc.iLevelIndex = ENUM_TO_UINT(ELevelType::LOGO);
		playerDesc.wstrBodyModelTag = L"Prototype_Component_Model_Master";
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(Vec3(229.12f,256.72f,-245.039f));
		playerDesc.pTransform_Desc = &transformDesc;
		if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_GameObject_MainPlayer",
			ENUM_TO_UINT(ELevelType::STATIC),
			wstrLayerTag, &playerDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_Logo::Ready_UI_Layer(const wstring& wstrLayerTag)
{
	if (FAILED(CUI_Manager::GetInstance()->Bind_Trigger(ENUM_TO_UINT(ELevelType::LOGO))))
		return E_FAIL;

	CUI_Manager::GetInstance()->Clear_TriggerUI();
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
	/* 임시 주석처리 */

	//{
	//	LIGHT_DESC desc = {};
	//	desc.eType = LIGHT_TYPE::DIRECTIONAL;
	//	desc.vDirection = Vec3{ 1.f, -1.f, 1.f };
	//	desc.vDiffuse = Vec4(0.7f, 0.7f, 0.7f, 1.f);
	//	desc.vAmbient = Vec4(0.3f, 0.3f, 0.3f, 1.f);
	//	desc.vSpecular = desc.vDiffuse;

	//	if (FAILED(m_pGameInstance->Add_Light(desc)))
	//		return E_FAIL;
	//}
	//{
	//	LIGHT_DESC desc = {};
	//	desc.eType = LIGHT_TYPE::STATICPOINT;
	//	desc.vDiffuse = Vec4(0.5f, 0.3f, 0.7f, 1.f);
	//	desc.vAmbient = Vec4(0.2f, 0.1f, 0.3f, 1.f);
	//	desc.vSpecular = desc.vDiffuse;
	//	desc.vPosition = Vec4(21.f, 18.f, 0.f, 1.f);
	//	desc.fRange = 10.f;

	//	if (FAILED(m_pGameInstance->Add_Light(desc)))
	//		return E_FAIL;
	//}
	//{
	//	LIGHT_DESC desc = {};
	//	desc.eType = LIGHT_TYPE::STATICPOINT;
	//	desc.vDiffuse = Vec4(0.3f, 0.6f, 0.4f, 1.f);
	//	desc.vAmbient = Vec4(0.1f, 0.3f, 0.2f, 1.f);
	//	desc.vSpecular = desc.vDiffuse;
	//	desc.vPosition = Vec4(21.f, 14.5f, 25.f, 1.f);
	//	desc.fRange = 10.f;

	//	if (FAILED(m_pGameInstance->Add_Light(desc)))
	//		return E_FAIL;
	//}

	//return S_OK;
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
