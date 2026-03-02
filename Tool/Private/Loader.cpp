#include "pch.h"
#include "Loader.h"
#include "UEMapDataLoader.h"
//=================
// Component
//=================
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Particle_Mesh.h"
#include "VIBuffer_Particle_Point.h"
#include "Model.h"
#include "Collider.h"
#include "Shader.h"
#include "Bounds.h"
#include "MonoBehaviour.h"
#include "Camera.h"
#include "VIBuffer_Line_Color.h"
#include "Transform.h"
//=================
// Object
//=================
#include "Tool_ContainerObject.h"
#include "Tool_PartObject.h"
#include "AnimObj.h"
#include "Effect.h"
#include "CEffectObject.h"
#include "Gravity_Force.h"
#include "Tool_Weapon.h"
//=================
// UI
//=================
#include "ToolCanvas.h"
#include "ToolUI.h"
//=================
// Map
//=================
#include "MapObject.h"
#include "LevelData.h"
//=================
// Resource
//=================
#include "Texture.h"
#include "MaterialInstance.h"
#include "Material.h"
#include "GameInstance.h"



CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingELevelType)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_eLoadingELevelType(eLoadingELevelType)
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

	switch (m_eLoadingELevelType)
	{
	case Tool::ELevelType::LOGO:
		hr = Loading_For_Logo();
		break;
	case Tool::ELevelType::MAP:
		hr = Loading_For_Map();
		break;
	case Tool::ELevelType::ANIMATION:
		hr = Loading_For_Animation();
		break;
	case Tool::ELevelType::EFFECT:
		hr = Loading_For_Effect();
		break;
	case Tool::ELevelType::CAMERA:
		hr = Loading_For_Camera();
		break;
	case Tool::ELevelType::UI:
		hr = Loading_For_UI();
		break;
	case Tool::ELevelType::ATTACK_PRESET:
		hr = Loading_For_AttackPreset();
		break;
	case Tool::ELevelType::ASSET_CONVERT:
		hr = Loading_For_AssetConverter();
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
	m_isFinished = true;


	return S_OK;
}

