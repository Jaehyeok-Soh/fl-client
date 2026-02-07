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
#include "VIBuffer_Particle_Mesh.h"
#include "InstanceMesh.h"
#include "VIBuffer_Cube_Tex.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "PhysicsCollider.h"
//=================
// Builder
//=================
#include "DataDocument_Example.h"
#include "DataDocument_Map.h"
#include "DataDocument_Effect.h"
#include "DataDocument_UI.h"
#include "Builder_Example.h"
#include "Builder_UI.h"
#include "BuilderSystem.h"

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
#include "Physics_Terrain.h" // physics test
#include "Effect.h"
#include "EffectObject.h"
#include "Physics_LandScape.h" // physics test
#include "StaticModel.h"
#include "InstanceModel.h"
//=================
// UI
//=================
#include "Canvas.h"
#include "GenericUI.h"
#include "UIPlayer_HP.h"
//=================
// Resource
//=================
#include "TextureBase.h"
#include "Model.h"
#include "ModelLoader.h"
#include "GameInstance.h"

#pragma region Macro
#define ADD_PROTOTYPE(eLevelType, wstrPrototypeTag, pBase) if(FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(eLevelType), wstrPrototypeTag, pBase))) return E_FAIL
#pragma endregion



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
#pragma region PretransformMatrix
	Matrix matPreTransformScale = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	Matrix matPreTransformIdentity = Matrix::Identity;
	Matrix matPreTransformTurn90 = matPreTransformScale * Matrix::CreateFromYawPitchRoll(XMConvertToRadians(90.f), 0.f, 0.f);
#pragma endregion

	/////////////////////////////////////////
	//////////// Ready ToolData ////////////
	/////////////////////////////////////////
#pragma region ToolData
	{
		// Regist Document
		{
			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Map>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::MAP)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_Effect>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::EFFECT)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::MAP)))
				return E_FAIL;
		}


		// Read Json
		{
			if (FAILED(Loading_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::EFFECT, L"../../Resources/Data/EffectData/Attack_1.json")))
				return E_FAIL;
			// For. Example
			// if (FAILED(Loading_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::MAP, L"asdf")))
			// 	return E_FAIL;
		}
	}
#pragma endregion

	/////////////////////////////////////////
	//////////// Ready Resources ////////////
	/////////////////////////////////////////

#pragma region Resource
	{
		//if (FAILED(m_pGameInstance->Load_Sounds(L"../../Resources/Sounds")))
		//	return E_FAIL;

		//if (FAILED(Make_StaticModel_Prototype(ELevelType::LOGO, L"../../Resources/Models/Map/TestMap")))
		//	return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Load_Sounds(L"../../Resources/Sounds")))
		return E_FAIL;

		// For. Prototype_Component_Button_Test_Texture
	{
		if (FAILED(Loading_Textures(L"../../Resources/Textures/UI/Playable/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/UI/Menu/")))
			return E_FAIL;
	}	
	
#pragma endregion

	//////////////////////////////////////////
	//////////// Ready Components ////////////
	//////////////////////////////////////////
#pragma region Component
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
		desc.pMatPreTransform = &(matPreTransformScale);	// matPreTransformScale // matPreTransformTurn90
		desc.wstrModelFolderName = L"PlayerMoon";					// PlayerMoon // Pino

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

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



	// For. Prototype_Component_Collider_SPHERE
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", CInstanceMesh::Create(m_pDevice, m_pDeviceContext));


	///////////////////////////////////////
	//////////// Ready Objects ////////////
	///////////////////////////////////////
#pragma region Objects
	{
		// For. Prototype_GameObject_MainPlayer
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_MainPlayer", CMainPlayer::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_CameraManTargeter
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_CameraManTargeter", CCameraMan_Targeter::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Part_Body
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Body", CBody::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Part_Collider
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Collider", CColliderPart::Create(m_pDevice, m_pDeviceContext));

		// 이펙트 Object
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Effect", Effect::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Effect_Parts", CEffectObject::Create(m_pDevice, m_pDeviceContext));

		/* Map Object */
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_StaticModel", CStaticModel::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_InstanceModel", CInstanceModel::Create(m_pDevice, m_pDeviceContext));

	}
