#include "pch.h"
#include "Monster_Base.h"

#include "EngineConsole.h"

#include "Monster_Body_Base.h"

#include "MonsterControlContext.h"
#include "MonsterActionState.h"
#include "Model.h"
#include "ComputeShader.h"
#include "PhysicsCCT.h"
#include "PhysicsCollider.h"
#include "PhysicsAttackOverlap.h"
#include "EffectHandler.h"
#include "RenderFx.h"
#include "UIMinimap_Manager.h"
#include "UI_Manager.h"
#include "GameInstance.h"

#include "MyStat.h"

#include "Monster_Dog.h"
#include "Monster_Boomer.h"

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

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::MONSTER_DEFAULT);

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

	if (FAILED(Ready_EffectHandler(pArg)))
		return E_FAIL;

	//if (FAILED(Ready_Ability()))
	//	return E_FAIL;

	Get_Component<CPhysicsAttackOverlap>()->Bind_Events();
	m_pEffectHandler->Setup_ForOwner(this, Get_Part<CMonster_Body_Base>(Part::BODY)->Get_Component<CModel>());

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

	Get_Component<CPhysicsCCT>()->Ready_Position();
	
	return S_OK;
}

void CMonster_Base::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMonster_Base::Update(const _float fTimeDelta)
{
	if (CMonsterControlContext* pMonsterControlContext = Get_Component<CMonsterControlContext>())
		pMonsterControlContext->Update_RuntimeDesc(fTimeDelta);

	if (CMonsterActionState* pMonsterState = Get_Component<CMonsterActionState>())
		pMonsterState->Update(fTimeDelta);

	if (m_pEffectHandler)
		m_pEffectHandler->Update(fTimeDelta);

	Super::Update(fTimeDelta);
}

void CMonster_Base::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (Get_Component <CPhysicsAttackOverlap>())
		Get_Component<CPhysicsAttackOverlap>()->Update(fTimeDelta);
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

void CMonster_Base::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	COLLIDED_DESC collidedDesc{};
	collidedDesc.iCollisionType = COLLISIONEVENT::ON_TRIGGER_ENTER;
	collidedDesc.iRequesterLayer = iMyColliderLayer;
	collidedDesc.iOtherLayer = iOtherLayer;
	collidedDesc.pRequester = this;
	collidedDesc.pOther = pOther;
	collidedDesc.tHitInfo = tHitInfo;

	m_pGameInstance->Push_CollidedData(collidedDesc);
}

void CMonster_Base::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

_bool CMonster_Base::On_Hit(const HIT_DESC& hitDesc)
{
	Get_Component<CMonsterControlContext>()->Set_HitDesc(hitDesc);

	_uint iDamageFlag = hitDesc.iDamageFlag;
	// 살아 있을때만 피격 처리를 하겠다
	if (IsAlive())
	{
		if (Engine_Utils::Has_Flag(iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::MOON)))
		{
			OnHit_PlayerMoon(hitDesc);
		}
	}

	// stat 관리
	auto myStat = Get_Component<CMyStat>();
	if (myStat)
	{
		myStat->Add_Health(-hitDesc.fFinalDamage);

		auto vHp = myStat->Get_Stat_Vec2(CMyStat::STAT_TYPE::HP);
		if (vHp.x <= 0)
		{
			DTO::QUEST_EVENT_SIGNATURE callback;
			callback.eEvent = DTO::EQuestEvent::MONSTER_KILL;
			callback.eTargetType = m_eObject_Enum_Tag;
			callback.iCount = 1;
			CGameInstance::GetInstance()->Broadcast<QUEST_NOTIFY>(callback);

			Get_Component<CMonsterControlContext>()->Set_CCT_Collision_Disable();
			CUIMinimap_Manager::GetInstance()->Delete_Ranged_Object(this);
			Set_Dying();
		}
	}

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

