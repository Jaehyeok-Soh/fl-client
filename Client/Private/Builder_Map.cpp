#include "pch.h"
#include "Builder_Map.h"
#include "StaticObject.h"
#include "LandScape.h"
#include "Bounds.h"
#include "DataDocument_Map.h"
#include "Bush.h"
#include "Grass.h"
#include "Moss.h"
#include "Tree.h"
#include "Vine.h"
#include "Rock.h"
#include "Water.h"
#include "LightObject.h"
#include "Shader.h"
#include "Fog.h"
#include "EnvObject.h"
#include "SkyBox.h"
#pragma region Batch 관련
/* Batch Player */
#include "MainPlayer.h"

/* Batch Monster */
#include "Monster_Body_Base.h"

/* Batch Object */
#include "BattleField.h"
#include "PointLight.h"

/* Batch NPC */
#include "NPC_Base.h"

/* Batch Interactive Object */
#include "InteractiveObject.h"

#pragma endregion

#pragma region Trigger Box
#include "TriggerBox_LevelChange.h"
#include "TriggerBox_MonsterSpawner.h"
#include "TriggerBox_MonsterWaveSpawner.h"
#include "TriggerBox_GlobalEvent_BroadCaster.h"
#include "TriggerBox_TutorialUIEvent.h"
#include "TriggerBox_CinematicPlayer.h"
#pragma endregion


#pragma region 기능

#include "InvisibleWall.h"

#pragma endregion


#include "PhysicsCCT.h"
#include "GameInstance.h"


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

	/* Scene Data를 적용시켜줄 Shader 미리 Binding  */
	m_pInstMeshShader =
		static_cast<CShader*> (m_pGameInstance->Clone_Prototype(EPrototypeType::COMPONENT, ENUM_TO_UINT(ELevelType::STATIC),
			L"Prototype_Component_Shader_VtxInstanceMesh", nullptr));
	if (!m_pInstMeshShader) return E_FAIL;

	return S_OK;
}


