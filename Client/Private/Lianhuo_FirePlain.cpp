#include "pch.h"
#include "Lianhuo_FirePlain.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CLianhuo_FirePlain::CLianhuo_FirePlain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CLianhuo_FirePlain::CLianhuo_FirePlain(const CLianhuo_FirePlain& rhs)
	: Super(rhs)
{

}

HRESULT CLianhuo_FirePlain::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLianhuo_FirePlain::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Modules()))
		return E_FAIL;

	m_tWarnDesc.VFX_COLORTYPE = EFFECT_SPAWN_DESC::E_VFX_COLORMODE::COLOR_CHANGE;
	m_tWarnDesc.VFX_Color = Vec3{ 187.f / 255.f, 45.f / 255.f, 20.f / 255.f };
	m_tWarnDesc.VFX_Scale = Vec3{ 3.f, 3.f, 3.f };
	return S_OK;
}

HRESULT CLianhuo_FirePlain::Ready_Modules()
{
	wstring wstrDefaultPrototypeTag = L"Prototype_GameObject_Effect";

	// Effect
	{
		// WARNING
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"WarningCircle1",
				L"Prototype_GameObject_Effect_WarningCircle",
				ENUM_TO_UINT(EState::WARNING))))
				return E_FAIL;
		}

		// STRIKE
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"Boss_LianHuo_FirePlane",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::STRIKE))))
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
		colliderDesc.fRadius = 2.f;
		colliderDesc.strAttackPresetTag = "Lianhuo_FirePlain";
		PHYSICSMATERIAL_DESC mtrlDesc{};
		mtrlDesc.eMaterial = EPhysicsMaterial::CONCRETE;
		colliderDesc.tMaterial = mtrlDesc;

		PHYSICSRIGIDBODY_DESC rigidbodyDesc{};
		rigidbodyDesc.eType = EPhysicsActorType::KINEMATIC;
		rigidbodyDesc.detection = EPhysicsCollisionDetection::DISCRETE;
		rigidbodyDesc.bUseGravity = false;
		rigidbodyDesc.bIsKinematic = true;

		// STRIKE
		{
			if (FAILED(Add_CollideModule(
				ENUM_TO_UINT(EState::STRIKE),
				&colliderDesc,
				&rigidbodyDesc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

CLianhuo_FirePlain* CLianhuo_FirePlain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLianhuo_FirePlain* pInstance = new CLianhuo_FirePlain(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CLianhuo_FirePlain::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLianhuo_FirePlain::Clone(void* pArg)
{
	CLianhuo_FirePlain* pInstance = new CLianhuo_FirePlain(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLianhuo_FirePlain::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLianhuo_FirePlain::Free()
{
	Super::Free();
}