HRESULT CLoader::Loading_For_Map()
{
	Matrix matPreTransformScale100 = Matrix::CreateScale(0.01f, 0.01f, 0.01f);





	/* Model Prototype */
	CUEMapDataLoader* pMapDataLoader = CUEMapDataLoader::Create(m_pDevice,m_pDeviceContext);
	if (pMapDataLoader == nullptr) return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"../../Resources/Models/Map/")))
	{
		Safe_Release(pMapDataLoader);
		return E_FAIL;
	}
	Safe_Release(pMapDataLoader);



	std::filesystem::path mapFolderPath = L"../../Resources/Models/DebugCamera/";
	if (std::filesystem::exists(mapFolderPath))
	{
		/* Model Prototype */
		CUEMapDataLoader* pMapDataLoader = CUEMapDataLoader::Create(m_pDevice, m_pDeviceContext);
		if (pMapDataLoader == nullptr) return E_FAIL;
		if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"../../Resources/Models/DebugCamera/")))
		{
			Safe_Release(pMapDataLoader);
			return E_FAIL;
		}
		Safe_Release(pMapDataLoader);
	}


	//=================
	// CGameObject
	//=================
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_GameObject_MapObject", CMapObject::Create(EToolObjectType::MAPOBJECT,m_pDevice, m_pDeviceContext));



	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Animation()
{
	/* model load */
	Matrix matPreTransformScale = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	// For. Prototype_Component_Model
	{
		//CModel::MODEL_ORIGIN_DESC desc = {};
		//desc.eType = EModelType::ANIM;
		//desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::ANIMATION);
		//desc.pMatPreTransform = &matPreTransformScale;
		//desc.wstrModelFolderName = L"PlayerMoon";
		//m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_Component_Model_PlayerMoon", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	{
		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_AnimObject", CAnimObj::Create(EToolObjectType::ANIMATION, m_pDevice, m_pDeviceContext));
	}

	// For. Prototype_Component_Collider_OBB
	CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC	ExploDesc{};
	ExploDesc.iInstnaceCount = 30;
	ExploDesc.vCenter = Vec3(0.f, 0.f, 0.f);
	ExploDesc.vSize = Vec2(0.05f, 0.15f);
	ExploDesc.vRange = Vec3(0.5f, 0.5f, 0.5f);
	ExploDesc.vSpeed = Vec2(2.f, 5.f);
	ExploDesc.vLifeTime = Vec2(1.f, 5.5f);
	ExploDesc.isLoop = false;
	ExploDesc.vPivot = Vec3(0.f, 0.f, 0.5f);

	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_Component_VIBuffer_Particle_Point", CVIBuffer_Particle_Point::Create(m_pDevice, m_pDeviceContext, &ExploDesc));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_Component_VIBuffer_Particle_Mesh", CVIBuffer_Particle_Mesh::Create(m_pDevice, m_pDeviceContext, &ExploDesc));

	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_Effect", Effect::Create(EToolObjectType::MESHEFFECT, m_pDevice, m_pDeviceContext));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_Effect_Part_Particle", CEffectObject::Create(EToolObjectType::MESHEFFECT, m_pDevice, m_pDeviceContext));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_Effect_Part_ForceField", CGravity_Force::Create(EToolObjectType::MESHEFFECT, m_pDevice, m_pDeviceContext));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"Prototype_GameObject_Tool_Weapon", CTool_Weapon::Create(EToolObjectType::ANIMATION, m_pDevice, m_pDeviceContext));
	/* Effect Data Model */
	wstring basicBoxPath = L"../../Resources/Models/Map/Level/BasicShapes/Model/";

	CUEMapDataLoader* pMapDataLoader = CUEMapDataLoader::Create(m_pDevice, m_pDeviceContext);
	if (pMapDataLoader == nullptr) return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/blade/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Circle/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Cone/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Object/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Object_Chain/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Object_Female_Character/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Rock/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Tornado/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Lightning/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Twist/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Plane/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), L"../../Resources/Models/Effect_FBX/Claw/Model/")))
		return E_FAIL;

	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::ANIMATION), basicBoxPath)))
		return E_FAIL;
	Safe_Release(pMapDataLoader);

	Loading_Textures_Effect(L"../../Resources/Textures/Effect");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Effect()
{
	// For. Prototype_Component_Collider_OBB
	CVIBuffer_Particle_Point::PARTICLE_POINT_ORIGIN_DESC	ExploDesc{};
	ExploDesc.iInstnaceCount = 30;
	ExploDesc.vCenter = Vec3(0.f, 0.f, 0.f);
	ExploDesc.vSize = Vec2(0.05f, 0.15f);
	ExploDesc.vRange = Vec3(0.5f, 0.5f, 0.5f);
	ExploDesc.vSpeed = Vec2(2.f, 5.f);
	ExploDesc.vLifeTime = Vec2(1.f, 5.5f);
	ExploDesc.isLoop = false;
	ExploDesc.vPivot = Vec3(0.f, 0.f, 0.5f);

	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Point", CVIBuffer_Particle_Point::Create(m_pDevice, m_pDeviceContext, &ExploDesc));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_Component_VIBuffer_Particle_Mesh", CVIBuffer_Particle_Mesh::Create(m_pDevice, m_pDeviceContext, &ExploDesc));
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"Prototype_GameObject_MapObject", CMapObject::Create(EToolObjectType::MAPOBJECT, m_pDevice, m_pDeviceContext));

	//====================
	// Resource Material
	//====================
	// For. Material_Default
	{
		CMaterial::tagMaterialDesc desc = {};
		desc.wstrName = L"Material_Default";
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterial::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// For. MaterialInstance_Default
	{
		CMaterialInstance::tagMaterialInstanceOrignDesc desc = {};
		desc.vTintColor = Vec4{ 1.f, 1.f, 1.f, 1.f };
		desc.wstrMaterialTag = L"Material_Default";
		desc.wstrName = L"MaterialInstance_Default";
		desc.fEmissivePower = 1.f;
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterialInstance::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// For. MaterialInstance_Default_Red
	{
		CMaterialInstance::tagMaterialInstanceOrignDesc desc = {};
		desc.vTintColor = Vec4{ 1.f, 0.3f, 0.3f, 1.f };
		desc.wstrMaterialTag = L"Material_Default";
		desc.wstrName = L"MaterialInstance_Default_Red";
		desc.fEmissivePower = 1.f;
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterialInstance::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// For. MaterialInstance_Default_Blue
	{
		CMaterialInstance::tagMaterialInstanceOrignDesc desc = {};
		desc.vTintColor = Vec4{ 0.3f, 0.3f, 1.f, 1.f };
		desc.wstrMaterialTag = L"Material_Default";
		desc.wstrName = L"MaterialInstance_Default_Blue";
		desc.fEmissivePower = 1.f;
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterialInstance::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// For. MaterialInstance_Default_Green
	{
		CMaterialInstance::tagMaterialInstanceOrignDesc desc = {};
		desc.vTintColor = Vec4{ 0.3f, 1.f, 0.3f, 1.f };
		desc.wstrMaterialTag = L"Material_Default";
		desc.wstrName = L"MaterialInstance_Default_Green";
		desc.fEmissivePower = 1.f;
		if (FAILED(m_pGameInstance->Add_Resource(desc.wstrName, CMaterialInstance::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}

	/* Effect Data Model */
	wstring basicBoxPath = L"../../Resources/Models/Map/Level/BasicShapes/Model/";

	CUEMapDataLoader* pMapDataLoader = CUEMapDataLoader::Create(m_pDevice, m_pDeviceContext);
	if (pMapDataLoader == nullptr) return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/blade/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Circle/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Cone/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Object/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Object_Chain/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Object_Female_Character/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Rock/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Lightning/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Tornado/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Plane/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Twist/Model/")))
		return E_FAIL;
	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), L"../../Resources/Models/Effect_FBX/Claw/Model/")))
		return E_FAIL;

	if (FAILED(pMapDataLoader->Make_Prototype(ENUM_TO_UINT(ELevelType::EFFECT), basicBoxPath)))
		return E_FAIL;
	Safe_Release(pMapDataLoader);
	
	Loading_Textures_Effect(L"../../Resources/Textures/Effect");

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Camera()
{
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_UI()
{

	//=================
	// Resource Component
	//=================

	std::filesystem::path root = L"../../Resources/Textures/UI/UI_Tool/";
	std::error_code ec;
	for (auto it = std::filesystem::recursive_directory_iterator(
		root,
		std::filesystem::directory_options::skip_permission_denied,
		ec);
		it != std::filesystem::recursive_directory_iterator();
		it.increment(ec))
	{
		if (ec)
			return E_FAIL;

		std::error_code ecDir;
		if (!it->is_directory(ecDir))
		{
			if (ecDir)
				return E_FAIL;
			continue;
		}

		const std::wstring wstrSubFolder = it->path().wstring();

		if (FAILED(Loading_Textures_UI(wstrSubFolder)))
			return E_FAIL;
	}

	//=================
	// UI Objects
	//=================

	// For. Prototype_UI_Canvas
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::UI), g_wszPrototypeTagCanvas, CToolCanvas::Create(EToolObjectType::UI, m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}
	// For. Prototype_UI_UI
	{
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::UI), g_wszPrototypeTagUI, CToolUI::Create(EToolObjectType::UI, m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_AttackPreset()
{
	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_AssetConverter()
{
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
	/* 바탕화면 경로(C:\Users\...\Desktop) 쪽은 특히 desktop.ini가 흔합니다. */
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		wstring wstrFileName = { L"" };
		_wstring ext = { L"" };
		if (entry.is_regular_file())
		{
			ext = entry.path().extension().wstring();
			if (ext == L".ini")
				continue;

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

HRESULT CLoader::Loading_Textures_Effect(const wstring& wstrFolder)
{
	namespace fs = std::filesystem;

	if (fs::exists(wstrFolder) == false)
		return E_FAIL;

	for (const auto& entry : fs::recursive_directory_iterator(wstrFolder))
	{
		if (entry.is_regular_file())
		{
			auto path = entry.path();

			wstring wstrExtension = path.extension().wstring();
			for (auto& c : wstrExtension) c = towlower(c);

			if (wstrExtension == L".hdr")
				continue;

			wstring wstrFileName = path.stem().wstring();

			wstring wstrFolderName = path.parent_path().filename().wstring();
			wstring wstrResourceTag = L"Texture_" + wstrFileName;

			CTextureBase::RESOURCE_BASE_DESC desc = {};
			desc.wstrName = wstrFileName;
			desc.wstrPath = path.wstring();

			if (FAILED(m_pGameInstance->Add_Resource(wstrResourceTag,
				CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
			{
				continue;
			}
		}
	}

	return S_OK;
}

HRESULT CLoader::Loading_Textures_Map(const wstring& wstrFolder)
{
	namespace fs = std::filesystem;

	if (fs::exists(wstrFolder) == false)
		return E_FAIL;

	for (const auto& entry : fs::recursive_directory_iterator(wstrFolder))
	{
		if (entry.is_regular_file())
		{
			auto path = entry.path();

			wstring wstrExtension = path.extension().wstring();
			for (auto& c : wstrExtension) c = towlower(c);

			if (wstrExtension == L".hdr")
				continue;

			wstring wstrFileName = path.stem().wstring();
			wstring wstrFolderName = path.parent_path().filename().wstring();
			wstring wstrResourceTag = L"Texture_" + wstrFileName;

			CTextureBase::RESOURCE_BASE_DESC desc = {};
			desc.wstrName = wstrFileName;
			desc.wstrPath = path.wstring();

			if (FAILED(m_pGameInstance->Add_Resource(wstrResourceTag,
				CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
			{
				continue;
			}
		}
	}

	return S_OK;
}

HRESULT CLoader::Loading_Textures_UI(const wstring& wstrFolder)
{
	if (std::filesystem::exists(wstrFolder) == false)
		return E_FAIL;

	size_t iFileCount = { 0 };
	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		if (entry.is_regular_file())
		{
			_wstring ext = entry.path().extension().wstring();
			for (auto& ch : ext) ch = (wchar_t)towlower(ch);

			if (ext == L".png" || ext == L".dds")
				++iFileCount;
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator(wstrFolder))
	{
		wstring wstrFileName = { L"" };
		_wstring ext = { L"" };

		if (entry.is_regular_file())
		{
			ext = entry.path().extension().wstring();
			for (auto& ch : ext) ch = (wchar_t)towlower(ch);

			if (ext != L".png" && ext != L".dds")
				continue;

			wstrFileName = entry.path().filename().lexically_normal().stem();
			CTextureBase::RESOURCE_BASE_DESC desc = {};
			desc.wstrName = wstrFileName;
			desc.wstrPath = entry.path();

			if (FAILED(m_pGameInstance->Add_Resource(L"Texture_" + wstrFileName,
				CTextureBase::Create(m_pDevice, m_pDeviceContext, &desc))))
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

CLoader* CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, ELevelType eLoadingELevelType)
{
	CLoader* pInstance = new CLoader(pDevice, pDeviceContext, eLoadingELevelType);
	
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