HRESULT CBuilder_Map::Build(const CDataDocumentBase& document)
{
	m_eLevelType = ELevelType::STATIC;


	const auto& doc = static_cast<const CDataDocument_Map&>(document);
	// For Scene Data 
	{
		const vector<Engine::IObjectDataBase*> vecSceneData = doc.Get_ListByType(ENUM_TO_UINT(DTO::EMapObject_Type::SCENEDATA));
		for (const auto& pObjectData : vecSceneData)
		{
			const auto* pSceneData = static_cast<const Engine::CData_LevelData*>(pObjectData);

			DTO::TLevelData tData = pSceneData->Get_Data();

			/* Scene Data */
			LevelData_Setting(tData);
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
			case DTO::EClientMakePath::StaticObject:						Create_StaticObject(tData);							break;
			case DTO::EClientMakePath::LandScape:							Create_LandScape(tData);							break;
			case DTO::EClientMakePath::Bush:								Create_Bush(tData);									break;
			case DTO::EClientMakePath::Tree:								Create_Tree(tData);									break;
			case DTO::EClientMakePath::Grass:								Create_Grass(tData);								break;
			case DTO::EClientMakePath::Moss:								Create_Moss(tData);									break;
			case DTO::EClientMakePath::Rock:								Create_Rock(tData);									break;
			case DTO::EClientMakePath::Vine:								Create_Vine(tData);									break;
			case DTO::EClientMakePath::Water:								Create_Water(tData);								break;
			case DTO::EClientMakePath::Env:									Create_Env(tData);									break;
			case DTO::EClientMakePath::LightObject:							Create_LightObject(tData);							break;

			case DTO::EClientMakePath::Batch_Player:						Batch_Player(tData);								break;
			case DTO::EClientMakePath::Batch_Monster:						Batch_Monster(tData);								break;
			case DTO::EClientMakePath::Batch_Object:						Batch_Object(tData);								break;
			case DTO::EClientMakePath::Batch_InteractiveObject:				Batch_InteractiveObject(tData);						break;

			case DTO::EClientMakePath::TriggerBox_ChangeLevel:				Create_TriggerBox_ChangeLevel(tData);				break;
			case DTO::EClientMakePath::TriggerBox_MonsterSpawner:			Create_TriggerBox_MonsterSpawner(tData);			break;
			case DTO::EClientMakePath::TriggerBox_MonsterWaveSpawner:		Create_TriggerBox_MonsterWaveSpawner(tData);		break;
			case DTO::EClientMakePath::TriggerBox_GlobalEvent_BroadCaster:	Create_TriggerBox_GlobalEvent_BroadCaster(tData);	break;
			case DTO::EClientMakePath::TriggerBox_TutorialUIEvent:			Create_TriggerBox_TutorialUIEvent(tData);			break;
			case DTO::EClientMakePath::TriggerBox_CinematicPlayer:			Create_TriggerBox_CinematicPlayer(tData);			break;



			case DTO::EClientMakePath::Invisible_Wall:						Create_InvisibleWall(tData); break;


			case DTO::EClientMakePath::Batch_NPC:							Batch_NPC(tData); break;


			default:									return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CBuilder_Map::LevelData_Setting(const DTO::TLevelData& tData)
{
	m_eLevelType = StringToClientleveltype(tData.strLevelTypeName);

	if (m_pMeshShader == nullptr) return E_FAIL;
	if (m_pInstMeshShader == nullptr) return E_FAIL;


	/*  None이라면 돌아가기  */
	if (tData.strTextureSplatingInfoName != "None")
	{
		m_pGameInstance->GameDataManager_Bind_SplatingTextureInfo(m_pMeshShader, Engine_Utils::ToWString(tData.strTextureSplatingInfoName));
	}

	CB_EnvData tEnvData{};
	
	/* Wind */
	tEnvData.vWindDirection = tData.vWindDirection;
	tEnvData.fWindPower = tData.fWindPower;

	/* SkyBox */

	tEnvData.vEnvColor = tData.vEnvColor;							// 환경 Color세팅
	tEnvData.vSkyColor = tData.vSkyColor;							// SkyColor 세팅

	tEnvData.vCloudBaseColor = tData.vCloudBaseColor;				// Base Color 세팅

	tEnvData.vCloudHighlight = tData.vCloudHighlight;				// Clouds Hightlight 세팅
	tEnvData.fCloudHighlightPower = tData.fCloudHighlightPower;		// Power

	tEnvData.vCloudShadowColor = tData.vCloudShadowColor;			// Cloud Shadow Color 세팅
	tEnvData.fCloudShadowPower = tData.fCloudShadowPower;			// Power

	tEnvData.vSkyBoxTextureUVSpeed = tData.vSkyBoxTextureUVSpeed;	// UV Speed Setting
	tEnvData.isChannelPacking = tData.isSkyBoxChannelPacking;		// Chaneel Packing Setting
	tEnvData.iSkyBoxTextureType = tData.iSkyBoxTextureType;			//TextureType Setting
	tEnvData.fPolarRadiusScale = tData.fPolarRadiusScale;

	/* Env Data 세팅 */
	ID3DX11EffectConstantBuffer* pCB = m_pMeshShader->Get_ConstantBuffer("CB_EnvData");
	if (!pCB->IsValid())	return E_FAIL;
	pCB->SetRawValue(&tEnvData, 0 , sizeof(CB_EnvData));

	pCB = m_pInstMeshShader->Get_ConstantBuffer("CB_EnvData");
	if (!pCB->IsValid())	return E_FAIL;
	pCB->SetRawValue(&tEnvData, 0, sizeof(CB_EnvData));

	/* Map Box */
	m_pGameInstance->Set_MapMinMaxBox(tData.vMapMinMaxBox_Center,tData.vMapMinMaxBox_extents);


	/* SkyBox 생성 체크 */
	if(tData.strSkyBoxModelName != "None" && tData.strSKyBoxTextureName != "None")
	{
		CSkyBox::SKYBOX_DESC tDesc{};
		tDesc.wstrModelTag = Engine_Utils::ToWString(tData.strSkyBoxModelName);
		tDesc.wstrTextureTag = Engine_Utils::ToWString(tData.strSKyBoxTextureName);

		CTransform::TRANSFORM_DESC tTsDesc{};
		tTsDesc.ScaleMatrix = Matrix::CreateScale(tData.vSkyBoxScale);
		tTsDesc.RotationMatrix = Matrix::CreateFromYawPitchRoll(
			XMConvertToRadians(tData.vSkyBoxPitchYawRoll.y), XMConvertToRadians(tData.vSkyBoxPitchYawRoll.x), XMConvertToRadians(tData.vSkyBoxPitchYawRoll.z));
		tTsDesc.TranslationMatrix = Matrix::CreateTranslation(tData.vSkyBoxPositionOffset);

		tDesc.pTransform_Desc = &tTsDesc;
		tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);

		m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), g_wszSkyBox_Prototype_Tag, tDesc.iLevelIndex, g_wszSkyBoxLayer, &tDesc);
	}

	return S_OK;
}

