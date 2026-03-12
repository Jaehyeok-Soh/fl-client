#include "pch.h"
#include "Moon_SkillE_Obj.h"
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

	if (FAILED(Ready_Modules()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMoon_SkillE_Obj::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CMoon_SkillE_Obj::Handle_Hit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	if (iOtherLayer == PHYSICSFILTERGROUP::Enum::MAP)
	{
		Set_Dead();
		return;
	}

	COLLIDED_DESC desc{};
	desc.iCollisionType = COLLISIONEVENT::ON_COLLISION_ENTER;
	desc.iRequesterLayer = iMyLayer;
	desc.iOtherLayer = iOtherLayer;
	desc.pRequester = this;
	desc.pOther = pOther;
	desc.tHitInfo = tHitInfo;

	EXTRA_ATTACK_DESC tExtra = {};
	{
		tExtra.iDamageFlag = ENUM_TO_UINT(EPlayerAttackFlag::MOON) | ENUM_TO_UINT(EPlayerAttackFlag::SKILLE);

		desc.tExtraDesc = tExtra;
	}

	m_pGameInstance->Push_CollidedData(desc);
}

HRESULT CMoon_SkillE_Obj::Ready_Modules()
{
	wstring wstrDefaultPrototypeTag = L"Prototype_GameObject_Effect";

	// Effect
	{
		// FLY
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"PlayerMoon_ESkillObject",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::FLY))))
				return E_FAIL;
		}
	}
	// Collider
	{
		PHYSICSCOLLIDER_DESC colliderDesc{};
		colliderDesc.eShape = EPhysicsShape::BOX;
		colliderDesc.eFilterLayer = PHYSICSFILTERGROUP::ATTACK;
		//cloneDesc.bIsSkillTrigger = true;
		colliderDesc.iFilterMask =
		{
				PHYSICSFILTERGROUP::Enum::MONSTER
				| PHYSICSFILTERGROUP::Enum::OBJECT1
				| PHYSICSFILTERGROUP::Enum::OBJECT2
		};
		colliderDesc.bIsTrigger = true;
		colliderDesc.bSetOnlyFilter = false;
		colliderDesc.bIsActive = true;
		colliderDesc.vCenter = { 0.f, 0.3f, 1.5f };
		colliderDesc.vExtents = { 3.f, 5.f, 2.f };
		colliderDesc.strAttackPresetTag = "MoonSkill_E";
		PHYSICSMATERIAL_DESC mtrlDesc{};
		mtrlDesc.eMaterial = EPhysicsMaterial::CONCRETE;
		colliderDesc.tMaterial = mtrlDesc;

		PHYSICSRIGIDBODY_DESC rigidbodyDesc{};
		rigidbodyDesc.eType = EPhysicsActorType::KINEMATIC;
		rigidbodyDesc.detection = EPhysicsCollisionDetection::DISCRETE;
		rigidbodyDesc.bUseGravity = false;
		rigidbodyDesc.bIsKinematic = true;

		// IMPACT
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