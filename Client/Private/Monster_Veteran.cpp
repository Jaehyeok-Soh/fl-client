#include "pch.h"
#include "Client_EventDefine.h"
#include "Monster_Veteran.h"
#include "Monster_Body_Base.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "StateBase_Monster.h"
#include "Model.h"
#include "PhysicsCCT.h"
#include "ComputeShader.h"
#include "UI_Manager.h"
#include "UIIcon_Component.h"
#include "GameInstance.h"
#include "MyStat.h"
#include "Monster_Veteran_Body.h"
#include "CameraEventBinder.h"
#include "Monster_GimmikController.h"
#include "SingleSkillSpawner.h"
#include "SoundEventBinder.h"

CMonster_Veteran::CMonster_Veteran(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_eMonsterType = EMonster_Type::Veteran;
}

CMonster_Veteran::CMonster_Veteran(const CMonster_Veteran& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Veteran::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::MONSTER_ELITE_VETERAN);

	return S_OK;
}

HRESULT CMonster_Veteran::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Ability()))
		return E_FAIL;

	Set_Name("천번 찔린 베테랑");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	if (FAILED(Ready_CameraEvent()))
		return E_FAIL;

	if (FAILED(Ready_SkillSpawner()))
		return E_FAIL;

	if (FAILED(Ready_SoundHandler()))
		return E_FAIL;

	m_arrHitSoundHash[HitSoundHashNum::HURT01_VO] = TO_HASH("sfx_boss_Lizhanzhe_hit_r");
	m_arrHitSoundHash[HitSoundHashNum::HURT02_VO] = TO_HASH("sfx_boss_Lizhanzhe_hitLight_vo_r");

	return S_OK;
}

HRESULT CMonster_Veteran::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	{
		Get_Component<CMyStat>()->Set_Stat(CMyStat::STAT_TYPE::HP, 160000.f); // 시연회 때 35만으로 설정하면 좋을 듯.
	}

	Ready_StateIndexForDirecting();

	if (FAILED(Get_Component<CMonster_GimmikController>()->Awake(iCurrentLevelID)))
		return E_FAIL;

	m_pGameInstance->StopBGM_FadeOut(1.f);
	m_pGameInstance->PlayBGM_FadeIn(0, TO_HASH("ELITE_BOSS_BGM"), 0.5f, 1.f);

	{
		UI_PREFAB_DATA ePrefabData = {};
		UI_BOSS_NAMEPLATE_PREFAB_DATA Desc = {};
		Desc.pTarget = this;
		ePrefabData.Data = Desc;
		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::BOSS_NAMEPLATE, iCurrentLevelID, &ePrefabData);
	}

	return S_OK;
}

void CMonster_Veteran::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Veteran::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	Get_Component<CMonster_GimmikController>()->Update(fTimeDelta);
}

void CMonster_Veteran::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Veteran::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CMonster_Veteran::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Veteran::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Veteran::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Veteran::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Veteran::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Veteran::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CMonster_Veteran::On_Hit(const HIT_DESC& hitDesc)
{
	_bool result = Super::On_Hit(hitDesc);

	auto myStat = Get_Component<CMyStat>();
	auto vHp = myStat->Get_Stat_Vec2(CMyStat::STAT_TYPE::HP);

	if (vHp.x <= 0)
	{
		m_pGameInstance->StopBGM_FadeOut(1.f);
		m_pGameInstance->PlayBGM_FadeIn(0, TO_HASH("KUANGKENG_BGM"), 0.5f, 1.f);
		m_pGameInstance->Broadcast<MONSTER_DEAD_EVENT_START>(this);
		m_pGameInstance->Broadcast<BOSS_UI_OFF>();

	}
	else
	{
		m_pGameInstance->Play_OneShot(0 /* static */, m_arrHitSoundHash[HitSoundHashNum::HURT01_VO], 0.5f, 1.f, false);
		m_pGameInstance->Play_OneShot(0 /* static */, m_arrHitSoundHash[HitSoundHashNum::HURT02_VO], 0.5f, 1.f, false);
	}

	return result;
}

void CMonster_Veteran::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);
}

