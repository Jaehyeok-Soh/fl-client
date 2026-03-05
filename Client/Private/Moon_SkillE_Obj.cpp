#include "pch.h"
#include "Moon_SkillE_Obj.h"
#include "EffectHandler.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"

// manager
#include "UI_Manager.h"
#include "GameInstance.h"

CMoon_SkillE_Obj::CMoon_SkillE_Obj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CMoon_SkillE_Obj::CMoon_SkillE_Obj(const CMoon_SkillE_Obj& rhs)
	: Super(rhs)
{

}

HRESULT CMoon_SkillE_Obj::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoon_SkillE_Obj::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();
	Get_Component<CEffectHandler>()->Setup_ForOwner(this);
	return S_OK;
}

HRESULT CMoon_SkillE_Obj::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CMoon_SkillE_Obj::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CMoon_SkillE_Obj::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CMoon_SkillE_Obj::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CMoon_SkillE_Obj::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

HRESULT CMoon_SkillE_Obj::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CMoon_SkillE_Obj::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

void CMoon_SkillE_Obj::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CMoon_SkillE_Obj::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CMoon_SkillE_Obj::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyColliderLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	//desc.tHitInfo = tHitInfo;

	m_pGameInstance->Push_CollidedData(desc);
}

_bool CMoon_SkillE_Obj::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CMoon_SkillE_Obj::Try_Attack(const HIT_DESC& hitDesc)
{
	// damage 폰트 : iDamageFlag에 따라 크리티컬 || 일반 판정

	// 일반 공격 데미지 폰트
	{
		UI_PREFAB_DATA tPrefabData = {};
		tPrefabData.DamageFontData.iDamage = hitDesc.fFinalDamage; // 데미지 폰트에 뜰 숫자 // 플레이어 공격력 // 랜덤은 보여주기용
		tPrefabData.DamageFontData.vFontColor = Vec4{ 1.f, 0.95f, 0.47f, 1.f }; // 데미지 폰트 색 // 캐릭터 고유 색
		tPrefabData.DamageFontData.vHitPos = hitDesc.vHitPoint; // 데미지 폰트를 띄울 World 위치 // 
		tPrefabData.DamageFontData.vRandOffset = Vec3{
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(-1.f, 1.f) }; // 랜덤 오프셋 // 더 커지면 이상함

		CUI_Manager::GetInstance()->Request_Add_Prefab(
			m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_COMMON, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
	}
}

HRESULT CMoon_SkillE_Obj::Ready_Components()
{
	// For. Component_EffectHandler
	{
		CEffectHandler::ANIM_EFFECT_HANDLER_DESC Desc{};
		CEffectHandler::STATE_VFX_DESC SkillDesc{};

		// SPAWN EFFECT
		{
			SkillDesc.EffectPrefabTag = "PlayerMoon_ESkillObject";
			SkillDesc.pParentTransformMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
			SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_LOCAL }; 
			SkillDesc.bFollowBone = { false };
			SkillDesc.iBoneIndex = -1;
			SkillDesc.vOffSet = { 0.f,0.f,0.f };
			SkillDesc.vRotation = { Vec3::Zero };
			Desc.eType = CEffectHandler::E_HANDLER_TYPE::SKILL_OBJ;
			Desc.mEffectState.emplace(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_SPAWN, SkillDesc);
		}

		// Distory EFFECT
		{
			SkillDesc.EffectPrefabTag = "";
			SkillDesc.pParentTransformMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
			SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_LOCAL };
			SkillDesc.bFollowBone = { false };
			SkillDesc.iBoneIndex = -1;
			SkillDesc.vOffSet = { Vec3::Zero };
			SkillDesc.vRotation = { Vec3::Zero };
			Desc.eType = CEffectHandler::E_HANDLER_TYPE::SKILL_OBJ;
			Desc.mEffectState.emplace(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_DESTROY, SkillDesc);
		}

		if (FAILED(Add_Component<CEffectHandler>(/*Static*/ 0, L"Prototype_Component_EffectHandler_SkillObject", &Desc)))
			return E_FAIL;
	}

	// For. Component_PhysicsCollider
	{
		/* 피직스 콜라이더 */
		{
			PHYSICSCOLLIDER_DESC cloneDesc{};
			cloneDesc.eShape = EPhysicsShape::BOX;
			cloneDesc.eFilterLayer = tagPhysicsFilterGroup::ATTACK;
			cloneDesc.iFilterMask =
			{
				PHYSICSFILTERGROUP::Enum::MONSTER
				| PHYSICSFILTERGROUP::Enum::OBJECT1
				| PHYSICSFILTERGROUP::Enum::OBJECT2
			};
			cloneDesc.bIsTrigger		= true;
			cloneDesc.bSetOnlyFilter	= false;
			cloneDesc.bIsActive			= true;
			cloneDesc.vCenter			= { 0.f, 0.3f, 0.f };
			cloneDesc.vExtents			= { 0.3f, 1.f,0.3f };
			cloneDesc.strAttackPresetTag = "Xibi_Circle";
			PHYSICSMATERIAL_DESC mtrlDesc{};
			mtrlDesc.eMaterial			= EPhysicsMaterial::CONCRETE;
			cloneDesc.tMaterial			= mtrlDesc;
			if (FAILED(Add_Component<CPhysicsCollider>(/* static */ 0, L"Prototype_Component_Physics_Collider", &cloneDesc)))
				return E_FAIL;
		}
	}

	// For. Component_PhysicsRigidBody
	{
		PHYSICSRIGIDBODY_DESC desc{};
		desc.eType			= EPhysicsActorType::KINEMATIC;
		desc.detection		= EPhysicsCollisionDetection::DISCRETE;
		desc.bUseGravity	= false;
		desc.bIsKinematic	= true;

		if (FAILED(Add_Component<CPhysicsRigidBody>(/* static */ 0, L"Prototype_Component_Physics_RigidBody", &desc)))
			return E_FAIL;
	}
	return S_OK;
}

CMoon_SkillE_Obj* CMoon_SkillE_Obj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMoon_SkillE_Obj* pInstance = new CMoon_SkillE_Obj(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMoon_SkillE_Obj::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMoon_SkillE_Obj::Clone(void* pArg)
{
	CMoon_SkillE_Obj* pInstance = new CMoon_SkillE_Obj(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMoon_SkillE_Obj::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMoon_SkillE_Obj::Free()
{
	Super::Free();
}