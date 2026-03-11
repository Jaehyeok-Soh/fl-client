#include "pch.h"
#include "Xibi_Loop_Thunder.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CXibi_Loop_Thunder::CXibi_Loop_Thunder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CXibi_Loop_Thunder::CXibi_Loop_Thunder(const CXibi_Loop_Thunder& rhs)
	: Super(rhs)
{

}

HRESULT CXibi_Loop_Thunder::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_Loop_Thunder::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Modules()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_Loop_Thunder::Ready_Modules()
{
	wstring wstrDefaultPrototypeTag = L"Prototype_GameObject_Effect";

	// Effect
	{
		// FLY
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"Boss_Xibi_Lightning",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::FLY))))
				return E_FAIL;
		}
	}
	// Collider
	{
		PHYSICSCOLLIDER_DESC colliderDesc{};
		colliderDesc.eShape = EPhysicsShape::BOX;
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
		colliderDesc.vCenter = { 0.f, 2.3f, 0.f };
		colliderDesc.vExtents = { 0.3f, 4.f,0.3f };
		colliderDesc.strAttackPresetTag = "Xibi_Thunder";
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

CXibi_Loop_Thunder* CXibi_Loop_Thunder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CXibi_Loop_Thunder* pInstance = new CXibi_Loop_Thunder(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CXibi_Loop_Thunder::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CXibi_Loop_Thunder::Clone(void* pArg)
{
	CXibi_Loop_Thunder* pInstance = new CXibi_Loop_Thunder(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CXibi_Loop_Thunder::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CXibi_Loop_Thunder::Free()
{
	Super::Free();
}