void CMonster_Base::Set_RootMotion_Apply(_bool bApply)
{
	Get_Part<CMonster_Body_Base>(Part::BODY)->Get_Component<CModel>()->Set_CurAnimation_RootApply(bApply);
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

HRESULT CMonster_Base::Ready_EffectHandler(void* pArg)
{
	MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(pArg);

	wstring NameTag = pDesc->wstrBodyModelTag;
	Engine_Utils::Replace(NameTag, L"Prototype_Component_Model_", L"");

	if (FAILED(Add_Component<CEffectHandler>(/*Static*/0, L"Prototype_Component_EffectHandler_" + NameTag, nullptr)))
		return E_FAIL;

	m_pEffectHandler = Get_Component<CEffectHandler>();
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

void CMonster_Base::OnHit_PlayerMoon(const HIT_DESC& hitDesc)
{
	_uint iDamageFlag = hitDesc.iDamageFlag;
	_bool bCritical = false;

	UI_PREFAB_DATA tPrefabData = {};
	UI_DAMAGEFONT_PREFAB_DATA Desc = {};
	Desc.iDamage = static_cast<_uint>(hitDesc.fFinalDamage); // 데미지 폰트에 뜰 숫자 // 플레이어 공격력 // 랜덤은 보여주기용
	Desc.vFontColor = Vec4{ 1.f, 0.95f, 0.47f, 1.f }; // 데미지 폰트 색 // 캐릭터 고유 색
	Desc.vHitPos = hitDesc.vHitPoint; // 데미지 폰트를 띄울 World 위치 // 
	Desc.vRandOffset = Vec3{
		m_pGameInstance->Rand_Float(-1.f, 1.f),
		m_pGameInstance->Rand_Float(-1.f, 1.f),
		m_pGameInstance->Rand_Float(-1.f, 1.f) }; // 랜덤 오프셋 // 더 커지면 이상함

	/*이펙트를 생성하기 위해서*/
	//if (Engine_Utils::Has_Flag(hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::MOON)))
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

		if (Engine_Utils::Has_Flag(hitDesc.iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::CRITICAL)))
		{
			m_pGameInstance->Request_Effect("VFX_Critical_Hit", Desc);
		}


		else if (hitDesc.attackDesc.iAttackerLayer == PHYSICSFILTERGROUP::ATTACK_PROJECTTILE)
		{
			m_pGameInstance->Request_Effect("VFX_Bullet_Hit", Desc);
		}

		else
		{
			m_pGameInstance->Request_Effect("VFX_Sword_Hit", Desc);
		}
	}

	/* 폰트 추가 정보 */
	{
		// skill : hit point가 없어서 positoin 값 기준으로 데미지 폰트 띄움
		if (Engine_Utils::Has_Flag(iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::SKILLQ) | ENUM_TO_UINT(EPlayerAttackFlag::SKILLE)))
		{
			Vec3 vPos = Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
			vPos.y += 0.5f;

			Desc.vHitPos = vPos;
		}

		// critical 여부 판단
		if (Engine_Utils::Has_Flag(iDamageFlag, ENUM_TO_UINT(EPlayerAttackFlag::CRITICAL)))
		{
			bCritical = true;
		}

		////// UI에게 폰트 호출 //////
		if (bCritical)
		{
			tPrefabData.Data = Desc;
			CUI_Manager::GetInstance()->Request_Add_Prefab(
				m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_CRITICAL, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
		}

		else
		{
			tPrefabData.Data = Desc;
			CUI_Manager::GetInstance()->Request_Add_Prefab(
				m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_COMMON, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
		}
	}

	// Shake & Emissive
	if (CMonster_Body_Base* pBody = Get_Part<CMonster_Body_Base>(Part::Enum::BODY))
	{
		CRenderFx* pRenderFx = pBody->Get_Component<CRenderFx>();
		pRenderFx->Play_Shake(0.35f);
		pRenderFx->Play_EmissivePulse(0.05f, 0.08f, 0.18f);

	}
}

void CMonster_Base::SetSpawnPos(CTransform::TRANSFORM_DESC tTransformDesc)
{
	{
		Matrix transform = tTransformDesc.TranslationMatrix;
		Vec3 pos = transform.Translation();
		pos.x += m_pGameInstance->Rand_Float(-2.5f, 2.5f);
		pos.y += 3.f;
		pos.z += m_pGameInstance->Rand_Float(-2.5f, 2.5f);

		tTransformDesc.TranslationMatrix = Matrix::CreateTranslation(pos);
	}

	Matrix matWorld = tTransformDesc.ScaleMatrix * tTransformDesc.RotationMatrix * tTransformDesc.TranslationMatrix;

	Get_Component<CTransform>()->Set_WorldMatrix(matWorld);

	Get_Component<CPhysicsCCT>()->SetFootPosition(matWorld.Translation());
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
		monsterDesc = CMonster_Dog::Get_PreSetDesc(monsterDesc.iLevelIndex);
		monsterDesc.iLevelIndex = iAddLevelType;
		monsterDesc.pTransform_Desc = pTransformDesc;

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
		monsterDesc = CMonster_Boomer::Get_PreSetDesc(monsterDesc.iLevelIndex);
		monsterDesc.iLevelIndex = iAddLevelType;
		monsterDesc.pTransform_Desc = pTransformDesc;

		wstrFindPrototypeName		= g_wszMonster_Boomer_Prototype_Tag;
		wstrAddLayerName			= g_wszMonstereLayer;
	}
	break;
	case EMonster_Type::Xibi:
	{
		/////////////////
		//  BOSS Xibi  //
		/////////////////
		monsterDesc.wstrBodyModelTag				= g_wszBoss_Xibi_Model_Prototype_Tag;
		monsterDesc.wstrPartBodyPrototypeTag		= g_wszBoss_Xibi_Body_Prototype_Tag;
		monsterDesc.wstrAttackOverlapPrototypeTag	= g_wszBoss_Xibi_AttackOverlap_Prototype_Tag;
		monsterDesc.wstrMonsterStateTag				= g_wszBoss_Xibi_State_Tag;
		{
			PHYSICSCCT_DESC desc;
			desc.pOwner = nullptr;
			desc.bIsPlayer = false;
			desc.eType = EPhysicsCCTType::CAPSULE;
			desc.pOwnerMatrix = nullptr;
			desc.fRadius = 0.7f;
			desc.fHeight = 0.7f;
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
				| PHYSICSFILTERGROUP::Enum::OBJECT2
				| PHYSICSFILTERGROUP::Enum::DETECT_MONSTER
				| PHYSICSFILTERGROUP::Enum::NPC;

			desc.bGravity = { true };
			desc.fGravity = { -35.f };
			desc.MSpeed = { 0.f, 1.f };
			desc.MAccelRate = { 0.f, 10.f };
			desc.MDeAccelRate = { 0.f, 10.f };

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
