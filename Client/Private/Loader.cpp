#include "pch.h"
#include "Engine_Utils.h"
//=================
// Component
//=================
#include "PlayerControlContext.h"
#include "MonsterControlContext.h"
#include "MonoBehaviour.h"
#include "EffectHandler.h"
#include "PlayerActionState.h"
#include "MonsterActionState.h"
#include "StatCom_Player.h"
#include "Collider.h"
#include "Xibi_GimmikController.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Particle_Rect.h"
#include "VIBuffer_Particle_Point.h"
#include "VIBuffer_Particle_Mesh.h"
#include "InstanceMesh.h"
#include "VIBuffer_Cube_Tex.h"
#include "Bounds.h"
#include "Shader.h"
#include "Camera.h"
#include "Transform.h"
#include "SkillBase_MoonE.h"
#include "SkillBase_MoonQ.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
//=================
// Builder
//=================
#include "DataDocument_Example.h"
#include "DataDocument_Map.h"
#include "DataDocument_Effect.h"
#include "DataDocument_UI.h"
#include "Builder_Example.h"
#include "Builder_UI.h"
#include "Builder_UIPrefabs.h"
#include "BuilderSystem.h"
#include "Builder_AttackOverlap.h"
#include "DataStruct_AttackOverlap.h"
#include "DataDocument_AttackOverlap.h"
#include "Builder_EffectEvent.h"
#include "DataStruct_EffectEvent.h"
#include "DataDocument_EffectEvent.h"
#include "Builder_AttackPreset.h"
#include "DataDocument_AttackPreset.h"
//=================
// Object
//=================
#include "Texture.h"
#include "MainPlayer.h"
#include "CameraMan_Targeter.h"
#include "Body.h"
#include "Weapon.h"
#include "Sword.h"
#include "Gun.h"
#include "ColliderPart.h"
#include "Loader.h"
#include "Effect.h"
#include "EffectObject.h"
#include "BattleField.h"
#include "PartEffect.h"


//=================
// SkillObject
//=================
#include "SingleSkillSpawner.h"
#include "ProjectileSpawner_Fan.h"
#include "SkillObjectSpawner_RandomXZ.h"
#include "ProjectileSpawner_Radial360.h"
// xibi
#include "Xibi_Projectile_Circle.h"
#include "Xibi_Loop_Thunder.h"
#include "Xibi_Oneshot_Thunder.h"
// player
#include "Moon_SkillE_Obj.h"
#include "Moon_SkillQSheild_Obj.h"
#include "Moon_SkillQAttack_Obj.h"


//=================
// Map Object
//=================
#include "StaticObject.h"
#include "LandScape.h"
#include "Bush.h"
#include "Moss.h"
#include "Water.h"
#include "Rock.h"
#include "Vine.h"
#include "Tree.h"
#include "Grass.h"

//=================
// Trigger Box
//=================
#include "TriggerBox_LevelChange.h"
#include "TriggerBox_MonsterSpawner.h"
#include "TriggerBox_GlobalEvent_BroadCaster.h"

/* --------------------- */
//=================
// MONSTER
//=================
#include "Monster_Dog.h"
#include "Monster_Dog_Body.h"
#include "Monster_Boomer.h"
#include "Monster_Boomer_Body.h"
//=================
// BOSS
//=================
#include "Boss_Xibi.h"
#include "Boss_Xibi_Body.h"