#pragma region Create StaticObject
HRESULT CBuilder_Map::Create_StaticObject(const DTO::TMap_MapObjectData& tData)
{
	CStaticObject::STATICOBJECT_DESC tStaticObjectDesc{};

	tStaticObjectDesc.iLevelIndex		 = ENUM_TO_UINT(m_eLevelType);
	tStaticObjectDesc.isUELoaded		 = tData.isUELoaded;
	tStaticObjectDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tStaticObjectDesc.wstrModelPath		 = Engine_Utils::ToWString(tData.strModelPath);
	tStaticObjectDesc.iSectionNum		 = tData.iSectionNum;
	tStaticObjectDesc.eClientMakePath	 = tData.eClientMakePath;


	/* 여기서 Desc가 따로 필요한 객체라면 알잘딱 static_cast로 부여받기 */


	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tStaticObjectDesc.vecSRT.push_back(SRT_DATA);
	}

	m_pGameInstance->Add_GameObject( ENUM_TO_UINT(ELevelType::STATIC),g_wszStaticObject_Prototype_Tag , tStaticObjectDesc.iLevelIndex , g_wszStaticObjectLayer,&tStaticObjectDesc);

	return S_OK;
}
#pragma endregion

#pragma region Create LandScape
HRESULT CBuilder_Map::Create_LandScape(const DTO::TMap_MapObjectData& tData)
{
	CLandScape::LANDSCAPE_DESC tLandSapceDesc{};

	tLandSapceDesc.iLevelIndex			= ENUM_TO_UINT(m_eLevelType);
	tLandSapceDesc.isUELoaded			= tData.isUELoaded;
	tLandSapceDesc.eMapObjectDrawType	= static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tLandSapceDesc.wstrModelPath		= Engine_Utils::ToWString(tData.strModelPath);
	tLandSapceDesc.iSectionNum			= tData.iSectionNum;
	tLandSapceDesc.eClientMakePath		= tData.eClientMakePath;

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
		ENUM_TO_UINT(ELevelType::STATIC), g_wszLandScape_Prototype_Tag , 
		tLandSapceDesc.iLevelIndex, g_wszStaticObjectLayer, &tLandSapceDesc);


	return S_OK;
}
#pragma endregion

#pragma region Create Bush
HRESULT CBuilder_Map::Create_Bush(const DTO::TMap_MapObjectData& tData)
{
	CBush::BUSH_DESC tDesc{};
	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;
	
	

	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	if (!tData.vecClientMakePathDesc.empty())
	{
		/* Plants  */
		PLANTS_DESC* pFrontDesc = static_cast<PLANTS_DESC*>(tData.vecClientMakePathDesc.front());
		tDesc.vMI_TintColor = pFrontDesc->vMITint_Color;
		tDesc.fDiffuseColorPower = pFrontDesc->fDiffuseColorPower;
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszBush_Prototype_Tag ,
		tDesc.iLevelIndex, g_wszStaticObjectLayer, &tDesc);

	return S_OK;
}
#pragma endregion

#pragma region Create Grass
HRESULT CBuilder_Map::Create_Grass(const DTO::TMap_MapObjectData& tData)
{
	CGrass::GRASS_DESC tDesc{};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;


	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	if (!tData.vecClientMakePathDesc.empty())
	{
		/* Plants  */
		GRASS_DESC* pOrigin = static_cast<GRASS_DESC*>(tData.vecClientMakePathDesc.front());
		tDesc.vMI_TintColor = pOrigin->vMITint_Color;
		tDesc.fGrassSwaySpeed = pOrigin->fGrassSwaySpeed;
		tDesc.fGrassWaveSize = pOrigin->fGrassWaveSize;
		tDesc.fDiffuseColorPower = pOrigin->fDiffuseColorPower;

	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszGrass_Prototype_Tag ,
		tDesc.iLevelIndex, g_wszStaticObjectLayer, &tDesc);

	return S_OK;
}
#pragma endregion

#pragma region Create Moss
HRESULT CBuilder_Map::Create_Moss(const DTO::TMap_MapObjectData& tData)
{

	CGrass::GRASS_DESC tDesc{};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;


	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	if (!tData.vecClientMakePathDesc.empty())
	{
		/* Plants  */
		PLANTS_DESC* pOrigin = static_cast<PLANTS_DESC*>(tData.vecClientMakePathDesc.front());
		tDesc.vMI_TintColor = pOrigin->vMITint_Color;
		tDesc.fDiffuseColorPower = pOrigin->fDiffuseColorPower;
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszMoss_Prototype_Tag ,
		tDesc.iLevelIndex, g_wszStaticObjectLayer, &tDesc);

	return S_OK;

}
#pragma endregion

#pragma region Create Tree
HRESULT CBuilder_Map::Create_Tree(const DTO::TMap_MapObjectData& tData)
{
	CTree::TREE_DESC tDesc{};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;


	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	if (!tData.vecClientMakePathDesc.empty())
	{
		/* Plants  */
		PLANTS_DESC* pOrigin = static_cast<PLANTS_DESC*>(tData.vecClientMakePathDesc.front());
		tDesc.vMI_TintColor = pOrigin->vMITint_Color;
		tDesc.fDiffuseColorPower = pOrigin->fDiffuseColorPower;
	}

	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszTree_Prototype_Tag ,
		tDesc.iLevelIndex, g_wszStaticObjectLayer, &tDesc);

	return S_OK;
}
#pragma endregion

