#include "pch.h"
#include "Lianhuo_ChainThron.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CLianhuo_ChainThron::CLianhuo_ChainThron(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CLianhuo_ChainThron::CLianhuo_ChainThron(const CLianhuo_ChainThron& rhs)
	: Super(rhs)
{

}

HRESULT CLianhuo_ChainThron::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLianhuo_ChainThron::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Modules()))
		return E_FAIL;

	return S_OK;
}

void CLianhuo_ChainThron::Build_WarningDesc()
{
	m_tWarnDesc = {};
	m_tDesc.vSpawnPos.y += 0.01f;
	m_tWarnDesc.VFX_Target_Position = m_tDesc.vSpawnPos;
	m_tWarnDesc.VFX_COLORTYPE = EFFECT_SPAWN_DESC::E_VFX_COLORMODE::COLOR_CHANGE;
	m_tWarnDesc.VFX_Color = Vec3{ 187.f / 255.f, 45.f / 255.f, 20.f / 255.f };
	m_tWarnDesc.VFX_Scale = Vec3{ 3.f, 3.f, 3.f };;
	m_tWarnDesc.iSimulationType = ENUM_TO_UINT(EFFECT_WARNING_DESC::E_VFX_SIMULTYPE::VFX_WORLD);
}

HRESULT CLianhuo_ChainThron::Ready_Modules()
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
				"Boss_LianHuo_ChainThornAttack",
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
		colliderDesc.fRadius = 4.f;
		colliderDesc.strAttackPresetTag = "ChainThron";
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

CLianhuo_ChainThron* CLianhuo_ChainThron::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLianhuo_ChainThron* pInstance = new CLianhuo_ChainThron(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CLianhuo_ChainThron::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CLianhuo_ChainThron::Clone(void* pArg)
{
	CLianhuo_ChainThron* pInstance = new CLianhuo_ChainThron(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLianhuo_ChainThron::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLianhuo_ChainThron::Free()
{
	Super::Free();
}