//=================
// UI
//=================
#include "GenericUI.h"
//프로그레스바
#include "UIPlayerStat_Progress.h"
#include "UILoading_Progress.h"
#include "UIMonsterStat_Progress.h"
#include "UIPlayerAmmo_Progress.h"
#include "UIBossStat_Progress.h"
// 텍스트 
#include "UIMenu_Text.h"
#include "UIPlayerStat_Text.h"
#include "UILoading_Text.h"
#include "UIMonsterStat_Text.h"
#include "UIDamageFont_Text.h"
#include "UIBossStat_Text.h"
#include "UICombo_Text.h"
#include "UIBossAction_Text.h"
// 그냥 이미지
#include "UIJust_Image.h"
// 다이나믹 이미지 
#include "UISkill_BG.h"
#include "UIMini_Map.h"
#include "UIHover_Image.h"
#include "UIMenu_Image.h"
#include "UIMenu_OutLine.h"
#include "UILoading_Image.h"
#include "UINameplate_BG.h"
#include "UIAimDot_Image.h"
#include "UILevelChange_Image.h"
#include "UIBossStat_Image.h"
#include "UICombo_Image.h"
#include "UIMiniMap_Monster_Icon.h"
#include "UIBossAction_Image.h"
// 트리거 
#include "UIMenu_Trigger.h"
#include "UICommon_Trigger.h"
#include "UIMenu_Exit_Trigger.h"
//=================
// Resource
//=================
#include "TextureBase.h"
#include "Model.h"
#include "ModelLoader.h"
#include "GameInstance.h"

#pragma region Macro
#define ADD_PROTOTYPE(eLevelType, wstrPrototypeTag, pBase) if(FAILED(m_pGameInstance->Add_Prototype(ENUM_TO_UINT(eLevelType), wstrPrototypeTag, pBase))) return E_FAIL

#define REGISTER_GLOBAL_EVENT(EventStructName) \
	m_pGameInstance->Register_GlobalEventsBroadCast( \
		ENUM_TO_UINT(EGlobal_Broadcast_Type::EventStructName), \
		[pGameInstance = m_pGameInstance]() { pGameInstance->Broadcast<EventStructName>(); } \
	)
#pragma endregion


#define TIME_DELTA 0.016f 

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
	m_pBuilderSystem = CBuilderSystem::Create();

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
	case Client::ELevelType::LOADING:
		hr = Loading_For_LoadLevel();
		break;
	case Client::ELevelType::LOGO:
		hr = Loading_For_Logo();
		break;
	case Client::ELevelType::TUTORIAL_VILLAGE:
		hr = Loading_For_Tutorial_Village();
		break;
	case Client::ELevelType::TUTORIAL_BOSS:
		hr = Loading_For_Tutorial_Boss();
		break;
	case Client::ELevelType::SQUARE:
		hr = Loading_For_Square();
		break;
	case Client::ELevelType::TEST:
		hr = Loading_For_Test();
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

HRESULT CLoader::Loading_For_LoadLevel()
{
#pragma region ToolData
	{
		// Regist Document
		{
			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(ENUM_TO_UINT(ELevelType::LOADING), DTO::ECategory::UI)))
				return E_FAIL;
		}
	}
#pragma endregion

#pragma region UI

#pragma endregion

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Test()
{
	m_fLoadingRatio = 0.f;
	Sleep(1000);

	m_fLoadingRatio = 1.f;
	Sleep(5000);
	m_isFinished = true;
	return S_OK;
}