#pragma region Create Vine
HRESULT CBuilder_Map::Create_Vine(const DTO::TMap_MapObjectData& tData)
{
	CVine::VINE_DESC tDesc{};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}


	if (!tData.vecClientMakePathDesc.empty())
	{
		/* Plants  */
		PLANTS_DESC* pOrigin = static_cast<PLANTS_DESC*>(tData.vecClientMakePathDesc.front());
		tDesc.vMI_TintColor = pOrigin->vMITint_Color;
		tDesc.fDiffuseColorPower = pOrigin->fDiffuseColorPower;
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC),g_wszVine_Prototype_Tag ,
		tDesc.iLevelIndex, g_wszStaticObjectLayer, &tDesc);

	return S_OK;
}
#pragma endregion

#pragma region Create Water
HRESULT CBuilder_Map::Create_Water(const DTO::TMap_MapObjectData& tData)
{
	CWater::WATER_DESC tDesc{};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);;
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;
	
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	if (tData.vecClientMakePathDesc.empty())		return E_FAIL;
	/* Desc */
	Engine::WATER_DESC* pOriginDesc = static_cast<Engine::WATER_DESC*>(tData.vecClientMakePathDesc.front());
	if (pOriginDesc == nullptr) return E_FAIL;
	
	/* Flag 담아서 던져주기 */
	tDesc.arrayWaterSRVs.fill(nullptr);
	for (_uint i = 0; i < ENUM_TO_UINT(EWaterTextureType::END) ; ++i)
	{
		if(pOriginDesc->arrayTextureBase[i])
			tDesc.arrayWaterSRVs[i] = pOriginDesc->arrayTextureBase[i]->Get_SRV();
		if (tDesc.arrayWaterSRVs[i] != nullptr)
			Engine_Utils::Add_Flag(tDesc.tCBWaterData.g_WaterTexBindingFlags , 1 << i);
	}

	/* 기본 Normal 관련  */
	tDesc.tCBWaterData.g_vWaterSpeed1				= pOriginDesc->vSpeed1;
	tDesc.tCBWaterData.g_vWaterSpeed2				= pOriginDesc->vSpeed2;
	tDesc.tCBWaterData.g_vWaterUVPower				= pOriginDesc->vWaterUVPower;

	/* Noise Texture 관련 */
	tDesc.tCBWaterData.g_fDistortionPower			= pOriginDesc->fDistortionPower;
	tDesc.tCBWaterData.g_vWaterDistortionSpeed		= pOriginDesc->vDistortionSpeed;
	tDesc.tCBWaterData.g_vWaterDistortionUVPower	= pOriginDesc->vDistortionUVPower;

	/* Lighting Texture 관련 */
	tDesc.tCBWaterData.g_fSparklePower				= pOriginDesc->fSparklePower;
	tDesc.tCBWaterData.g_vSparkleUVPower			= pOriginDesc->vSparkleUVPower;

	tDesc.vMI_TintColor								= pOriginDesc->vMI_TintColor;

	tDesc.isUseRipple = pOriginDesc->isUseRipple;

	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszWater_Prototype_Tag ,
		tDesc.iLevelIndex, g_wszWaterLayer, &tDesc);

	return S_OK;
}
#pragma endregion

#pragma region Create Rock
HRESULT CBuilder_Map::Create_Rock(const DTO::TMap_MapObjectData& tData)
{
	CVine::VINE_DESC tRock_Desc{};

	tRock_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tRock_Desc.isUELoaded = tData.isUELoaded;
	tRock_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tRock_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tRock_Desc.iSectionNum = tData.iSectionNum;
	tRock_Desc.eClientMakePath = tData.eClientMakePath;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tRock_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszRock_Prototype_Tag ,
		tRock_Desc.iLevelIndex, g_wszStaticObjectLayer, &tRock_Desc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_Env(const DTO::TMap_MapObjectData& tData)
{
	CEnvObject::ENVOBJECT_DESC tDesc{};
	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;


	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	if (!tData.vecClientMakePathDesc.empty())
	{
		ENV_DESC* pDesc = static_cast<ENV_DESC*>(tData.vecClientMakePathDesc.front());
		if (pDesc == nullptr) return E_FAIL;
		tDesc.vecEnvEffectInfo = pDesc->vecEnvEffectInfo;
	}


	CGameObject* pGameObject{ nullptr };
	pGameObject = m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszEnvObject_Prototype_Tag,
		tDesc.iLevelIndex, g_wszStaticObjectLayer , &tDesc);

	if (pGameObject == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CBuilder_Map::Create_LightObject(const DTO::TMap_MapObjectData& tData)
{
	CLightObject::LIGHTOBJECT_DESC tDesc{};
	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	LIGHTOBJECT_DESC* pOrignDesc = static_cast<Engine::LIGHTOBJECT_DESC*>(tData.vecClientMakePathDesc.front());

	tDesc.tLightDesc = pOrignDesc->tLightDesc;
	tDesc.tLightDesc.fRange = pOrignDesc->fBaseRange;
	tDesc.isFlicker = pOrignDesc->isFlicker;
	tDesc.fFlickerMin = pOrignDesc->fFlickerMin;
	tDesc.fFlickerSpeed = pOrignDesc->fFlickerSpeed;
	tDesc.fEmissivePower = pOrignDesc->fEmissviePower;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC) , g_wszLightObject_Prototype_Tag  , tDesc.iLevelIndex , g_wszStaticObjectLayer , &tDesc );

	return S_OK;
}

