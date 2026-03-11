#include "pch.h"
#include "Xibi_Projectile_Circle.h"
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

	if (FAILED(Ready_Moduels()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_Projectile_Circle::Ready_Moduels()
{
	wstring wstrDefaultPrototypeTag = L"Prototype_GameObject_Effect";

	// Effect
	{
		// FLY
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"Boss_Xibi_Bullet_Spawn",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::FLY))))
				return E_FAIL;
		}
		// IMPACT
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"Boss_Xibi_Bullet_Dead",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::IMPACT))))
				return E_FAIL;
		}
	}
	// Collider
	{
		PHYSICSCOLLIDER_DESC colliderDesc{};
		colliderDesc.eShape = EPhysicsShape::SPHERE;
		colliderDesc.eFilterLayer = PHYSICSFILTERGROUP::MONSTER_SKILL_PROJECTTILE;
		//cloneDesc.bIsSkillTrigger = true;
		colliderDesc.iFilterMask =
		{
			PHYSICSFILTERGROUP::Enum::PLAYER
			| PHYSICSFILTERGROUP::Enum::MAP
		};
		colliderDesc.bIsTrigger = true;
		colliderDesc.bSetOnlyFilter = false;
		colliderDesc.bIsActive = true;
		colliderDesc.fRadius = 0.3f;
		colliderDesc.strAttackPresetTag = "Xibi_Circle";
		PHYSICSMATERIAL_DESC mtrlDesc{};
		mtrlDesc.eMaterial = EPhysicsMaterial::CONCRETE;
		colliderDesc.tMaterial = mtrlDesc;

		PHYSICSRIGIDBODY_DESC rigidbodyDesc{};
		rigidbodyDesc.eType = EPhysicsActorType::KINEMATIC;
		rigidbodyDesc.detection = EPhysicsCollisionDetection::DISCRETE;
		rigidbodyDesc.bUseGravity = false;
		rigidbodyDesc.bIsKinematic = true;

		// FLY
		{
			if (FAILED(Add_CollideModule(
				ENUM_TO_UINT(EState::FLY),
				&colliderDesc,
				&rigidbodyDesc)))
				return E_FAIL;
		}
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