/* 첫 실행 떄 등록해야하는 것들 등록 */
HRESULT CLoader::Loading_For_Logo()
{
	if (FAILED(Ready_Spawner()))
		return E_FAIL;



	/* Cinematic Data Load */
	if (FAILED(m_pGameInstance->GameDataManager_Load_CameraCinematicSequence()))
		return E_FAIL;

#pragma region Register Global Event
	/////////////////////////////////////////
	/////////// Ready GlobalEvent ///////////
	/////////////////////////////////////////
	/* Global */
	m_pGameInstance->Register_GlobalEventsBroadCast(ENUM_TO_UINT(EGlobal_Broadcast_Type::NONE), nullptr);
	REGISTER_GLOBAL_EVENT(TUTORIAL_BOSS_CONTATCT);

#pragma endregion


#pragma region PretransformMatrix
	Matrix matPreTransformScaleTest = Matrix::CreateScale(100.f, 100.f, 100.f);
	Matrix matPreTransformScale = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	Matrix matPreTransformScale150 = Matrix::CreateScale(1.5f, 1.5f, 1.5f);
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

			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_EffectEvent>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::EFFECTEVENT)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_UI>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::UI)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_AttackOverlap>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::OVERLAP_SCRIPT)))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Regist_Document<CDataDocument_AttackPreset>(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::ATTACK_PRESET)))
				return E_FAIL;
		}

		// Build prototype
		{
			if (FAILED(Build_Prototype()))
				return E_FAIL;
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

		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/blade")))
			return E_FAIL;

		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Circle")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Object")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Object_Chain")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Lightning")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Object_Female_Character")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Rock")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Cone")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Plane")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Tornado")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Claw")))
			return E_FAIL;
		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Sphere")))
			return E_FAIL;


		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Twist")))
			return E_FAIL;
	}
	m_fLoadingRatio = 0.13f;
	// For. Prototype_Component_Button_Test_Texture
	{
		/*Effect*/
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Crack/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Curve/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Fire/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Flower/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Fluid/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Glow/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Gradient/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Ice/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Knife/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Rock/")))
			return E_FAIL;
		m_fLoadingRatio = 0.33f;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Lens/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Lightning/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Line/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Mask/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Normal/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Object/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Partten/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Smoke/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Spark/")))
			return E_FAIL;
		m_fLoadingRatio = 0.65f;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Splash/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Spread/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Trail/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Turbulence/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/UI/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/VAT/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/water/")))
			return E_FAIL;
		if (FAILED(Loading_Textures(L"../../Resources/Textures/Effect/Wave/")))
			return E_FAIL;
	}

	// For. UI Texture
	if (FAILED(Loading_Textures(L"../../Resources/Textures/UI/UI_Client/")))
		return E_FAIL;

#pragma endregion