HRESULT CBuilder_Map::Create_Fog(const DTO::TMap_MapObjectData& tData)
{
	CFog::FOG_DESC tDesc{};
	tDesc.iLevelIndex			= ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded			= tData.isUELoaded;
	tDesc.eMapObjectDrawType	= static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath			= Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum			= tData.iSectionNum;
	tDesc.eClientMakePath		= tData.eClientMakePath;


	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	CGameObject* pGameObject{ nullptr };
	pGameObject=m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszRock_Prototype_Tag,
		tDesc.iLevelIndex, g_wszStaticObjectLayer, &tDesc);
	
	if (pGameObject == nullptr)
		return E_FAIL;

	return S_OK;
}
#pragma endregion

#pragma region Create InvisibleWalll

HRESULT CBuilder_Map::Create_InvisibleWall(const DTO::TMap_MapObjectData& tData)
{
	CInvisibleWall::INVISIBLEWALL_DESC tDesc{};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.isUELoaded = tData.isUELoaded;
	tDesc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tDesc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tDesc.iSectionNum = tData.iSectionNum;
	tDesc.eClientMakePath = tData.eClientMakePath;

	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tDesc.vecSRT.push_back(SRT_DATA);
	}

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
		g_wszInvisibleWall_Prototype_Tag, ENUM_TO_UINT(m_eLevelType),
		g_wszInvisibleWallLayer, &tDesc);


	return S_OK;
}

HRESULT CBuilder_Map::Batch_NPC(const DTO::TMap_MapObjectData& tData)
{
	/* EObject Enum Tag 별로 Batch */

	if (tData.vecClientMakePathDesc.empty())		return E_FAIL;
	if (tData.vecSRTs.empty())						return E_FAIL;

	CGameObject* pResult{ nullptr };

	_uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};
	_uint iCurLevelIndex = ENUM_TO_UINT(m_eLevelType);

	/* SRT Data를 들고온다 */
	DTO::SRT_DATA tSRT = tData.vecSRTs.front();
	CTransform::TRANSFORM_DESC tTransformDesc = {};
	tTransformDesc.TranslationMatrix = tSRT.Get_World();

	/* Description 제일 맨앞 */
	BATCH_NPC_DESC* pDesc = static_cast<BATCH_NPC_DESC*>(tData.vecClientMakePathDesc.front());
	if (pDesc == nullptr) return E_FAIL;

	if (FAILED(CNPC_Base::Create_NPC(pDesc, iFindPrototypeIndex, iCurLevelIndex, &tTransformDesc)))
		return E_FAIL;

	return S_OK;
}

#pragma endregion



#pragma region Batch Player

HRESULT CBuilder_Map::Batch_Player(const DTO::TMap_MapObjectData& tData)
{
	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC),g_wszPlayerLayer);
	if (pPlayer == nullptr) return E_FAIL;

	CTransform* pTs = pPlayer->Get_Component<CTransform>();
	if (pTs == nullptr) return E_FAIL;



	/* Player Info */
	if (tData.vecSRTs.empty()) return E_FAIL;
	DTO::SRT_DATA tSRT = tData.vecSRTs.front();
	//pTs->Set_Info(TRANSFORM_INFO_STATE::POS , tSRT.vPosition);

	CPhysicsCCT* pCCT = pPlayer->Get_Component<CPhysicsCCT>();
	if (pCCT == nullptr) return E_FAIL;
	//Vec3 vPosition{ tSRT.vPosition.x , tSRT.vPosition.y ,tSRT.vPosition.z };
	pCCT->SetFootPosition(tSRT.vPosition);
	pTs->Set_Info(TRANSFORM_INFO_STATE::POS, pCCT->GetFootPosition());


	return S_OK;
}

#pragma endregion

