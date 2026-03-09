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

_bool CXibi_Projectile_Circle::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CXibi_Projectile_Circle::Try_Attack(const HIT_DESC& hitDesc)
{
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
				ENUM_TO_UINT(EState::FLY))))
				return E_FAIL;
		}
	}
	// Collider
	{
		PHYSICSCOLLIDER_DESC colliderDesc{};
		colliderDesc.eShape = EPhysicsShape::SPHERE;
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
		colliderDesc.fRadius = 0.5f;
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