#pragma region Texture Splating Data

	///////////////////////////////////////////////////////
	////////// Ready Texture Splating Data Load ///////////
	///////////////////////////////////////////////////////

	/* Texture Loading */

	/* Defualt 사진 */
	if (FAILED(Loading_Textures(L"../../Resources/Textures/Map/LandScape/Defualt/")))
		return E_FAIL;
	/* Village 사진 */
	if (FAILED(Loading_Textures(L"../../Resources/Textures/Map/LandScape/Village/")))
		return E_FAIL;
	/* Clouds 사진 */
	if (FAILED(Loading_Textures(L"../../Resources/Textures/Map/LandScape/Clouds/")))
		return E_FAIL;
	if (FAILED(m_pGameInstance->GameDataManager_Load_TextureSplatingInfoData()))
		return E_FAIL;

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

	// For. Prototype_Component_Model_Moon
	{
		m_fLoadingRatio = 0.99f;
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType					= EModelType::ANIM;
		desc.iPrototypeLevelIndex	= ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform		= &(matPreTransformScale);	// matPreTransformScale // matPreTransformTurn90
		desc.wstrModelFolderName	= L"PlayerMoon";					// PlayerMoon // Pino
		desc.FStageBone				= CModel::STAGEING_BONE::SB_SPCIPICBONE;
		desc.vecStageBoneIndices	= {3,5,72,285,286,287,288,289,295,413,414,415,416 ,417,418,419 };

		// root bone 정보 셋팅 : 없으면 아예 안 넘겨주면 됨
		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_Moon", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For. Prototype_Component_Model_MoonSword
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::STATIC;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale);	// matPreTransformScale
		desc.wstrModelFolderName = L"Weapon_MoonSword";					// PlayerMoon // Pino
		desc.FStageBone = CModel::STAGEING_BONE::SB_ZEROBONE;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_MoonSword", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For. Prototype_Component_Model_MoonSkillWeap
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::STATIC;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale);	// matPreTransformScale
		desc.wstrModelFolderName = L"Weapon_MoonSkill";					// PlayerMoon // Pino
		desc.FStageBone = CModel::STAGEING_BONE::SB_ZEROBONE;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_MoonSkillWeap", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}
	// For. Prototype_Component_Model_MoonGun
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale);
		desc.wstrModelFolderName = L"Weapon_MoonGun";		
		desc.FStageBone = CModel::STAGEING_BONE::SB_ZEROBONE;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_MoonGun", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For.Prototype_Component_Model_Monster_Dog
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformIdentity);
		desc.wstrModelFolderName = L"Monster_Dog";
		desc.FStageBone = CModel::STAGEING_BONE::SB_SPCIPICBONE;
		desc.vecStageBoneIndices = {150, 152};

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 3;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Dog_Model_Prototype_Tag, CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For.Prototype_Component_Model_Monster_Boomer
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale150);
		desc.wstrModelFolderName = L"Monster_Boomer";
		desc.FStageBone = CModel::STAGEING_BONE::SB_SPCIPICBONE;
		desc.vecStageBoneIndices = {114, 116 };

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 3;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Boomer_Model_Prototype_Tag , CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}
	// For. Prototype_Component_Camera
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Camera", CCamera::Create());
	// For. Prototype_Component_ActionState_Player
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ActionState_Player", CPlayerActionState::Create());
	// For. Prototype_Component_ControlContext_Player
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ControlContext_Player", CPlayerControlContext::Create());
	// For. Prototype_Component_Collider_AABB
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), g_wszCollider_AABB_Prototype_Tag, CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::AABB));
	// For. Prototype_Component_Collider_OBB
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), g_wszCollider_OBB_Prototype_Tag , CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::OBB));
	// For. Prototype_Component_Collider_SPHERE
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), g_wszCollider_Sphere_PrototypeTag, CCollider::Create(m_pDevice, m_pDeviceContext, EColliderType::SPHERE));
	// For. Prototype_Component_Bounds
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Bounds", CBounds::Create(m_pDevice, m_pDeviceContext));
	// For. Prototype_Component_VIBuffer_InstanceMesh
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", CInstanceMesh::Create(m_pDevice, m_pDeviceContext));
	// For. Prototype_Component_EffectHandler_SkillObject
	{
		CEffectHandler::ANIM_EFFECT_HANDLER_DESC desc{};
		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_EffectHandler_SkillObject", CEffectHandler::Create(&desc));
	}

	/* player components */
	// For. Prototype_Component_Stat_Player
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Stat_Player", CStatCom_Player::Create());
	// For. Prototype_Component_ControlContext_Monster
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ControlContext_Monster", CMonsterControlContext::Create());
	// For. Prototype_Component_ActionState_Monster
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ActionState_Monster", CMonsterActionState::Create(m_pDevice, m_pDeviceContext));

#pragma endregion

	///////////////////////////////////////
	//////////// Ready Objects ////////////
	///////////////////////////////////////

#pragma region Objects
	{
		// For. Prototype_GameObject_MainPlayer
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_MainPlayer",			CMainPlayer::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_CameraManTargeter
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_CameraManTargeter",	CCameraMan_Targeter::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Part_Body
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Body",			CBody::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Part_Collider
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Collider",		CColliderPart::Create(m_pDevice, m_pDeviceContext));

		// 이펙트 Object
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Effect",				Effect::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Effect_Parts",			CEffectObject::Create(m_pDevice, m_pDeviceContext));
		
		// Projectile

		// player effect object
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMoonSkillE__Prototype_Tag,							CMoon_SkillE_Obj::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMoonSkillQSheild_Prototype_Tag,						CMoon_SkillQSheild_Obj::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMoonSkillQAttack_Prototype_Tag,						CMoon_SkillQAttack_Obj::Create(m_pDevice, m_pDeviceContext));

		/* Battle Field */
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszBattleField_Prototype_Tag ,				CBattleField::Create(m_pDevice, m_pDeviceContext));

#pragma region Map Object
		/* Map Object */
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszStaticObject_Prototype_Tag ,				CStaticObject::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszLandScape_Prototype_Tag,					CLandScape::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszBush_Prototype_Tag,						CBush::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszGrass_Prototype_Tag,						CGrass::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMoss_Prototype_Tag,						CMoss::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszTree_Prototype_Tag,						CTree::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszVine_Prototype_Tag,						CVine::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszRock_Prototype_Tag,						CRock::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszWater_Prototype_Tag,						CWater::Create(m_pDevice, m_pDeviceContext));
#pragma endregion