#pragma region Batch Monster
HRESULT CBuilder_Map::Batch_Monster(const DTO::TMap_MapObjectData& tData)
{
	/* Monster Type별로 Batch */

	if (tData.vecClientMakePathDesc.empty())		return E_FAIL;
	if (tData.vecSRTs.empty())						return E_FAIL;

	CGameObject* pResult{nullptr};

	_uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};
	_uint iCurLevelIndex = ENUM_TO_UINT(m_eLevelType);

	/* SRT Data를 들고온다 */
	DTO::SRT_DATA tSRT = tData.vecSRTs.front();
	CTransform::TRANSFORM_DESC tTransformDesc = {};
	tTransformDesc.TranslationMatrix = tSRT.Get_World();

	/* Description 제일 맨앞 */
	BATCH_MONSTER_DESC* pDesc = static_cast<BATCH_MONSTER_DESC*>(tData.vecClientMakePathDesc.front());
	if (pDesc == nullptr) return E_FAIL;

	if (FAILED(CMonster_Base::Create_Mosnter(CBuilder_Map::Change_MakeMonsterType_To_MonsterType(pDesc->eBatchMonsterType), iFindPrototypeIndex , iCurLevelIndex , &tTransformDesc)))
		return E_FAIL;

	return S_OK;
}
#pragma endregion

#pragma region Batch Object
HRESULT CBuilder_Map::Batch_Object(const DTO::TMap_MapObjectData& tData)
{
	/* Batch Object */

	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;
	if (tData.vecSRTs.empty())				 return E_FAIL;

	/* Transform  */
	CTransform::TRANSFORM_DESC tTransformDesc{};
	tTransformDesc.TranslationMatrix = tData.vecSRTs.front().Get_World();

	_uint	iAddLevelIndex{ENUM_TO_UINT(m_eLevelType)};
	_uint	iFindPrototypeIndex{ENUM_TO_UINT(ELevelType::STATIC)};
	wstring wstrPrototypeTag{L""};
	wstring wstrAddLayerTag{L""};

	BATCH_OBJECT_DESC* pBatchObjectDesc = static_cast<BATCH_OBJECT_DESC*>(tData.vecClientMakePathDesc.front());
	DTO::EMakeObjectType eMakeObjectType{ pBatchObjectDesc->eBatchObjectType };

	CGameObject* pResult{ nullptr };
	switch (eMakeObjectType)
	{
	case DTO::EMakeObjectType::Battle_Field:
	{
		wstrAddLayerTag		= g_wszBattleFieldLayer;
		wstrPrototypeTag	= g_wszBattleField_Prototype_Tag;

		BATTLE_FIELD_DESC* pOrigin = static_cast<BATTLE_FIELD_DESC*>(pBatchObjectDesc->pBatchObjectDesc);
		if (pOrigin == nullptr) return E_FAIL;

		CBattleField::BATTLEFIELD_DESC tBattleFieldDesc{};
		tBattleFieldDesc.pTransform_Desc = &tTransformDesc;
		tBattleFieldDesc.eFieldType = static_cast<Client::CBattleField::Field_Type>(pOrigin->eFieldType);
		tBattleFieldDesc.iLevelIndex = iAddLevelIndex;
		tBattleFieldDesc.fRadius = pOrigin->fRadius;
		tBattleFieldDesc.vExtents		= pOrigin->vExtents;

		pResult = m_pGameInstance->Add_GameObject(iFindPrototypeIndex, wstrPrototypeTag, iAddLevelIndex, wstrAddLayerTag, &tBattleFieldDesc);
		if (pResult == nullptr)
			return E_FAIL;
	}
	break;
	case DTO::EMakeObjectType::PointLight:
	{
		wstrAddLayerTag		= g_wszPointLightLayer;
		wstrPrototypeTag	= g_wszPointLight_Prototype_Tag;

		Engine::POINTLIHGT_DESC* pOrigin = static_cast<Engine::POINTLIHGT_DESC*>(pBatchObjectDesc->pBatchObjectDesc);
		if (pOrigin == nullptr) return E_FAIL;

		CPointLight::POINTLIGHT_DESC tPointLightDecs{};
		tPointLightDecs.pTransform_Desc = &tTransformDesc;	//Transform Data

		tPointLightDecs.tLightDesc			= pOrigin->tLightDesc;
		tPointLightDecs.tLightDesc.fRange	= pOrigin->fBaseRange; 
		tPointLightDecs.isFlicker			= pOrigin->isFlicker;
		tPointLightDecs.fFlickerMin			= pOrigin->fFlickerMin;
		tPointLightDecs.fFlickerSpeed		= pOrigin->fFlickerSpeed;

		pResult = m_pGameInstance->Add_GameObject(iFindPrototypeIndex, wstrPrototypeTag, iAddLevelIndex, wstrAddLayerTag,&tPointLightDecs);
		if (pResult == nullptr)
			return E_FAIL;
	}
	break;
	default:									return E_FAIL;
	}


	return S_OK;
}
HRESULT CBuilder_Map::Batch_InteractiveObject(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;
	if (tData.vecSRTs.empty()) return E_FAIL;

	_uint iAddLevelIndex = ENUM_TO_UINT(m_eLevelType);

	DTO::SRT_DATA tSRTData = tData.vecSRTs.front();
	CTransform::TRANSFORM_DESC tTsDesc{};
	tTsDesc.TranslationMatrix = tSRTData.Get_World();

	if (FAILED(CInteractiveObject::Create_InteractiveObject(static_cast<BATCH_INTERACTIVEOBJECT_DESC*>(tData.vecClientMakePathDesc.front())
		, iAddLevelIndex, Engine_Utils::ToWString(tData.strModelPath), &tTsDesc)))
		return E_FAIL;

	return S_OK;
}
#pragma endregion




