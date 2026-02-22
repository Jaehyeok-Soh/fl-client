#include "pch.h"
#include "Builder_Map.h"
#include "GameInstance.h"
#include "StaticObject.h"
#include "LandScape.h"
#include "Bounds.h"
#include "DataDocument_Map.h"
#include "Shader.h"

CBuilder_Map::CBuilder_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	: CBuilderBase(pDevice, pDeviceContext, iLevelID), m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT	CBuilder_Map::Initialize()
{
	/* Scene Data를 적용시켜줄 Shader 미리 Binding  */
	m_pMeshShader =
		static_cast<CShader*> (m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_Component_Shader_VtxMesh", nullptr));
	if (!m_pMeshShader) return E_FAIL;

	return S_OK;
}


HRESULT CBuilder_Map::Build(const CDataDocumentBase& document)
{
	const auto& doc = static_cast<const CDataDocument_Map&>(document);
	// For Scene Data 
	{
		const vector<Engine::IObjectDataBase*> vecSceneData = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::SCENEDATA));
		for (const auto& pObjectData : vecSceneData)
		{
			const auto* pSceneData = static_cast<const Engine::CData_SceneData*>(pObjectData);

			DTO::TSceneData tData = pSceneData->Get_Data();

			/* Scene Data */
			SceneData_Setting(tData);
		}
	}

	// For MapObject 
	{
		const vector<Engine::IObjectDataBase*> vecList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::MAPOBJECT));
		for (const auto& pObjectData : vecList)
		{
			const auto* pStaticObjectData = static_cast<const Engine::CData_MapObject*>(pObjectData);
	
			DTO::TMap_MapObjectData tData = pStaticObjectData->Get_Data();

			DTO::EClientMakePath eClientMakePath = static_cast<DTO::EClientMakePath>(tData.eClientMakePath);

			switch (eClientMakePath)
			{
			case DTO::EClientMakePath::StaticObject:	Create_StaticObject(tData); break;
			case DTO::EClientMakePath::LandScape:		Create_LandScape(tData); break;
			default:									return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CBuilder_Map::SceneData_Setting(const DTO::TSceneData& tData)
{
	/*  None이라면 돌아가기  */
	if (tData.strTextureSplatingInfoName != "None")
	{
		if (FAILED(m_pGameInstance->GameDataManager_Bind_SplatingTextureInfo(m_pMeshShader, Engine_Utils::ToWString(tData.strTextureSplatingInfoName))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CBuilder_Map::Create_StaticObject(const DTO::TMap_MapObjectData& tData)
{
	CStaticObject::STATICOBJECT_DESC tStaticObjectDesc{};

	tStaticObjectDesc.iLevelIndex		 = tData.eClientLevelType;
	tStaticObjectDesc.isUELoaded		 = tData.isUELoaded;
	tStaticObjectDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tStaticObjectDesc.wstrModelPath		 = Engine_Utils::ToWString(tData.strModelPath);
	tStaticObjectDesc.iSectionNum		= tData.iSectionNum;

	/* 여기서 Desc가 따로 필요한 객체라면 알잘딱 static_cast로 부여받기 */


	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tStaticObjectDesc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject( ENUM_TO_UINT(ELevelType::STATIC),L"Prototype_GameObject_StaticObject", tStaticObjectDesc.iLevelIndex , g_wszStaticObjectLayer,&tStaticObjectDesc);

	return S_OK;
}

HRESULT CBuilder_Map::Create_LandScape(const DTO::TMap_MapObjectData& tData)
{
	CLandScape::LANDSCAPE_DESC tLandSapceDesc{};

	tLandSapceDesc.iLevelIndex			= tData.eClientLevelType;
	tLandSapceDesc.isUELoaded			= tData.isUELoaded;
	tLandSapceDesc.eMapObjectDrawType	= static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tLandSapceDesc.wstrModelPath		= Engine_Utils::ToWString(tData.strModelPath);
	tLandSapceDesc.iSectionNum			= tData.iSectionNum;



	/* Land Scape 는 Instance Draw Type 불가능  */
	if (tLandSapceDesc.eMapObjectDrawType == EMapObject_DrawType::Instance)
		return E_FAIL;

	/* LandScape는 단일객체만 있을예정 */
	for (auto& Desc : tData.vecClientMakePathDesc)
	{
		LANDSCAPE_DESC* pDesc = static_cast<LANDSCAPE_DESC*>(Desc);
		tLandSapceDesc.iIndex = pDesc->iIndex;
		tLandSapceDesc.vTextureUV_LT = pDesc->vTextureUV_LT;
		tLandSapceDesc.vTextureUV_RB = pDesc->vTextureUV_RB;
	}

	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tLandSapceDesc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_GameObject_LandScape", 
		tLandSapceDesc.iLevelIndex, g_wszStaticObjectLayer, &tLandSapceDesc);


	return S_OK;
}


CBuilder_Map* CBuilder_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_Map* pBuilder = new CBuilder_Map(pDevice, pDeviceContext, iLevelID);

	if (FAILED(pBuilder->Initialize()))
	{
		Safe_Release(pBuilder);
		MSG_BOX(" Builder Map is Failed To Create ");
		return nullptr;
	}

	return pBuilder;
}

void CBuilder_Map::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pMeshShader);


	Super::Free();
}