#pragma region TriggerBox
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszTriggerBox_ChangeLevel_Prototype_Tag,			CTriggerBox_LevelChange::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszTriggerBox_MonsterSapwner_Prototype_Tag,			CTriggerBox_MonsterSpawner::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszTriggerBox_GlobalEvent_BroadCaster_PrototypeTag, CTriggerBox_GlobalEvent_BroadCaster::Create(m_pDevice, m_pDeviceContext));
#pragma endregion

		/* Weapons */
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Sword", CSword::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Gun", CGun::Create(m_pDevice, m_pDeviceContext));

		// For. Prototype_GameObject_Monster_Dummy
		ADD_PROTOTYPE(ELevelType::STATIC , g_wszMonster_Dog_Prototype_Tag , CMonster_Dog::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Monster_Dummy_Body
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMonster_Dog_Body_Prototype_Tag, CMonster_Dog_Body::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Monster_Dummy
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMonster_Boomer_Prototype_Tag , CMonster_Boomer::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Monster_Dummy_Body
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszMonster_Boomer_Body_Prototype_Tag, CMonster_Boomer_Body::Create(m_pDevice, m_pDeviceContext));

#pragma region PartObjs
		ADD_PROTOTYPE(ELevelType::STATIC, g_wszPartObj_Effect_Prototype_Tag, CPartEffect::Create(m_pDevice, m_pDeviceContext));
#pragma endregion

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
	}
#pragma endregion

#pragma region PHYSICS
#pragma endregion
#pragma region UI
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_PlayerStatProgress",		CUIPlayerStat_Progress::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MenuText",					CUIMenu_Text::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_PlayerStatText",			CUIPlayerStat_Text::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_JUST_IMAGE",				CUIJust_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_UIMenuTrigger",			CUIMenu_Trigger::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_UICommonTrigger",			CUICommon_Trigger::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_UIMenuExitTrigger",		CUIMenu_Exit_Trigger::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_SkillBG",					CUISkill_BG::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MiniMap",					CUIMini_Map::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_HoverImage",				CUIHover_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MenuImage",				CUIMenu_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MenuOutline",				CUIMenu_OutLine::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MonsterStatText",			CUIMonsterStat_Text::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MonsterStatProgress",		CUIMonsterStat_Progress::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_Nameplate_BG",				CUINameplate_BG::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_AimDotImage",				CUIAimDot_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_PlayerAmmoProgress",		CUIPlayerAmmo_Progress::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_LevelChangeImage",			CUILevelChange_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_DamageFontText",			CUIDamageFont_Text::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_BossStatProgress",			CUIBossStat_Progress::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_BossStatText",				CUIBossStat_Text::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_BossStatImage",			CUIBossStat_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_ComboImage",				CUICombo_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_ComboText",				CUICombo_Text::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_MiniMapMonsterIconImage",	CUIMiniMap_Monster_Icon::Create(m_pDevice, m_pDeviceContext));
	
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_BossActionImage",			CUIBossAction_Image::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_UI_BossActionText",			CUIBossAction_Text::Create(m_pDevice, m_pDeviceContext));
#pragma endregion
	
	m_isFinished = true;
	return S_OK;
}