#pragma region TriggerBox ChangeLevel
HRESULT CBuilder_Map::Create_TriggerBox_ChangeLevel(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;


	TRIGGERBOX_CHANGELEVEL_DESC* pTriggerBox_ChangeLevel_Desc = static_cast<TRIGGERBOX_CHANGELEVEL_DESC*> (tData.vecClientMakePathDesc.front());
	if (pTriggerBox_ChangeLevel_Desc == nullptr) return E_FAIL;

	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_LevelChange::TRIGGERBOX_LEVELCHANGE_DESC tDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = {tSRT.Get_World()};

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.pSRTData = &tSRT;
	tDesc.pTransform_Desc = &transformDesc;
	tDesc.vTriggerBox_Extents	= pTriggerBox_ChangeLevel_Desc->vExtents;
	tDesc.eChangeLevelType		= StringToClientleveltype(pTriggerBox_ChangeLevel_Desc->strChangeLevelTypeName);
	tDesc.vTriggerBox_Rotation	= pTriggerBox_ChangeLevel_Desc->vRotation;

	if (tDesc.bHasQuest = pTriggerBox_ChangeLevel_Desc->bHasQuest)
		tDesc.tQuestObjectDesc = pTriggerBox_ChangeLevel_Desc->tQuestObjectDesc;

	m_pGameInstance->Add_GameObject( ENUM_TO_UINT(ELevelType::STATIC) , g_wszTriggerBox_ChangeLevel_Prototype_Tag , ENUM_TO_UINT(m_eLevelType) , g_wszTriggerBoxLayer ,&tDesc);

	return S_OK;
}
#pragma endregion

#pragma region TriggerBox Monster Spawner
HRESULT CBuilder_Map::Create_TriggerBox_MonsterSpawner(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;


	TRIGGERBOX_MONSTERSPAWNER_DESC* pOrigin = static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*> (tData.vecClientMakePathDesc.front());
	if (pOrigin == nullptr) return E_FAIL;

	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_MonsterSpawner::TRIGGERBOX_MONSTERSPAWNER_DESC  tDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = { tSRT.Get_World() };

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.pSRTData = &tSRT;
	tDesc.pTransform_Desc = &transformDesc;
	tDesc.vTriggerBox_Extents = pOrigin->vExtents;
	tDesc.vecMonsterSpawnData = pOrigin->vecMonsterSpawnData;
	tDesc.vTriggerBox_Rotation = pOrigin->vRotation;


	if (tDesc.bHasQuest = pOrigin->bHasQuest)
		tDesc.tQuestObjectDesc = pOrigin->tQuestObjectDesc;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), g_wszTriggerBox_MonsterSapwner_Prototype_Tag , ENUM_TO_UINT(m_eLevelType), g_wszTriggerBoxLayer, &tDesc);
	return S_OK;
}
HRESULT CBuilder_Map::Create_TriggerBox_MonsterWaveSpawner(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;

	TRIGGERBOX_MONSTERWAVESPAWNER_DESC* pOrigin = static_cast<TRIGGERBOX_MONSTERWAVESPAWNER_DESC*> (tData.vecClientMakePathDesc.front());
	if (pOrigin == nullptr) return E_FAIL;

	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_MonsterWaveSpawner::MONSTERWAVESPAWNER_DESC  tDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = { tSRT.Get_World() };

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.pSRTData = &tSRT;
	tDesc.pTransform_Desc = &transformDesc;
	tDesc.vTriggerBox_Extents = pOrigin->vExtents;
	tDesc.tWaveData = *pOrigin;
	tDesc.vTriggerBox_Rotation = pOrigin->vRotation;


	if (tDesc.bHasQuest = pOrigin->bHasQuest)
		tDesc.tQuestObjectDesc = pOrigin->tQuestObjectDesc;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), g_wszTriggerBox_MonsterWaveSpawner_Prototype_Tag, ENUM_TO_UINT(m_eLevelType), g_wszTriggerBoxLayer, &tDesc);
	return S_OK;
}
#pragma endregion