#pragma endregion

#pragma region BUFFER
	{
		CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC	ExploDesc{};
		ExploDesc.iInstnaceCount = 30;
		ExploDesc.vCenter = Vec3(0.f, 0.f, 0.f);
		ExploDesc.vSize = Vec2(0.05f, 0.15f);
		ExploDesc.vRange = Vec3(0.5f, 0.5f, 0.5f);
		ExploDesc.vSpeed = Vec2(2.f, 5.f);
		ExploDesc.vLifeTime = Vec2(1.f, 5.5f);
		ExploDesc.isLoop = false;
		ExploDesc.vPivot = Vec3(0.f, 0.f, 0.5f);

		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_Component_VIBuffer_Particle_Point", CVIBuffer_Particle_Point::Create(m_pDevice, m_pDeviceContext, &ExploDesc));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_Component_VIBuffer_Particle_Mesh", CVIBuffer_Particle_Mesh::Create(m_pDevice, m_pDeviceContext, &ExploDesc));


		// For. Prototype_UI_Canvas
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_Canvas", CCanvas::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_PLAYER_HP", CUIPlayer_HP::Create(m_pDevice, m_pDeviceContext));
	}
#pragma endregion


#pragma region PHYSICS
	// For. Prototype_GameObject_Physics_Terrain
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_Physics_Terrain", CPhysics_Terrain::Create(m_pDevice, m_pDeviceContext));

	/* Map Parsing Test */
#pragma endregion

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_Files(_uint iLevelID, DTO::ECategory eCategory, const wstring& wstrFolderPath)
{
	return m_pGameInstance->Load_Folder_Json(iLevelID, eCategory, wstrFolderPath);
}

HRESULT CLoader::Loading_File(_uint iLevelID, DTO::ECategory eCategory, const wstring& wstrFilePath)
{
	return m_pGameInstance->Load_File_Json(iLevelID, eCategory, wstrFilePath);
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
			if (FAILED(m_pGameInstance->Add_Resource(L"Texture_" + wstrFileName, CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
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

HRESULT CLoader::Make_StaticModel_Prototype(ELevelType eLevelType, const wstring& wstrFilePath)
{
	std::filesystem::path filePath{ wstrFilePath };
	filePath /= "Model";
	const wstring wstrModelTag = L"Prototype_Component_Model_";
	const std::filesystem::path basePath = g_wszModelRelativePath;
	const _uint iPrototypeLevelType = ENUM_TO_UINT(eLevelType);
	for (const auto& entry : std::filesystem::directory_iterator(filePath))
	{
		if (entry.is_regular_file())
		{
			if (entry.path().extension() != g_wszMeshExtension)
				continue;

			std::filesystem::path fileFullPath = entry.path();
			wstring wstrFileName = fileFullPath.stem();
			{
				CBase* pFinded = { nullptr };
				if (pFinded = m_pGameInstance->Find_Prototype(iPrototypeLevelType, wstrModelTag + wstrFileName))
					continue;
			}

			CModel::MODEL_ORIGIN_DESC desc = {};
			desc.eType = EModelType::STATIC;
			desc.iPrototypeLevelIndex = iPrototypeLevelType;
			desc.wstrModelFolderName = fileFullPath.lexically_relative(basePath);
			m_pGameInstance->Add_Prototype(iPrototypeLevelType, wstrModelTag + wstrFileName, CModel::Create(m_pDevice, m_pDeviceContext, &desc));
		}
	}

	return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingLevelID)
{
	CLoader* pInstance = new CLoader(pDevice, pDeviceContext, eLoadingLevelID);

	if (FAILED(pInstance->Initailize()))
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