HRESULT CMonster_Veteran::Ready_Ability()
{
	// stat
	{
		CMyStat::STAT_DESC desc = {};
		desc.fMaxHp = 200000.f; //수정하기
		desc.fDefense = 0.f;
		desc.FStatFlags = CMyStat::StatFlags::HpUpdate | CMyStat::StatFlags::DefenseUpdtae;

		if (FAILED(Add_Component<CMyStat>(0/* STATIC */, L"Prototype_Component_Stat", &desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Veteran::Ready_BaseStates()
{
	return S_OK;
}

HRESULT CMonster_Veteran::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CMonster_Veteran::Ready_Components(void* pArg)
{
	// TODO : Dummy나 파생클래스의 Desc가 생긴다면 수정해야함
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	//typedef struct tagMonsterControlContextDesc
	//{
	//	_float fMeleeRange = {};
	//	_float fAttackRange = {};
	//	_float fCloseRange = {};
	//	_float fDetectionRange = {};
	//	_float fSpeed = {};
	//	_int iSkillCount = { -1 };
	//	vector<_int> vecSkillRange;
	//}MONSTER_CONTROLCONTEXT_DESC;
	{
		CMonsterActionState::MONSTERACTIONSTATE_DESC desc = {};
		desc.pOwnerModel = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
		desc.pOwnerAnimECS = static_cast<CComputeShader*>(Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Script_Component(TEXT("ComputeShader_AnimE")));
		desc.wstrMonsterStateTag = pDesc->wstrMonsterStateTag;
		desc.iLevelIndex = pDesc->iLevelIndex;
		if (FAILED(Add_Component<CMonsterActionState>(0, L"Prototype_Component_ActionState_Monster", &desc)))
			return E_FAIL;
	}

	CMonsterControlContext::MONSTER_CONTROLCONTEXT_DESC desc{};
	desc.fMeleeRange = 5.f;
	desc.fAttackRange = 30.f;
	desc.fCloseRange = 1.f;
	desc.fDetectionRange = 100.f;
	desc.fSpeed = 1.f;
	//desc.iSkillCount;
	//desc.vecSkillRange;

	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", &desc)))
		return E_FAIL;
	{
		CUIIcon_Component::UI_ICON_COMP_DESC Desc = {};
		Desc.wstrIconTextureTag = L"Texture_T_Battle_HudElite";
		if (FAILED(Add_Script_Component(L"UIIconComp", L"Prototype_ScriptComponent_UIIcon", &Desc)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CMonster_Veteran::Ready_CameraEvent()
{
	_uint iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
	CMonster_Veteran_Body* pBody = Get_Part<CMonster_Veteran_Body>(ENUM_TO_UINT(Part::BODY));
	if (pBody == nullptr)
		return E_FAIL;
	CModel* pAnimModel = pBody->Get_Component<CModel>();
	if (pAnimModel == nullptr)
		return E_FAIL;
	// 내부에서 Add_Component 해줌
	CCameraEventBinder* pResult = CCameraEventBinder::Create(iLevelID, this, pAnimModel, L"../../Resources/Data/CameraAnimationData/Monster_Veteran.json");
	if (pResult == nullptr)
		return E_FAIL;
	Safe_Release(pResult);

	return S_OK;
}

HRESULT CMonster_Veteran::Ready_SkillSpawner()
{
	_uint iLevelId = m_pGameInstance->Get_CurrentLevelIndex();

	CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY));
	if (pBody == nullptr)
		return E_FAIL;

	// GimmikController
	{
		CMonster_GimmikController::GIMMIKCTRL_DESC desc{};
		desc.pOwnerModel = pBody->Get_Component<CModel>();
		if (FAILED(Add_Component<CMonster_GimmikController>(0 /*static*/, L"Prototype_Component_Monster_GimmikController", &desc)))
			return E_FAIL;
	}

	// Oneshot
	{
		CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
		desc.iLevelIndex = 0; /*static*/
		desc.iSpawnLevelIndex = iLevelId; /*static*/

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			0 /*static*/, g_wszSpawner_MonsterVeteranOneshotCircleProjectile, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		Get_Component<CMonster_GimmikController>()->AddSkillSpawner(static_cast<CSkillObjectSpawnerBase*>(pResult));
	}

	return S_OK;
}

HRESULT CMonster_Veteran::Ready_SoundHandler()
{
	_uint iLevelID = m_pGameInstance->Get_CurrentLevelIndex();
	CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY));
	if (pBody == nullptr)
		return E_FAIL;
	CModel* pAnimModel = pBody->Get_Component<CModel>();
	if (pAnimModel == nullptr)
		return E_FAIL;
	// 내부에서 Add_Component 해줌
	CSoundEventBinder* pResult = CSoundEventBinder::Create(iLevelID, this, pAnimModel, L"../../Resources/Data/SoundAnimationData/Monster_Veteran.json");
	if (pResult == nullptr)
		return E_FAIL;
	Safe_Release(pResult);
	return S_OK;
}

HRESULT CMonster_Veteran::Ready_StateIndexForDirecting()
{
	CMonsterActionState* pActionState = Get_Component<CMonsterActionState>();
	if (pActionState == nullptr)
		return E_FAIL;

	_uint idleIndex = { 0 };

	auto setStateIndex = [&](_uint& iStateIndex, const string& strStateName)->_bool
		{
			_uint iIndex = pActionState->Get_StateIndex(strStateName);
			if (iIndex < 0)
				return false;
			iStateIndex = iIndex;
			return true;
		};

	if (setStateIndex(idleIndex, "Idle") == false)
		return E_FAIL;

	Change_State_ForDirecting(idleIndex);

	return S_OK;
}

HRESULT CMonster_Veteran::Change_State_ForDirecting(_int iStateIdx)
{
	CActionState* pActionState = Get_Component<CActionState>();
	if (pActionState == nullptr)
		return E_FAIL;

	if (FAILED(pActionState->Change_State(iStateIdx, true)))
		return E_FAIL;

	return S_OK;
}

CMonster_Base::MONSTER_DESC CMonster_Veteran::Get_PreSetDesc(_uint iLevelId)
{
	CMonster_Base::MONSTER_DESC monsterDesc = {};
	monsterDesc.iLevelIndex = iLevelId;
	monsterDesc.pTransform_Desc = nullptr;

	monsterDesc.wstrBodyModelTag = g_wszMonster_Veteran_Model_Prototype_Tag;
	monsterDesc.wstrPartBodyPrototypeTag = g_wszMonster_Veteran_Body_Prototype_Tag;
	monsterDesc.wstrAttackOverlapPrototypeTag = g_wszMonster_Veteran_AttackOverlap_Prototype_Tag;
	monsterDesc.wstrMonsterStateTag = g_wszMonster_Veteran_State_Tag;

	{
		PHYSICSCCT_DESC desc;
		desc.pOwner = nullptr;
		desc.bIsPlayer = false;
		desc.eType = EPhysicsCCTType::CAPSULE;
		desc.pOwnerMatrix = nullptr;
		desc.fRadius = 2.f;
		desc.fHeight = 3.f;
		desc.vExtens = { 2.f, 2.f, 2.f };

		desc.fContactOffset = 0.01f;
		desc.fStepOffset = 0.2f;
		desc.fSlopeLimit = 0.7f;

		desc.vLocalOffset = {};
		desc.vWorldOffset = {};

		desc.bIsHover = { false };
		desc.fHoverOffset = { 2.f };

		PHYSICSMATERIAL_DESC mtrlDesc{};
		mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
		desc.tMaterial = mtrlDesc;

		desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::MONSTER;
		desc.iFilterMask =
			PHYSICSFILTERGROUP::Enum::MONSTER
			| PHYSICSFILTERGROUP::Enum::PLAYER
			| PHYSICSFILTERGROUP::Enum::ATTACK
			| PHYSICSFILTERGROUP::Enum::ATTACK_PROJECTTILE
			| PHYSICSFILTERGROUP::Enum::SKILL
			| PHYSICSFILTERGROUP::Enum::SKILL_PROJECTTILE
			| PHYSICSFILTERGROUP::Enum::MAP
			| PHYSICSFILTERGROUP::Enum::OBJECT1
			| PHYSICSFILTERGROUP::Enum::OBJECT2
			| PHYSICSFILTERGROUP::Enum::DETECT_MONSTER;

		desc.bGravity = { true };
		desc.fGravity = { -35.f };
		desc.MSpeed = { 0.f, 3.f };
		desc.MAccelRate = { 0.f, 10.f };
		desc.MDeAccelRate = { 0.f, 10.f };

		monsterDesc.tCCTDesc = desc;
	}

	return monsterDesc;
}

CMonster_Veteran* CMonster_Veteran::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Veteran* pInsatnce = new CMonster_Veteran(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Veteran::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CMonster_Veteran::Clone(void* pArg)
{
	CMonster_Veteran* pClone = new CMonster_Veteran(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Veteran::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonster_Veteran::Free()
{
	Super::Free();
}