#pragma region TriggerBox GlobalEvent BroadCaster
HRESULT CBuilder_Map::Create_TriggerBox_GlobalEvent_BroadCaster(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;

	Engine::TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC* pOrigin = static_cast<Engine::TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC*>(tData.vecClientMakePathDesc.front());
	if (pOrigin == nullptr) return E_FAIL;


	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_GlobalEvent_BroadCaster::TRIGGERBOX_GLOBALEVENT_BROADCASTER_DESC  tDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = { tSRT.Get_World() };

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.pSRTData = &tSRT;
	tDesc.pTransform_Desc = &transformDesc;
	tDesc.vTriggerBox_Extents = pOrigin->vExtents;
	tDesc.vTriggerBox_Rotation = pOrigin->vRotation;

	tDesc.vecGlobalBroadcastType.reserve(pOrigin->vecGlobalEventBroadCasetNames.size());
	for (auto& str : pOrigin->vecGlobalEventBroadCasetNames)
	{
		tDesc.vecGlobalBroadcastType.push_back(Global_Broadcast_Type_ToEnum(str));
	}

	if (tDesc.bHasQuest = pOrigin->bHasQuest)
		tDesc.tQuestObjectDesc = pOrigin->tQuestObjectDesc;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), 
									g_wszTriggerBox_GlobalEvent_BroadCaster_PrototypeTag, ENUM_TO_UINT(m_eLevelType),
									g_wszTriggerBoxLayer, &tDesc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_TriggerBox_TutorialUIEvent(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;

	Engine::TRIGGERBOX_TUTORIALUIEVENT_DESC* pOrigin = static_cast<Engine::TRIGGERBOX_TUTORIALUIEVENT_DESC*>(tData.vecClientMakePathDesc.front());
	if (pOrigin == nullptr) return E_FAIL;


	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_TutorialUIEvent::TRIGGERBOX_TUTORIALUIEVENT_DESC tDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = { tSRT.Get_World() };

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.pSRTData				= &tSRT;
	tDesc.pTransform_Desc		= &transformDesc;
	tDesc.vTriggerBox_Extents	= pOrigin->vExtents;
	tDesc.eType					= UITutorialPopUpTypeID_ToEnum(pOrigin->strEventName);
	tDesc.vTriggerBox_Rotation	= pOrigin->vRotation;


	if (tDesc.bHasQuest = pOrigin->bHasQuest)
		tDesc.tQuestObjectDesc = pOrigin->tQuestObjectDesc;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
		g_wszTriggerBox_TutorialUIEvent_PrototypeTag, ENUM_TO_UINT(m_eLevelType),
		g_wszTriggerBoxLayer, &tDesc);

	return S_OK;
}

HRESULT CBuilder_Map::Create_TriggerBox_CinematicPlayer(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;

	Engine::TRIGGERBOX_CINEMATICPLAYER_DESC* pOrigin = static_cast<Engine::TRIGGERBOX_CINEMATICPLAYER_DESC*>(tData.vecClientMakePathDesc.front());
	if (pOrigin == nullptr) return E_FAIL;


	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_CinematicPlayer::TRIGGERBOX_CINEMATICCAMERA_DESC tDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = { tSRT.Get_World() };

	tDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tDesc.pSRTData = &tSRT;
	tDesc.pTransform_Desc = &transformDesc;
	tDesc.vTriggerBox_Extents = pOrigin->vExtents;
	tDesc.vTriggerBox_Rotation = pOrigin->vRotation;

	tDesc.strCinemaitcCameraSequnceName = pOrigin->strCinematicName;


	if (tDesc.bHasQuest = pOrigin->bHasQuest)
		tDesc.tQuestObjectDesc = pOrigin->tQuestObjectDesc;

	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
		g_wszTriggerBox_CinematicPlayer_PrototypeTag, ENUM_TO_UINT(m_eLevelType),
		g_wszTriggerBoxLayer, &tDesc);

	return S_OK;
}
#pragma endregion




EMonster_Type CBuilder_Map::Change_MakeMonsterType_To_MonsterType(DTO::EMakeMonsterType eMakeMonsterType)
{
	switch (eMakeMonsterType)
	{
	case DTO::EMakeMonsterType::Dog:			return EMonster_Type::Dog;
	case DTO::EMakeMonsterType::Boomer:			return EMonster_Type::Boomer;
	case DTO::EMakeMonsterType::Shooter:		return EMonster_Type::Shooter;
	case DTO::EMakeMonsterType::Xibi:			return EMonster_Type::Xibi;
	case DTO::EMakeMonsterType::Lianhuo:		return EMonster_Type::Lianhuo;
	default:									return EMonster_Type::END;
	}
	return EMonster_Type::END;
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
	Safe_Release(m_pInstMeshShader);

	Super::Free();
}