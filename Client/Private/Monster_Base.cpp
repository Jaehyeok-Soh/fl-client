#include "pch.h"
#include "Monster_Base.h"


#include "EngineConsole.h"

#include "Monster_Body_Base.h"
#include "Ray.h"

#include "MonsterControlContext.h"
#include "MonsterActionState.h"
#include "Model.h"
#include "ComputeShader.h"
#include "PhysicsCCT.h"
#include "PhysicsCollider.h"
#include "PhysicsAttackOverlap.h"

#include "GameInstance.h"



CMonster_Base::CMonster_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext), m_eMonsterType{ EMonster_Type::END}
{
	m_vecPartObjects.resize(Part::END, nullptr);
}

CMonster_Base::CMonster_Base(const CMonster_Base& rhs)
	: Super(rhs), m_eMonsterType{rhs.m_eMonsterType }
{
	m_vecPartObjects.resize(Part::END, nullptr);
}

HRESULT CMonster_Base::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Base::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Ability()))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Base::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	CGameInstance::GetInstance()->Add_Actor_Object(this);
	if (CMonsterActionState* pMonsterState = Get_Component<CMonsterActionState>())
		if (FAILED(pMonsterState->Awake(iCurrentLevelID)))
			return E_FAIL;

	if (FAILED(Get_Component<CMonsterActionState>()->Change_State(ENUM_TO_UINT(State::IDLE))))
		return E_FAIL;

	if (FAILED(Get_Component<CControlContext>()->Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CPhysicsCCT>()->Awake();

	if (CPhysicsAttackOverlap* attackOverlap = Get_Component<CPhysicsAttackOverlap>())
		attackOverlap->Awake();

	return S_OK;
}

void CMonster_Base::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Base::Update(const _float fTimeDelta)
{
	if (CMonsterActionState* pMonsterState = Get_Component<CMonsterActionState>())
	{
		pMonsterState->Update(fTimeDelta);
	}

	Super::Update(fTimeDelta);
}

void CMonster_Base::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (Get_Component <CPhysicsAttackOverlap>())
		Get_Component<CPhysicsAttackOverlap>()->Update(fTimeDelta);

	if (Get_Component<CPhysicsCCT>())
		Get_Component<CPhysicsCCT>()->Update(fTimeDelta);
}

void CMonster_Base::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsCCT>());
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsAttackOverlap>());
#endif // _DEBUG
}

HRESULT CMonster_Base::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

_int CMonster_Base::Get_AnimationIndex(const wstring& wstrName)
{
	if (CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(Part::BODY))
	{
		if (CModel* pModel = pBody->Get_Component<CModel>())
		{
			return pModel->Get_AnimationIndex(wstrName);
		}
		return -1;
	}

	return -1;
}

_wstring CMonster_Base::Get_AnimationName(_uint iAniIndex)
{
	if (CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(Part::BODY))
	{
		if (CModel* pModel = pBody->Get_Component<CModel>())
		{
			return pModel->Get_AnimationName(iAniIndex);
		}
		return L"";
	}

	return L"";
}

void CMonster_Base::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CMonster_Base::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO &tHitInfo)
{
	COLLIDED_DESC collidedDesc{};
	collidedDesc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	collidedDesc.iRequesterLayer = iMyColliderLayer;
	collidedDesc.iOtherLayer = iOtherLayer;
	collidedDesc.pRequester = this;
	collidedDesc.pOther = pOther;
	collidedDesc.tHitInfo = tHitInfo;

	m_pGameInstance->Push_CollidedData(collidedDesc);
}

void CMonster_Base::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CMonster_Base::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CMonster_Base::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

_bool CMonster_Base::On_Hit(const HIT_DESC& hitDesc)
{
	Get_Component<CMonsterControlContext>()->Set_HitDesc(hitDesc);

#ifdef _DEBUG
	wstring infoHeader(L"Monster Hit ");
	wstring infoSeparate(L": ");
	wstring infoContant = infoHeader
		+ infoSeparate
		+ Engine_Utils::ToWString(m_strName)
		+ infoSeparate
		+ std::to_wstring(Get_ID());

	CLOG_INFO(infoContant);
#endif // _DEBUG
	return true;
}

void CMonster_Base::Try_Attack(const HIT_DESC& hitDesc)
{
#ifdef _DEBUG
	wstring infoHeader(L"Monster Attack ");
	wstring infoSeparate(L": ");
	wstring infoContant = infoHeader
		+ infoSeparate
		+ Engine_Utils::ToWString(m_strName)
		+ infoSeparate
		+ std::to_wstring(Get_ID());

	CLOG_INFO(infoContant);
#endif // _DEBUG
}

HRESULT CMonster_Base::Ready_BaseStates()
{
	CMonsterActionState* pActionState = { nullptr };
	CModel* pModel = Get_Part<CMonster_Body_Base>(ENUM_TO_UINT(Part::BODY))->Get_Component<CModel>();
	if (!pModel)
		return E_FAIL;

	if (!(pActionState = Get_Component<CMonsterActionState>()))
		return E_FAIL;

	vector<_uint> vecChangeState_ByKey{};
	vecChangeState_ByKey.resize(State::END, State::END);

	TIME_COUNTER tKeyTimer = {};
	tKeyTimer.bCountTime = false;
	tKeyTimer.bTimeReset = false;

	return S_OK;
}

