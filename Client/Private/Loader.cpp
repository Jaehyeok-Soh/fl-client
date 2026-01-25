#include "pch.h"
#include "Engine_Utils.h"
//=================
// Component
//=================
#include "PlayerControlContext.h"
#include "MonoBehaviour.h"
#include "PlayerActionState.h"
#include "StatComponent.h"
#include "Collider.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Cube_Tex.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "Model.h"
//=================
// Object
//=================
#include "Texture.h"
#include "MainPlayer.h"
#include "CameraMan_Targeter.h"
#include "Body.h"
#include "Weapon.h"
#include "ColliderPart.h"
#include "Loader.h"
//=================
// UI
//=================
#include "GenericUI.h"

//=================
// Resource
//=================
#include "TextureBase.h"
#include "Model.h"
#include "GameInstance.h"



CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingLevelID)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_eLoadingLevelID(eLoadingLevelID)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

HRESULT CLoader::Initailize()
{
	try
	{
		m_LoadingThread = std::thread(
			[this]()->void
			{
				Loading();
			});
	}
	catch (std::exception& e)
	{
		std::string src{ e.what() };
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoader::Loading()
{
	::CoInitializeEx(nullptr, 0);

	HRESULT hr = {};

	switch (m_eLoadingLevelID)
	{
	case Client::ELevelType::LOGO:
		hr = Loading_For_Logo();
		break;
	default:
		hr = E_FAIL;
		break;
	}

	::CoUninitialize();

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_For_Logo()
{
	Matrix matPreTransformScale = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	Matrix matPreTransformIdentity = Matrix::Identity;

	if (FAILED(m_pGameInstance->Load_Sounds(L"../../Resources/Sounds")))
		return E_FAIL;

	{
		std::lock_guard<std::mutex> lockguard(m_mutex_1);
		lstrcpy(m_szFPS, TEXT("텍스쳐를 로딩 중 입니다."));
	}

	// For. Prototype_Component_GenericUI_Texture
	{
		CTexture::TEXTURE_COMPONENT_ORIGIN_DESC textureDesc = {};
		textureDesc.iTextureCount = 1;
		textureDesc.wstrTexturePath = L"../../Resources/Textures/UI/Button/T_Com_BtnIcon_Custom.png";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::LOGO), L"Prototype_Component_GenericUI_Texture", CTexture::Create(&textureDesc))))
			return E_FAIL;
	}

	{
		std::lock_guard<std::mutex> lockguard(m_mutex_1);
		lstrcpy(m_szFPS, TEXT("모델을(를) 로딩 중 입니다."));
	}


	{
		std::lock_guard<std::mutex> lockguard(m_mutex_1);
		lstrcpy(m_szFPS, TEXT("ㅅㅖ이더을(를) 로딩 중 입니다."));
	}


	{
		std::lock_guard<std::mutex> lockguard(m_mutex_1);
		lstrcpy(m_szFPS, TEXT("객체원형을(를) 로딩 중 입니다."));
	}
	//=================
	// Component
	//=================
	// For. Prototype_Component_Stat
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Stat", CStatComponent::Create());
	// For. Prototype_Component_Model_Master
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &matPreTransformScale;
		desc.wstrModelFolderName = L"PlayerMoon";
		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_Master", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}
	// For. Prototype_Component_Camera
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Camera", CCamera::Create());
	// For. Prototype_Component_ActionState_Player
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ActionState_Player", CPlayerActionState::Create());
	// For. Prototype_Component_ControlContext_Player
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ControlContext_Player", CPlayerControlContext::Create());
	// For. Prototype_Component_Collider_AABB
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_AABB", CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::AABB));
	// For. Prototype_Component_Collider_OBB
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_OBB", CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::OBB));
	// For. Prototype_Component_Collider_SPHERE
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Collider_Sphere", CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::SPHERE));

	//=================
	// Object
	//=================
	// For. Prototype_GameObject_MainPlayer
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_MainPlayer", CMainPlayer::Create(m_pDevice, m_pDeviceContext));
	// For. Prototype_GameObject_CameraManTargeter
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_CameraManTargeter", CCameraMan_Targeter::Create(m_pDevice, m_pDeviceContext));
	// For. Prototype_GameObject_Part_Body
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Body", CBody::Create(m_pDevice, m_pDeviceContext));
	// For. Prototype_GameObject_Part_Collider
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Part_Collider", CColliderPart::Create(m_pDevice, m_pDeviceContext));

	//=================
	// UI
	//=================
	// For. Prototype_UI_GenericUI
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::LOGO), L"Prototype_UI_GenericUI", CGenericUI::Create(m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}

	{
		std::lock_guard<std::mutex> lockguard(m_mutex_1);
		lstrcpy(m_szFPS, TEXT("로딩이 완료되었슴니다."));
	}

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Textures(const wstring& wstrFolder)
{
	if (std::filesystem::exists(wstrFolder) == false)
		return E_FAIL;

	size_t iFileCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		if (entry.is_regular_file())
		{
			++iFileCount;
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		wstring wstrFileName = { L"" };
		if (entry.is_regular_file())
		{
			wstrFileName = entry.path().filename().lexically_normal().stem();
			CTextureBase::RESOURCE_BASE_DESC desc = {};
			desc.wstrName = wstrFileName;
			desc.wstrPath = entry.path();
			if(FAILED(m_pGameInstance->Add_Resource(L"Texture_" + wstrFileName, CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Loading_Texture(const wstring& wstrFile)
{
	if (wstrFile.empty())
		return E_FAIL;

	std::filesystem::path filePath{ wstrFile };
	wstring wstrFileName = filePath.filename().lexically_normal().stem();

	CTextureBase::RESOURCE_BASE_DESC desc = {};
	desc.wstrName = wstrFileName;
	desc.wstrPath = filePath.wstring();
	if (FAILED(m_pGameInstance->Add_Resource(L"Texture_" + wstrFileName, CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
		return E_FAIL;

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pDeviceContext, eLoadingLevelID);
	
	if(FAILED(pInstance->Initailize()))
	{
		MSG_BOX("CLoader::Create, Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Super::Free();
}