HRESULT CLoader::Loading_For_Tutorial_Village()
{
	/* Tutorial Village */
	m_fLoadingRatio = 0.f;
		
	// 오브젝트
	
	// 이펙트 Object
	m_fLoadingRatio = 1.f;
	Sleep(3000);

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Tutorial_Boss()
{
	/* Tutorial Boss */
	m_fLoadingRatio = 0.f;

#pragma region PretransformMatrix
	Matrix matPreTransformScaleTest = Matrix::CreateScale(100.f, 100.f, 100.f);
	Matrix matPreTransformScale = Matrix::CreateScale(0.01f, 0.01f, 0.01f);
	Matrix matPreTransformScale150 = Matrix::CreateScale(1.5f, 1.5f, 1.5f);
	Matrix matPreTransformIdentity = Matrix::Identity;
	Matrix matPreTransformTurn90 = matPreTransformScale * Matrix::CreateFromYawPitchRoll(XMConvertToRadians(90.f), 0.f, 0.f);
#pragma endregion
	// 이펙트 Object

	// For. Prototype_GameObject_Boss_Xibi
	ADD_PROTOTYPE(ELevelType::STATIC, g_wszBoss_Xibi_Prototype_Tag, CBoss_Xibi::Create(m_pDevice, m_pDeviceContext));
	// For. Prototype_GameObject_Boss_XibiBody
	ADD_PROTOTYPE(ELevelType::STATIC, g_wszBoss_Xibi_Body_Prototype_Tag, CBoss_Xibi_Body::Create(m_pDevice, m_pDeviceContext));

	// 오브젝트
	// For.Prototype_Component_Model_Xibi
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale);
		desc.wstrModelFolderName = L"Xibi";
		desc.FStageBone = CModel::STAGEING_BONE::SB_SPCIPICBONE;
		desc.vecStageBoneIndices = { 375 };

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), g_wszBoss_Xibi_Model_Prototype_Tag, CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For.Prototype_Component_Model_XibiWeapon
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale);
		desc.wstrModelFolderName = L"XibiWeapon";
		desc.FStageBone = CModel::STAGEING_BONE::SB_SPCIPICBONE;

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_XibiWeapon", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}
	// For. Prototype_Component_Xibi_GimmikController
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Xibi_GimmikController", CXibi_GimmikController::Create());

	// Projectile
	ADD_PROTOTYPE(ELevelType::TUTORIAL_BOSS, g_wszXibiProjectile_Prototype_Tag, CXibi_Projectile_Circle::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::TUTORIAL_BOSS, g_wszXibiLoopThunder_Prototype_Tag, CXibi_Loop_Thunder::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::TUTORIAL_BOSS, g_wszXibiOneshotThunder_Prototype_Tag, CXibi_Oneshot_Thunder::Create(m_pDevice, m_pDeviceContext));

	m_fLoadingRatio = 1.f;
	Sleep(1000);

	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Square()
{
	/* Square */
	m_fLoadingRatio = 1.f;

	// 오브젝트

	// 이펙트 Object
	ADD_PROTOTYPE(ELevelType::SQUARE, L"Prototype_GameObject_Effect", Effect::Create(m_pDevice, m_pDeviceContext));
	ADD_PROTOTYPE(ELevelType::SQUARE, L"Prototype_GameObject_Effect_Parts", CEffectObject::Create(m_pDevice, m_pDeviceContext));

	m_fLoadingRatio = 1.f;
	Sleep(5000);
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

HRESULT CLoader::Make_StaticObject_Prototype(ELevelType eLevelType, const wstring& wstrFilePath)
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

HRESULT CLoader::Build_Prototype()
{
	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::OVERLAP_SCRIPT, CBuilder_AttackOverlap::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::LOGO)))))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::EFFECTEVENT, CBuilder_EffectEvent::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::LOGO)))))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Ready_Builder(DTO::ECategory::ATTACK_PRESET, CBuilder_AttackPreset::Create(m_pDevice, m_pDeviceContext, ENUM_TO_UINT(ELevelType::LOGO)))))
		return E_FAIL;

	if (FAILED(Build_Files()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Build_Files()
{
	if (FAILED(Ready_AttackPresets()))
		return E_FAIL;

	if (FAILED(Ready_AttackOverlap()))
		return E_FAIL;

	if (FAILED(Ready_EffectEvent()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_AttackOverlap()
{
	if (FAILED(Ready_AttackOverlap_PlayerMoon()))
		return E_FAIL;

	if (FAILED(Ready_AttackOverlap_Monster_Dog()))
		return E_FAIL;
	
	if (FAILED(Ready_AttackOverlap_Monster_Boomer()))
		return E_FAIL;

	if (FAILED(Ready_AttackOverlap_Xibi()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_EffectEvent()
{
	if (FAILED(Ready_EffectEvent_AnimationData()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_Spawner()
{
	_uint iLevelID = ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS);

	/* Xibi */
	// SingleSkill
	{
		CSingleSkillSpawner::SPAWNER_ORIGIN_DESC originDesc{};
		originDesc.iPoolLevelIndex = iLevelID;
		originDesc.wstrSkillPoolTag = g_wszPool_XibiOneshotThunder;
		originDesc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		originDesc.fLifeTime = 0.5f;
		originDesc.fStartDelay = 0.1f;
		if (FAILED(m_pGameInstance->Add_Prototype(iLevelID, g_wszSpawner_XibiOneshotSingleThunder,
			CSingleSkillSpawner::Create(m_pDevice, m_pDeviceContext, &originDesc))))
			return E_FAIL;
	}
	// RandomXZ 
	{
		CSkillObjectSpawner_RandomXZ::SPAWNER_ORIGIN_DESC originDesc{};
		originDesc.iPoolLevelIndex = iLevelID;
		originDesc.wstrSkillPoolTag = g_wszPool_XibiOneshotThunder;
		originDesc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		originDesc.fLifeTime = 0.5f;
		originDesc.fInterval = 0.1f;
		if (FAILED(m_pGameInstance->Add_Prototype(iLevelID, g_wszSpawner_XibiOneshotRandomThunder,
			CSkillObjectSpawner_RandomXZ::Create(m_pDevice, m_pDeviceContext, &originDesc))))
			return E_FAIL;
	}
	// 360Circle
	{
		CProjectileSpawner_Radial360::SPAWNER_ORIGIN_DESC desc{};
		desc.iPoolLevelIndex = iLevelID;
		desc.wstrSkillPoolTag = g_wszPool_XibiCircleProjectile;
		desc.fLifeTime = 5.f;
		desc.fInterval = 0.05f;
		desc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Move_Straight) | ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		if (FAILED(m_pGameInstance->Add_Prototype(iLevelID, g_wszSpawner_Xibi360CircleProjectile,
			CProjectileSpawner_Radial360::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// 360Thunder
	{
		CProjectileSpawner_Radial360::SPAWNER_ORIGIN_DESC desc{};
		desc.iPoolLevelIndex = iLevelID;
		desc.wstrSkillPoolTag = g_wszPool_XibiLoopThunder;
		desc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Move_Straight) | ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		desc.fLifeTime = 7.f;
		desc.fInterval = 0.03f;

		if (FAILED(m_pGameInstance->Add_Prototype(iLevelID, g_wszSpawner_Xibi360ThunderProjectile,
			CProjectileSpawner_Radial360::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// 3wayThunder
	{
		CProjectileSpawner_Radial360::SPAWNER_ORIGIN_DESC desc{};
		desc.iPoolLevelIndex = iLevelID;
		desc.wstrSkillPoolTag = g_wszPool_XibiLoopThunder;
		desc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Move_Straight) | ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		desc.fLifeTime = 7.f;

		if (FAILED(m_pGameInstance->Add_Prototype(iLevelID, g_wszSpawner_Xibi3wayLoopThunder,
			CProjectileSpawner_Fan::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}

	/* player */
	// level : static
	// Moon SkillE
	{
		CSkillObjectSpawnerBase::SPAWNER_ORIGIN_DESC desc{};
		desc.iPoolLevelIndex = 0;
		desc.wstrSkillPoolTag = g_wszPool_MoonSkillE; // 스킬 poot에서 꺼내올 오브젝트 태그
		desc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Move_Straight) | ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		desc.fLifeTime = 3.5f;
		//desc.fSpeed = 50.f;

		if (FAILED(m_pGameInstance->Add_Prototype(0, g_wszSpawner_MoonSkillE,
			CSingleSkillSpawner::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// Moon SkillQ : sheild
	{
		CSkillObjectSpawnerBase::SPAWNER_ORIGIN_DESC desc{};
		desc.iPoolLevelIndex = 0;
		desc.wstrSkillPoolTag = g_wszPool_MoonSkillQSheild;
		desc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Follow_Owner) | ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		desc.fLifeTime = 12.f;

		if (FAILED(m_pGameInstance->Add_Prototype(0, g_wszSpawner_MoonSkillQ_Sheild,
			CSingleSkillSpawner::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	// Moon SkillQ : attack
	{
		CSkillObjectSpawnerBase::SPAWNER_ORIGIN_DESC desc{};
		desc.iPoolLevelIndex = 0;
		desc.wstrSkillPoolTag = g_wszPool_MoonSkillQAttack;
		desc.iSkillObjectFlags = ENUM_TO_UINT(ESkillObjectFlag::Life_Timer);
		desc.fLifeTime = 12.f;

		if (FAILED(m_pGameInstance->Add_Prototype(0, g_wszSpawner_MoonSkillQ_Attack,
			CSingleSkillSpawner::Create(m_pDevice, m_pDeviceContext, &desc))))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoader::Ready_AttackOverlap_PlayerMoon()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path FilePath = L"../../Resources/Data/AttackOverlapData/Moon2.json";
	vector<path> vecfiles;

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_AttackPresets()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::ATTACK_PRESET;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path rootFolder{ g_wszAttackPresetDataPath };

	if (std::filesystem::exists(rootFolder) == false)
	{
		MSG_BOX("CLoader::Ready_AttackPresets, RootFolder exist failed");
		return E_FAIL;
	}

	for (const auto& dirEntry : std::filesystem::directory_iterator(rootFolder))
	{
		if (dirEntry.is_directory() == false)
			continue;

		const std::filesystem::path categoryFolder = dirEntry.path();
		for (const auto& fileEntry : std::filesystem::directory_iterator(categoryFolder))
		{
			if (fileEntry.is_regular_file() == false)
				continue;

			const std::filesystem::path filePath = fileEntry.path();
			if (filePath.extension() != ".json")
				continue;

			string strFileKey = filePath.filename().stem().string();
			if (FAILED(Loading_File(iLevelID, eCategory, filePath)))
			{
				MSG_BOX("CLoader::Ready_AttackPresets, Load_File_Json failed");
				return E_FAIL;
			}

			if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, filePath.stem().string())))
				return E_FAIL;
		}
	}

	const auto& debugForDatas = m_pGameInstance->Get_AttackPresetsData_ForDebug();
	for (const auto& [key, value] : debugForDatas)
	{
		_uint iKey = key;
		const DTO::TAttackPreset_Data &data = value;
		iKey += 2;
	}
	return S_OK;
}

HRESULT CLoader::Ready_EffectEvent_AnimationData()
{
	_uint iLevelID = ENUM_TO_UINT(ELevelType::LOGO);
	DTO::ECategory eCategory = DTO::ECategory::EFFECTEVENT;
	std::filesystem::path FilePath = L"../../Resources/Data/EffectAnimationData/";
	if (std::filesystem::exists(FilePath))
	{
		for (auto iter : std::filesystem::directory_iterator(FilePath))
		{
			if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, iter.path())))
				return E_FAIL;

			if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, iter.path().stem().string())))
				return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CLoader::Ready_AttackOverlap_Monster_Dog()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path FilePath = L"../../Resources/Data/AttackOverlapData/Monster_Dog_Attack.json";
	vector<path> vecfiles;

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_AttackOverlap_Monster_Boomer()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path FilePath = L"../../Resources/Data/AttackOverlapData/Monster_Boomer_Attack.json";
	vector<path> vecfiles;

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Ready_AttackOverlap_Xibi()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path FilePath = L"../../Resources/Data/AttackOverlapData/Xibi_Attack.json";
	vector<path> vecfiles;

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

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

	Safe_Release(m_pBuilderSystem);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Super::Free();
}
