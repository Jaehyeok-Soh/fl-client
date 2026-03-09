#include "pch.h"
#include "Client_EventDefine.h"
#include "Monster_Boomer.h"
#include "Monster_Body_Base.h"
#include "MonsterActionState.h"
#include "MonsterControlContext.h"
#include "StateBase_Monster.h"
#include "Model.h"
#include "PhysicsCCT.h"
#include "ComputeShader.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "MyStat.h"

CMonster_Boomer::CMonster_Boomer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{
	m_eMonsterType = EMonster_Type::Boomer;
}

CMonster_Boomer::CMonster_Boomer(const CMonster_Boomer& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_Boomer::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boomer::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Ability()))
		return E_FAIL;

	Set_Name("세비지 필토이드");

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_BaseStates()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Boomer::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;
	{
		UI_PREFAB_DATA tPrefabData = {};
		UI_NAMEPLATE_PREFAB_DATA Desc = {};
		Desc.pTarget = this;
		Desc.vOffset = Vec3{ 0.f, 2.5f, 0.f };
		tPrefabData.Data = Desc;
		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::MONSTER_NAMEPLATE, iCurrentLevelID, &tPrefabData);
	}
	{
		UI_PREFAB_DATA tPrefabData = {};
		UI_NAMEPLATE_PREFAB_DATA Desc = {};
		Desc.pTarget = this;
		tPrefabData.Data = Desc;
		CUI_Manager::GetInstance()->Request_Add_Prefab(iCurrentLevelID, EUIPrefabType::MINIMAP_MONSTER_ICON, iCurrentLevelID, &tPrefabData);
	}
	return S_OK;
}

void CMonster_Boomer::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Boomer::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMonster_Boomer::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMonster_Boomer::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CMonster_Boomer::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMonster_Boomer::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Boomer::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Boomer::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CMonster_Boomer::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CMonster_Boomer::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

_bool CMonster_Boomer::On_Hit(const HIT_DESC& hitDesc)
{
	_bool result = Super::On_Hit(hitDesc);
	
	auto myStat = Get_Component<CMyStat>();
	auto vHp = myStat->Get_Stat_Vec2(CMyStat::STAT_TYPE::HP);
	if (vHp.x <= 0)
	{
		Get_Component<CMonsterControlContext>()->Set_Dead();
		m_pGameInstance->Broadcast<MONSTER_DEAD_EVENT_START>(this);
	}

	/*이펙트를 생성하기 위해서*/
	if (Engine_Utils::Has_Flag(hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::MOON)))
	{
	/*	hitDesc.attackDesc.iAttackerLayer = PHYSICSFILTERGROUP::ATTACK_PROJECTTILE;*/
		EFFECT_SPAWN_DESC Desc = {};
		//Matrix OffsetMatrix = Matrix::CreateTranslation(Vec3(0.f, 0.5f, 0.5f));
		Matrix WorldMatrix = Get_Component<CTransform>()->Get_WorldMatrix();

		Vec3 vScale, vPos;
		Quat vQuat;
		WorldMatrix.Decompose(vScale, vQuat, vPos);

		Desc.matWorld = Matrix::CreateFromQuaternion(vQuat) * Matrix::CreateTranslation(hitDesc.vHitPoint);
		Desc.iSimulationType = (int)EFFECT_SPAWN_DESC::E_VFX_SIMULTYPE::VFX_WORLD;
		m_pGameInstance->Request_Effect("VFX_Sword_Hit", Desc);
	}

	return result;
}

void CMonster_Boomer::Try_Attack(const HIT_DESC& hitDesc)
{
	Super::Try_Attack(hitDesc);
}

HRESULT CMonster_Boomer::Ready_Ability()
{
	// stat
	{
		CMyStat::STAT_DESC desc = {};
		desc.fMaxHp = 600.f;
		desc.fDefense = 100.f;
		desc.FStatFlags = CMyStat::StatFlags::HpUpdate | CMyStat::StatFlags::DefenseUpdtae;

		if (FAILED(Add_Component<CMyStat>(0/* STATIC */, L"Prototype_Component_Stat", &desc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Boomer::Ready_BaseStates()
{
	CMonsterActionState* pActionState = { nullptr };
	CModel* pModel = Get_Part<CMonster_Body_Base>(Part::BODY)->Get_Component<CModel>();
	if (!pModel)
		return E_FAIL;

	if (!(pActionState = Get_Component<CMonsterActionState>()))
		return E_FAIL;

	TIME_COUNTER tStateLifeTime = {};
	TIME_COUNTER tStateCoolDownTime = {};

	return S_OK;
}

HRESULT CMonster_Boomer::Ready_PartObjects()
{
	return S_OK;
}

HRESULT CMonster_Boomer::Ready_Components(void* pArg)
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
	desc.fMeleeRange = 2.f;
	desc.fAttackRange = 6.f;
	desc.fCloseRange = 1.f;
	desc.fDetectionRange = 15.f;
	desc.fSpeed = 1.f;
	//desc.iSkillCount;
	//desc.vecSkillRange;

	if (FAILED(Add_Component<CMonsterControlContext>(0 /*static*/, L"Prototype_Component_ControlContext_Monster", &desc)))
		return E_FAIL;

	return S_OK;
}

CMonster_Base::MONSTER_DESC CMonster_Boomer::Get_PreSetDesc(_uint iLevelId)
{
	CMonster_Base::MONSTER_DESC monsterDesc = {};
	monsterDesc.iLevelIndex = iLevelId;

	monsterDesc.wstrPartBodyPrototypeTag = g_wszMonster_Boomer_Body_Prototype_Tag;
	monsterDesc.wstrBodyModelTag = g_wszMonster_Boomer_Model_Prototype_Tag;
	monsterDesc.wstrAttackOverlapPrototypeTag = g_wszMonster_Boomer_AttackOverlap_Prototype_Tag;
	monsterDesc.wstrMonsterStateTag = g_wszMonster_Boomer_State_Tag;

	{
		PHYSICSCCT_DESC desc;
		desc.pOwner = nullptr;
		desc.bIsPlayer = false;
		desc.eType = EPhysicsCCTType::CAPSULE;
		desc.pOwnerMatrix = nullptr;
		desc.fRadius = 1.f;
		desc.fHeight = 1.5f;
		desc.vExtens = { 2.f, 2.f, 2.f };

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
			| PHYSICSFILTERGROUP::Enum::OBJECT2;

		desc.bGravity = { true };
		desc.fGravity = { -35.f };
		desc.MSpeed = { 0.f, 3.f };
		desc.MAccelRate = { 0.f, 10.f };
		desc.MDeAccelRate = { 0.f, 10.f };

		monsterDesc.tCCTDesc = desc;
	}

	return monsterDesc;
}

CMonster_Boomer* CMonster_Boomer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMonster_Boomer* pInsatnce = new CMonster_Boomer(pDevice, pDeviceContext);
	if (FAILED(pInsatnce->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_Boomer::Create, Failed");
		Safe_Release(pInsatnce);
	}

	return pInsatnce;
}

CGameObject* CMonster_Boomer::Clone(void* pArg)
{
	CMonster_Boomer* pClone = new CMonster_Boomer(*this);
	if (FAILED(pClone->Initialize(pArg)))
	{
		MSG_BOX("CMonster_Boomer::Clone, Failed");
		Safe_Release(pClone);
	}
	return pClone;
}

void CMonster_Boomer::Free()
{
	Super::Free();
}
