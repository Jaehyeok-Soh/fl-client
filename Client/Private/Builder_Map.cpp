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
#include "Shader.h"

#pragma region Batch 관련
/* Batch Player */
#include "MainPlayer.h"

/* Batch Monster */
#include "Monster_Body_Base.h"

/* Batch Object */
#include "BattleField.h"

#pragma endregion

#pragma region Trigger Box
#include "TriggerBox_LevelChange.h"
#include "TriggerBox_MonsterSpawner.h"
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
			case DTO::EClientMakePath::StaticObject:	Create_StaticObject(tData); break;
			case DTO::EClientMakePath::LandScape:		Create_LandScape(tData);	break;
			case DTO::EClientMakePath::Bush:			Create_Bush(tData);			break;
			case DTO::EClientMakePath::Tree:			Create_Tree(tData);			break;
			case DTO::EClientMakePath::Grass:			Create_Grass(tData);		break;
			case DTO::EClientMakePath::Moss:			Create_Moss(tData);			break;
			case DTO::EClientMakePath::Rock:			Create_Rock(tData);			break;
			case DTO::EClientMakePath::Vine:			Create_Vine(tData);			break;
			case DTO::EClientMakePath::Water:			Create_Water(tData);		break;

			case DTO::EClientMakePath::Batch_Player:	Batch_Player(tData);		break;
			case DTO::EClientMakePath::Batch_Monster:	Batch_Monster(tData);		break;
			case DTO::EClientMakePath::Batch_Object:	Batch_Object(tData);		break;

			case DTO::EClientMakePath::TriggerBox_ChangeLevel:		Create_TriggerBox_ChangeLevel(tData); break;
			case DTO::EClientMakePath::TriggerBox_MonsterSpawner:	Create_TriggerBox_MonsterSpawner(tData); break;


			default:									return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CBuilder_Map::LevelData_Setting(const DTO::TLevelData& tData)
{
	m_eLevelType = StringToClientleveltype(tData.strLevelTypeName);


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

	tStaticObjectDesc.iLevelIndex		 = ENUM_TO_UINT(m_eLevelType);
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

	m_pGameInstance->Add_GameObject( ENUM_TO_UINT(ELevelType::STATIC),g_wszStaticObject_Prototype_Tag , tStaticObjectDesc.iLevelIndex , g_wszStaticObjectLayer,&tStaticObjectDesc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_LandScape(const DTO::TMap_MapObjectData& tData)
{
	CLandScape::LANDSCAPE_DESC tLandSapceDesc{};

	tLandSapceDesc.iLevelIndex			= ENUM_TO_UINT(m_eLevelType);
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
		ENUM_TO_UINT(ELevelType::STATIC), g_wszLandScape_Prototype_Tag , 
		tLandSapceDesc.iLevelIndex, g_wszStaticObjectLayer, &tLandSapceDesc);


	return S_OK;
}
HRESULT CBuilder_Map::Create_Bush(const DTO::TMap_MapObjectData& tData)
{
	CBush::BUSH_DESC tBush_Desc{};

	tBush_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tBush_Desc.isUELoaded = tData.isUELoaded;
	tBush_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tBush_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tBush_Desc.iSectionNum = tData.iSectionNum;

	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tBush_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszBush_Prototype_Tag ,
		tBush_Desc.iLevelIndex, g_wszStaticObjectLayer, &tBush_Desc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_Grass(const DTO::TMap_MapObjectData& tData)
{
	CGrass::GRASS_DESC tGrass_Desc{};

	tGrass_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tGrass_Desc.isUELoaded = tData.isUELoaded;
	tGrass_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tGrass_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tGrass_Desc.iSectionNum = tData.iSectionNum;

	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tGrass_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszGrass_Prototype_Tag ,
		tGrass_Desc.iLevelIndex, g_wszStaticObjectLayer, &tGrass_Desc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_Moss(const DTO::TMap_MapObjectData& tData)
{

	CGrass::GRASS_DESC tMoss_Desc{};

	tMoss_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tMoss_Desc.isUELoaded = tData.isUELoaded;
	tMoss_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tMoss_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tMoss_Desc.iSectionNum = tData.iSectionNum;

	/* SRT DATA */
	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tMoss_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszMoss_Prototype_Tag ,
		tMoss_Desc.iLevelIndex, g_wszStaticObjectLayer, &tMoss_Desc);

	return S_OK;

}
HRESULT CBuilder_Map::Create_Tree(const DTO::TMap_MapObjectData& tData)
{
	CTree::TREE_DESC tTree_Desc{};

	tTree_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tTree_Desc.isUELoaded = tData.isUELoaded;
	tTree_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tTree_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tTree_Desc.iSectionNum = tData.iSectionNum;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tTree_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszTree_Prototype_Tag ,
		tTree_Desc.iLevelIndex, g_wszStaticObjectLayer, &tTree_Desc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_Vine(const DTO::TMap_MapObjectData& tData)
{
	CVine::VINE_DESC tVine_Desc{};

	tVine_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tVine_Desc.isUELoaded = tData.isUELoaded;
	tVine_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tVine_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tVine_Desc.iSectionNum = tData.iSectionNum;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tVine_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC),g_wszVine_Prototype_Tag ,
		tVine_Desc.iLevelIndex, g_wszStaticObjectLayer, &tVine_Desc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_Water(const DTO::TMap_MapObjectData& tData)
{
	CVine::VINE_DESC tWater_Desc{};

	tWater_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);;
	tWater_Desc.isUELoaded = tData.isUELoaded;
	tWater_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tWater_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tWater_Desc.iSectionNum = tData.iSectionNum;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tWater_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszWater_Prototype_Tag ,
		tWater_Desc.iLevelIndex, g_wszStaticObjectLayer, &tWater_Desc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_Rock(const DTO::TMap_MapObjectData& tData)
{
	CVine::VINE_DESC tRock_Desc{};

	tRock_Desc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	tRock_Desc.isUELoaded = tData.isUELoaded;
	tRock_Desc.eMapObjectDrawType = static_cast<EMapObject_DrawType>(tData.eMapObjectDrawType);
	tRock_Desc.wstrModelPath = Engine_Utils::ToWString(tData.strModelPath);
	tRock_Desc.iSectionNum = tData.iSectionNum;

	for (auto& SRT_DATA : tData.vecSRTs)
	{
		tRock_Desc.vecSRT.push_back(SRT_DATA);
	}


	m_pGameInstance->Add_GameObject(
		ENUM_TO_UINT(ELevelType::STATIC), g_wszRock_Prototype_Tag ,
		tRock_Desc.iLevelIndex, g_wszStaticObjectLayer, &tRock_Desc);

	return S_OK;
}
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

HRESULT CBuilder_Map::Batch_Object(const DTO::TMap_MapObjectData& tData)
{
	/* Batch Object */

	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;
	if (tData.vecSRTs.empty())				 return E_FAIL;

	/* Transform  */
	CTransform::TRANSFORM_DESC tTransformDesc{};
	tTransformDesc.TranslationMatrix = tData.vecSRTs.front().Get_World();

	void*	pDesc{nullptr};
	_uint	iAddLevelIndex{ENUM_TO_UINT(m_eLevelType)};
	_uint	iFindPrototypeIndex{ENUM_TO_UINT(ELevelType::STATIC)};
	wstring wstrPrototypeTag{L""};
	wstring wstrAddLayerTag{L""};

	BATCH_OBJECT_DESC* pBatchObjectDesc = static_cast<BATCH_OBJECT_DESC*>(tData.vecClientMakePathDesc.front());
	DTO::EMakeObjectType eMakeObjectType{ pBatchObjectDesc->eBatchObjectType };

	switch (eMakeObjectType)
	{
	case DTO::EMakeObjectType::Battle_Field:
	{
		wstrAddLayerTag		= g_wszBattleFieldLayer;
		wstrPrototypeTag	= g_wszBattleField_Prototype_Tag;

		BATTLE_FIELD_DESC* pDataDesc = static_cast<BATTLE_FIELD_DESC*>(pBatchObjectDesc->pBatchObjectDesc);
		if (pDataDesc == nullptr) return E_FAIL;

		CBattleField::BATTLEFIELD_DESC tClientDesc{};
		tClientDesc.pTransform_Desc = &tTransformDesc;
		tClientDesc.eFieldType		= static_cast<Client::CBattleField::Field_Type>(pDataDesc->eFieldType);
		tClientDesc.iLevelIndex		= iAddLevelIndex;
		tClientDesc.fRadius			= pDataDesc->fRadius;
		tClientDesc.vExtents		= pDataDesc->vExtents;
		pDesc = &tClientDesc;
	}
	break;
	default:									return E_FAIL;
	}

	CGameObject* pResult{nullptr};
	pResult =  m_pGameInstance->Add_GameObject(iFindPrototypeIndex , wstrPrototypeTag, iAddLevelIndex , wstrAddLayerTag, pDesc );
	if (pResult == nullptr)
		return E_FAIL;

	return S_OK;
}

HRESULT CBuilder_Map::Create_TriggerBox_ChangeLevel(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;


	TRIGGERBOX_CHANGELEVEL_DESC* pTriggerBox_ChangeLevel_Desc = static_cast<TRIGGERBOX_CHANGELEVEL_DESC*> (tData.vecClientMakePathDesc.front());
	if (pTriggerBox_ChangeLevel_Desc == nullptr) return E_FAIL;

	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_LevelChange::TRIGGERBOX_LEVELCHANGE_DESC pDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = {tSRT.Get_World()};

	pDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	pDesc.pSRTData = &tSRT;
	pDesc.pTransform_Desc = &transformDesc;
	pDesc.vTriggerBox_Extents = pTriggerBox_ChangeLevel_Desc->vExtents;
	pDesc.eChangeLevelType = StringToClientleveltype(pTriggerBox_ChangeLevel_Desc->strChangeLevelTypeName);


	m_pGameInstance->Add_GameObject( ENUM_TO_UINT(ELevelType::STATIC) , g_wszTriggerBox_ChangeLevel_Prototype_Tag , ENUM_TO_UINT(m_eLevelType) , g_wszTriggerBoxLayer ,&pDesc);

	return S_OK;
}
HRESULT CBuilder_Map::Create_TriggerBox_MonsterSpawner(const DTO::TMap_MapObjectData& tData)
{
	if (tData.vecSRTs.empty()) return E_FAIL;
	if (tData.vecClientMakePathDesc.empty()) return E_FAIL;


	TRIGGERBOX_MONSTERSPAWNER_DESC* pTriggerBox_MonsterSpawner = static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*> (tData.vecClientMakePathDesc.front());
	if (pTriggerBox_MonsterSpawner == nullptr) return E_FAIL;

	DTO::SRT_DATA tSRT{ tData.vecSRTs.front() };
	CTriggerBox_MonsterSpawner::TRIGGERBOX_MONSTERSPAWNER_DESC  pDesc{};
	CTransform::TRANSFORM_DESC transformDesc = {};
	transformDesc.TranslationMatrix = { tSRT.Get_World() };

	pDesc.iLevelIndex = ENUM_TO_UINT(m_eLevelType);
	pDesc.pSRTData = &tSRT;
	pDesc.pTransform_Desc = &transformDesc;
	pDesc.vTriggerBox_Extents = pTriggerBox_MonsterSpawner->vExtents;
	pDesc.vecMonsterSpawnData = pTriggerBox_MonsterSpawner->vecMonsterSpawnData;
	m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), g_wszTriggerBox_MonsterSapwner_Prototype_Tag , ENUM_TO_UINT(m_eLevelType), g_wszTriggerBoxLayer, &pDesc);
	return S_OK;
}

EMonster_Type CBuilder_Map::Change_MakeMonsterType_To_MonsterType(DTO::EMakeMonsterType eMakeMonsterType)
{
	switch (eMakeMonsterType)
	{
	case DTO::EMakeMonsterType::Dog:			return EMonster_Type::Dog;
	case DTO::EMakeMonsterType::Boomer:			return EMonster_Type::Boomer;
	case DTO::EMakeMonsterType::Shooter:		return EMonster_Type::Shooter;
	case DTO::EMakeMonsterType::Xibi:			return EMonster_Type::Xibi;
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


	Super::Free();
}