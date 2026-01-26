#include "pch.h"
#include "Loader.h"
#include "UEMapDataLoader.h"
//=================
// Component
//=================
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Particle_Point.h"
#include "Model.h"
#include "Collider.h"
#include "Shader.h"
#include "MonoBehaviour.h"
#include "Camera.h"
#include "Transform.h"
//=================
// Object
//=================
#include "StaticModel.h"

//=================
// UI
//=================
#include "ToolUI.h"
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

HRESULT CLoader::Loading_For_Map()
{
	Matrix matPreTransformScale100 = Matrix::CreateScale(0.01f, 0.01f, 0.01f);

	//=================
	// Componment
	//=================
	// For. Prototype_Component_Collider_Sphere
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Collider_Sphere", CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::SPHERE));
	// For. Prototype_Component_Collider_AABB
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Collider_AABB", CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::AABB));
	// For. Prototype_Component_Collider_OBB
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_Component_Collider_OBB", CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::OBB));

	///* Map Data Model */
	//CUEMapDataLoader* pMapDataLoader = CUEMapDataLoader::Create(m_pDevice,m_pDeviceContext);
	//if (pMapDataLoader == nullptr) return E_FAIL;
	//if (FAILED(pMapDataLoader->Make_Prototype(L"../../Resources/Models/Map/DevScene/Model/")))
	//{
	//	Safe_Release(pMapDataLoader);
	//	return E_FAIL;
	//}
	//Safe_Release(pMapDataLoader);


	//=================
	// CGameObject
	//=================
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::MAP), L"Prototype_GameObject_StaticModel", CStaticModel::Create(EToolObjectType::MAPOBJECT, m_pDevice, m_pDeviceContext));


	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Animation()
{
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

	// For. Prototype_Component_Button_Test_Texture
	{
		CTexture::TEXTURE_COMPONENT_ORIGIN_DESC textureDesc = {};
		textureDesc.iTextureCount = 1;
		textureDesc.wstrTexturePath = L"../../Resources/Textures/UI/Button/T_Com_BtnIcon_Custom.png";
		if (FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::UI), L"Prototype_Component_Button_Test_Texture", CTexture::Create(&textureDesc))))
			return E_FAIL;

		if (FAILED(Loading_Textures(L"../../Resources/Textures/UI/Layout")))
			return E_FAIL;
	}

	//=================
	// UI Objects
	//=================

	// For. Prototype_UI_Test_Button
	{
		if(FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::UI), L"Prototype_UI_Test_Button", CToolUI::Create(EToolObjectType::UI, m_pDevice, m_pDeviceContext))))
			return E_FAIL;
	}

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
