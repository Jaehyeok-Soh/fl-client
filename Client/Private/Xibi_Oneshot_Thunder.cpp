#include "pch.h"
#include "Xibi_Oneshot_Thunder.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"
#include "GameInstance.h"

CXibi_Oneshot_Thunder::CXibi_Oneshot_Thunder(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: Super(pDevice, pDeviceContext)
{

}

CXibi_Oneshot_Thunder::CXibi_Oneshot_Thunder(const CXibi_Oneshot_Thunder& rhs)
	: Super(rhs)
{

}

HRESULT CXibi_Oneshot_Thunder::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_Oneshot_Thunder::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Modules()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_Oneshot_Thunder::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CXibi_Oneshot_Thunder::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CXibi_Oneshot_Thunder::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

_bool CXibi_Oneshot_Thunder::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CXibi_Oneshot_Thunder::Try_Attack(const HIT_DESC& hitDesc)
{
}

HRESULT CXibi_Oneshot_Thunder::Ready_Modules()
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
				"Boss_Xibi_Lightning_Oneshot",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::STRIKE))))
				return E_FAIL;
		}
	}
	// Collider
	{
		PHYSICSCOLLIDER_DESC colliderDesc{};
		colliderDesc.eShape = EPhysicsShape::BOX;
		colliderDesc.eFilterLayer = tagPhysicsFilterGroup::MONSTER_SKILL_PROJECTTILE;
		//cloneDesc.bIsSkillTrigger = true;
		colliderDesc.iFilterMask =
		{
			PHYSICSFILTERGROUP::Enum::PLAYER
			| PHYSICSFILTERGROUP::Enum::MAP
		};
		colliderDesc.bIsTrigger = true;
		colliderDesc.bSetOnlyFilter = false;
		colliderDesc.bIsActive = true;
		colliderDesc.vCenter = { 0.f, 2.f, 0.f };
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
				ENUM_TO_UINT(EState::STRIKE),
				&colliderDesc,
				&rigidbodyDesc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

CXibi_Oneshot_Thunder* CXibi_Oneshot_Thunder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CXibi_Oneshot_Thunder* pInstance = new CXibi_Oneshot_Thunder(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CXibi_Oneshot_Thunder::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CXibi_Oneshot_Thunder::Clone(void* pArg)
{
	CXibi_Oneshot_Thunder* pInstance = new CXibi_Oneshot_Thunder(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CXibi_Oneshot_Thunder::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CXibi_Oneshot_Thunder::Free()
{
	Super::Free();
}