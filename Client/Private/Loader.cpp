#include "pch.h"
#include "Engine_Utils.h"
//=================
// Component
//=================
#include "PlayerControlContext.h"
#include "MonsterControlContext.h"
#include "MonoBehaviour.h"
#include "PlayerActionState.h"
#include "MonsterActionState.h"
#include "StatCom_Player.h"
#include "Collider.h"
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
#include "PhysicsCollider.h"
#include "SkillComp_MoonE.h"
#include "SkillComp_MoonQ.h"
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
#include "ColliderPart.h"
#include "Loader.h"
#include "Effect.h"
#include "EffectObject.h"
#include "Physics_LandScape.h" // physics test



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

/* --------------------- */
#include "Monster_Dummy.h" // test
#include "Monster_Dummy_Body.h" // test
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
// 텍스트 
#include "UIMenu_Text.h"
#include "UIPlayerStat_Text.h"
#include "UILoading_Text.h"
#include "UIMonsterStat_Text.h"
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

/* 첫 실행 떄 등록해야하는 것들 등록 */
HRESULT CLoader::Loading_For_Logo()
{
	m_fLoadingRatio = 0.f;

#pragma region PretransformMatrix
	Matrix matPreTransformScaleTest = Matrix::CreateScale(100.f, 100.f, 100.f);
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

		// Read Json
		{
			//if (FAILED(Loading_File(ENUM_TO_UINT(ELevelType::LOGO), DTO::ECategory::EFFECT, L"../../Resources/Data/EffectData/Attack_1.json")))
			//	return E_FAIL;
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

		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/blade")))
			return E_FAIL;

		if (FAILED(Make_StaticObject_Prototype(ELevelType::STATIC, L"../../Resources/Models/Effect_FBX/Circle")))
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
		desc.vecStageBoneIndices	= { 285,286,287,288,289,414,415,416 ,417,418,419 };

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

	// For.Prototype_Component_Model_Xibi
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformScale);
		desc.wstrModelFolderName = L"Xibi";
		desc.FStageBone = CModel::STAGEING_BONE::SB_SPCIPICBONE;
		desc.vecStageBoneIndices = { 75 };

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_Xibi", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For.Prototype_Component_Model_XibiWeapon
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformIdentity);
		desc.wstrModelFolderName = L"XibiWeapon";
		desc.FStageBone = CModel::STAGEING_BONE::SB_ZEROBONE;

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 2;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_XibiWeapon", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
	}

	// For.Prototype_Component_Model_Monster_Dog
	{
		CModel::MODEL_ORIGIN_DESC desc = {};
		desc.eType = EModelType::ANIM;
		desc.iPrototypeLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);
		desc.pMatPreTransform = &(matPreTransformIdentity);
		desc.wstrModelFolderName = L"Monster_Dog";
		desc.FStageBone = CModel::STAGEING_BONE::SB_ZEROBONE;
		desc.vecStageBoneIndices = { };

		CModel::DATA_ANIMCHANNEL tAniChannelData = {};
		tAniChannelData.iRootBoneIndex = 3;
		desc.pAniChannelData = &tAniChannelData;

		m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Model_Monster_Dog", CModel::Create(m_pDevice, m_pDeviceContext, &desc));
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
	// For. Prototype_Component_Bounds
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Bounds", CBounds::Create(m_pDevice, m_pDeviceContext));

	// For. Prototype_Component_Collider_SPHERE
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_VIBuffer_InstanceMesh", CInstanceMesh::Create(m_pDevice, m_pDeviceContext));

	/* player components */
	// For. Prototype_Component_Stat_Player
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Stat_Player", CStatCom_Player::Create());


	// For. Prototype_Component_ControlContext_Monster
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ControlContext_Monster", CMonsterControlContext::Create());
	// For. Prototype_Component_ActionState_Monster
	m_pGameInstance->Add_Prototype(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_ActionState_Monster", CMonsterActionState::Create(m_pDevice, m_pDeviceContext));

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
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Effect",					Effect::Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Effect_Parts",			CEffectObject::Create(m_pDevice, m_pDeviceContext));


#pragma region Map Object
		/* Map Object */
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_StaticObject", CStaticObject::		Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_LandScape",	CLandScape::		Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Bush",			CBush::				Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Grass",		CGrass::			Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Moss",			CMoss::				Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Tree",			CTree::				Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Vine",			CVine::				Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Rock",			CRock::				Create(m_pDevice, m_pDeviceContext));
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Water",		CWater::			Create(m_pDevice, m_pDeviceContext));
#pragma endregion

		/* Weapons */
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Part_Sword", CSword::Create(m_pDevice, m_pDeviceContext));

		// For. Prototype_GameObject_Monster_Dummy
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Monster_Dummy", CMonster_Dummy::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Monster_Dummy_Body
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Monster_Dummy_Body", CMonster_Dummy_Body::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Boss_Xibi
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Boss_Xibi", CBoss_Xibi::Create(m_pDevice, m_pDeviceContext));
		// For. Prototype_GameObject_Boss_XibiBody
		ADD_PROTOTYPE(ELevelType::LOGO, L"Prototype_GameObject_Boss_Xibi_Body", CBoss_Xibi_Body::Create(m_pDevice, m_pDeviceContext));
		/* Monster Object */
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Monster_Dummy", CMonster_Dummy::Create(m_pDevice, m_pDeviceContext));

		/* Monster Part Object */
		ADD_PROTOTYPE(ELevelType::STATIC, L"Prototype_GameObject_Monster_Dummy_Body", CMonster_Dummy_Body::Create(m_pDevice, m_pDeviceContext));
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
#pragma endregion

	m_isFinished = true;
	return S_OK;
}



HRESULT CLoader::Loading_For_Tutorial_Village()
{
	/* Tutorial Village */




	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Tutorial_Boss()
{
	/* Tutorial Boss */


	m_isFinished = true;
	return S_OK;
}

HRESULT CLoader::Loading_For_Square()
{
	/* Square */


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
	if (FAILED(Ready_AttackOverlap()))
		return E_FAIL;

	if (FAILED(Ready_AttackPresets()))
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

	return S_OK;
}

HRESULT CLoader::Ready_AttackOverlap_PlayerMoon()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::OVERLAP_SCRIPT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path FilePath = L"../../Resources/Data/AttackOverlapData/PlayerMoon_156_Animations_Save_Test_animTag.json";
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

HRESULT CLoader::Ready_EffectEvent()
{
	ELevelType eLevelType = ELevelType::LOGO;
	DTO::ECategory eCategory = DTO::ECategory::EFFECTEVENT;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);

	std::filesystem::path FilePath = L"../../Resources/Data/EffectAnimationData/PlayerMoon.json";
	vector<path> vecfiles;

	if (!std::filesystem::exists(FilePath))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Load_File_Json(iLevelID, eCategory, FilePath)))
		return E_FAIL;

	if (FAILED(m_pBuilderSystem->Build_File(iLevelID, eCategory, FilePath.stem().string())))
		return E_FAIL;

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
