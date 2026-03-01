#include "pch.h"
#include "Xibi_Loop_Thunder.h"
#include "EffectHandler.h"
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

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Get_Component<CPhysicsRigidBody>()->Awake();
	return S_OK;
}

HRESULT CXibi_Loop_Thunder::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	Get_Component<CEffectHandler>()->Awake();
	return S_OK;
}

void CXibi_Loop_Thunder::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CXibi_Loop_Thunder::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CXibi_Loop_Thunder::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CXibi_Loop_Thunder::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

HRESULT CXibi_Loop_Thunder::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	return S_OK;
}

void CXibi_Loop_Thunder::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{

}

void CXibi_Loop_Thunder::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CXibi_Loop_Thunder::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CXibi_Loop_Thunder::Set_Dead(const wstring& wstrLayerTag)
{
	m_bDead = true;
	m_pGameInstance->Request_DeleteGameObject(
		m_pGameInstance->Get_CurrentLevelIndex(),
		g_wszSkillObjectLayer,
		this);
	Get_Component<CEffectHandler>()->Trigger_Lifecycle_Effect(CEffectHandler::E_OBJ_LIFECYCLE_STATE::ON_DESTROY);
}

_bool CXibi_Loop_Thunder::On_Hit(const HIT_DESC& hitDesc)
{
	return true;
}

void CXibi_Loop_Thunder::Try_Attack(const HIT_DESC& hitDesc)
{
}

HRESULT CXibi_Loop_Thunder::Ready_Components()
{
	// For. Component_EffectHandler
	{
		CEffectHandler::ANIM_EFFECT_HANDLER_DESC Desc{};
		CEffectHandler::STATE_VFX_DESC SkillDesc{};

		// SPAWN EFFECT
		{
			SkillDesc.EffectPrefabTag = "Boss_Xibi_Lightning";
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
			cloneDesc.eFilterLayer = tagPhysicsFilterGroup::MONSTER_SKILL_PROJECTTILE;
			cloneDesc.iFilterMask =
			{
				PHYSICSFILTERGROUP::Enum::PLAYER
				| PHYSICSFILTERGROUP::Enum::ATTACK_PROJECTTILE
				| PHYSICSFILTERGROUP::Enum::SKILL_PROJECTTILE
				| PHYSICSFILTERGROUP::Enum::ATTACK
				| PHYSICSFILTERGROUP::Enum::MAP
			};
			cloneDesc.bIsTrigger = true;
			cloneDesc.bSetOnlyFilter = false;
			cloneDesc.bIsActive = true;
			cloneDesc.vExtents = { 0.3f, 5.f,0.3f };
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