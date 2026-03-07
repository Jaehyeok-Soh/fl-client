#include "pch.h"
#include "Xibi_Projectile_Circle.h"
#include "EffectHandler.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CXibi_Projectile_Circle::CXibi_Projectile_Circle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CXibi_Projectile_Circle::CXibi_Projectile_Circle(const CXibi_Projectile_Circle& rhs)
	: Super(rhs)
{

}

HRESULT CXibi_Projectile_Circle::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_Projectile_Circle::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (CPhysicsRigidBody* pRigidBody = Get_Component<CPhysicsRigidBody>())
		pRigidBody->Awake();
	
	if (CEffectHandler* pEffectHandler = Get_Component<CEffectHandler>())
		pEffectHandler->Setup_ForOwner(this);

	return S_OK;
}

HRESULT CXibi_Projectile_Circle::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CXibi_Projectile_Circle::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CXibi_Projectile_Circle::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CXibi_Projectile_Circle::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CXibi_Projectile_Circle::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

HRESULT CXibi_Projectile_Circle::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CXibi_Projectile_Circle::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

void CXibi_Projectile_Circle::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{

}

void CXibi_Projectile_Circle::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CXibi_Projectile_Circle::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (iOtherLayer == PHYSICSFILTERGROUP::Enum::MAP)
		Set_Dead();

	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyColliderLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	//desc.tHitInfo = tHitInfo;

	m_pGameInstance->Push_CollidedData(desc);
}

_bool CXibi_Projectile_Circle::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CXibi_Projectile_Circle::Try_Attack(const HIT_DESC& hitDesc)
{
}

HRESULT CXibi_Projectile_Circle::Ready_Components()
{
	// For. Component_EffectHandler
	{
		CEffectHandler::ANIM_EFFECT_HANDLER_DESC Desc{};
		CEffectHandler::STATE_VFX_DESC SkillDesc{};

		// SPAWN EFFECT
		{
			SkillDesc.EffectPrefabTag = "Boss_Xibi_Bullet_Spawn";
			SkillDesc.pParentTransformMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
			SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_LOCAL };
			SkillDesc.bFollowBone = { false };
			SkillDesc.iBoneIndex = -1;
			SkillDesc.vOffSet = { Vec3::Zero };
			SkillDesc.vRotation = { Vec3::Zero };
			Desc.eType = CEffectHandler::E_HANDLER_TYPE::SKILL_OBJ;
			Desc.mEffectState.emplace(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_SPAWN, SkillDesc);
		}

		// Distory EFFECT
		{
			SkillDesc.EffectPrefabTag = "Boss_Xibi_Bullet_Dead";
			SkillDesc.pParentTransformMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
			SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_LOCAL };
			SkillDesc.bFollowBone = { false };
			SkillDesc.iBoneIndex = -1;
			SkillDesc.vOffSet = {Vec3::Zero};
			SkillDesc.vRotation = {Vec3::Zero};			
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
			cloneDesc.eShape = EPhysicsShape::SPHERE;
			cloneDesc.eFilterLayer = tagPhysicsFilterGroup::MONSTER_SKILL_PROJECTTILE;
			//cloneDesc.bIsSkillTrigger = true;
			cloneDesc.iFilterMask = 
			{
				PHYSICSFILTERGROUP::Enum::PLAYER
				| PHYSICSFILTERGROUP::Enum::MAP
			};
			cloneDesc.bIsTrigger = true;
			cloneDesc.bSetOnlyFilter = false;
			cloneDesc.bIsActive = true;
			cloneDesc.fRadius = 0.5f;
			cloneDesc.strAttackPresetTag = "Xibi_Circle";
			PHYSICSMATERIAL_DESC mtrlDesc{};
			mtrlDesc.eMaterial = EPhysicsMaterial::CONCRETE;
			cloneDesc.tMaterial = mtrlDesc;
			if (FAILED(Add_Component<CPhysicsCollider>(/* static */ 0, L"Prototype_Component_Physics_Collider", &cloneDesc)))
				return E_FAIL;
		}
	}

	// For. Component_PhysicsRigidBody
	{
		PHYSICSRIGIDBODY_DESC desc{};
		desc.eType = EPhysicsActorType::KINEMATIC;
		desc.detection = EPhysicsCollisionDetection::DISCRETE;
		desc.bUseGravity = false;
		desc.bIsKinematic = true;

		if (FAILED(Add_Component<CPhysicsRigidBody>(/* static */ 0, L"Prototype_Component_Physics_RigidBody", &desc)))
			return E_FAIL;
	}
	return S_OK;
}

CXibi_Projectile_Circle* CXibi_Projectile_Circle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CXibi_Projectile_Circle* pInstance = new CXibi_Projectile_Circle(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CXibi_Projectile_Circle::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CXibi_Projectile_Circle::Clone(void* pArg)
{
	CXibi_Projectile_Circle* pInstance = new CXibi_Projectile_Circle(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CXibi_Projectile_Circle::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CXibi_Projectile_Circle::Free()
{
	Super::Free();
}