HRESULT CMonster_Base::Ready_PartObjects(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	{
		CMonster_Body_Base::MONSTERBODY_DESC bodyDesc = {};
		bodyDesc.pMatParent = &Get_Component<CTransform>()->Get_WorldMatrix();
		bodyDesc.iLevelIndex = pDesc->iLevelIndex;
		bodyDesc.wstrModelPrototypeTag = pDesc->wstrBodyModelTag;
		bodyDesc.spanBoneNames = pDesc->spanBoneNames;
		// TODO : 재혁아 이거 LevelID 바꿔야할수도있다 Static에 넣어두고 쓸까 ...?
		if (FAILED(Add_Part(Part::BODY,ENUM_TO_UINT(ELevelType::STATIC), pDesc->wstrPartBodyPrototypeTag, &bodyDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Base::Ready_Components(void* pArgs)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArgs);

	if (FAILED(Ready_CCT(pArgs)))
		return E_FAIL;

	if (FAILED(Ready_AttackOverlap(pDesc->wstrAttackOverlapPrototypeTag)))
			return E_FAIL;

	return S_OK;
}

HRESULT CMonster_Base::Ready_AttackOverlap(wstring prototypeName)
{
	if (prototypeName.size() > 0)
	{
		if (FAILED(Add_Component<CPhysicsAttackOverlap>(0, prototypeName, nullptr)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Base::Ready_CCT(void* pArgs)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArgs);
	pDesc->tCCTDesc.pOwner = this;
	pDesc->tCCTDesc.pOwnerMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();

	if (FAILED(Add_Component<CPhysicsCCT>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_CCT", &pDesc->tCCTDesc)))
		return E_FAIL;

	{
		// CCT, PhysicsCollider 세트
		PHYSICSCOLLIDER_DESC cloneDesc{};
		cloneDesc.eFilterLayer = pDesc->tCCTDesc.eFilterLayer;
		cloneDesc.iFilterMask = pDesc->tCCTDesc.iFilterMask;
		cloneDesc.bSetOnlyFilter = true;

		if (FAILED(Add_Component<CPhysicsCollider>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Physics_Collider", &cloneDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CMonster_Base::Create_Mosnter(EMonster_Type eCreateMonsterType, _uint iFindPrototypeLevelType, _uint iAddLevelType, CTransform::TRANSFORM_DESC* pTransformDesc)
{
	/* Monster Type별로 Batch */
	CGameObject* pResult{ nullptr };

	_uint	iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};

	CMonster_Base::MONSTER_DESC monsterDesc = {};
	monsterDesc.iLevelIndex = iAddLevelType;
	monsterDesc.pTransform_Desc = pTransformDesc;


	switch (eCreateMonsterType)
	{
	case EMonster_Type::Dog:
	{
		monsterDesc.wstrBodyModelTag				= g_wszMonster_Dog_Model_Prototype_Tag;
		monsterDesc.wstrPartBodyPrototypeTag		= g_wszMonster_Dog_Body_Prototype_Tag;
		monsterDesc.wstrAttackOverlapPrototypeTag	= g_wszMonster_Dog_AttackOverlap_Prototype_Tag;
		monsterDesc.wstrMonsterStateTag				= g_wszMonster_Dog_State_Tag;

		{
			PHYSICSCCT_DESC desc;
			desc.pOwner = nullptr;
			desc.bIsPlayer = false;
			desc.eType = EPhysicsCCTType::CAPSULE;
			desc.pOwnerMatrix = nullptr;
			desc.fRadius = 1.f;
			desc.fHeight = 0.1f;
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

			monsterDesc.tCCTDesc = desc;
		}
		wstrFindPrototypeName	= g_wszMonster_Dog_Prototype_Tag;
		wstrAddLayerName		= g_wszMonstereLayer;
	}
	break;
	case EMonster_Type::Shooter:
	{
		MSG_BOX(" Mosnter Shooter is None Ready Now Version ");
		return S_OK;
	}
	break;
	case EMonster_Type::Boomer:
	{
		////////////////////
		// MONSTER BOOMER //
		////////////////////
		monsterDesc.wstrPartBodyPrototypeTag		= g_wszMonster_Boomer_Body_Prototype_Tag;
		monsterDesc.wstrBodyModelTag				= g_wszMonster_Boomer_Model_Prototype_Tag;
		monsterDesc.wstrMonsterStateTag				= g_wszMonster_Boomer_State_Tag;

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

			monsterDesc.tCCTDesc = desc;
		}

		wstrFindPrototypeName		= g_wszMonster_Boomer_Prototype_Tag;
		wstrAddLayerName			= g_wszMonstereLayer;
	}
	break;
	case EMonster_Type::Xibi:
	{
		////////////////////
		//  BOSS Xibi  //
		////////////////////
		monsterDesc.wstrBodyModelTag			= g_wszBoss_Xibi_Model_Prototype_Tag;
		monsterDesc.wstrPartBodyPrototypeTag	= g_wszBoss_Xibi_Body_Prototype_Tag;
		monsterDesc.wstrMonsterStateTag			= g_wszBoss_Xibi_State_Tag;
		{
			PHYSICSCCT_DESC desc;
			desc.pOwner = nullptr;
			desc.bIsPlayer = false;
			desc.eType = EPhysicsCCTType::CAPSULE;
			desc.pOwnerMatrix = nullptr;
			desc.fRadius = 1.f;
			desc.fHeight = 1.f;
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

			monsterDesc.tCCTDesc = desc;
		}
		wstrFindPrototypeName		= g_wszBoss_Xibi_Prototype_Tag;
		wstrAddLayerName			= g_wszBossLayer;
	}
	break;
	default:
		break;
	}


	if (!(pResult = CGameInstance::GetInstance()->Add_GameObject(iFindPrototypeIndex, wstrFindPrototypeName,iAddLevelType, wstrAddLayerName, &monsterDesc)))
		return E_FAIL;

	return S_OK;
}

void CMonster_Base::Free()
{
	Super::Free();
}
