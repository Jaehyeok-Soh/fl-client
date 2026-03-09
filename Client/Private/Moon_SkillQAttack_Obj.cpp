#include "pch.h"
#include "Moon_SkillQAttack_Obj.h"
#include "EffectHandler.h"
#include "PhysicsCollider.h"
#include "PhysicsRigidBody.h"

#include "UI_Manager.h"

// manager
#include "GameInstance.h"

CMoon_SkillQAttack_Obj::CMoon_SkillQAttack_Obj(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pDevice, pDeviceContext)
{
}

CMoon_SkillQAttack_Obj::CMoon_SkillQAttack_Obj(const CMoon_SkillQAttack_Obj& rhs)
    : Super(rhs)
	, m_TAttackCoolTime(rhs.m_TAttackCoolTime)
{
}

HRESULT CMoon_SkillQAttack_Obj::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;;
}

HRESULT CMoon_SkillQAttack_Obj::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    Get_Component<CPhysicsRigidBody>()->Awake();
	Get_Component<CEffectHandler>()->Setup_ForOwner(this);

    return S_OK;
}

HRESULT CMoon_SkillQAttack_Obj::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CMoon_SkillQAttack_Obj::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);

	Count_CoolTime(fTimeDelta);
}

void CMoon_SkillQAttack_Obj::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);
}

void CMoon_SkillQAttack_Obj::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CMoon_SkillQAttack_Obj::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
#ifdef _DEBUG
    m_pGameInstance->Push_DebugComponent(Get_Component<CPhysicsRigidBody>());
#endif
}

HRESULT CMoon_SkillQAttack_Obj::Render()
{
    if (FAILED(Super::Render()))
        return E_FAIL;

    return S_OK;
}

_bool CMoon_SkillQAttack_Obj::On_Hit(const HIT_DESC& hitDesc)
{
    return false;
}

void CMoon_SkillQAttack_Obj::Try_Attack(const HIT_DESC& hitDesc)
{

}

void CMoon_SkillQAttack_Obj::Handle_Hit(_uint iMyLayer, _uint iOtherLayer, Engine::CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	// todo : 한번 충돌 된 애들은 다시 충돌 처리가 안되어서
	// 그거에 대한 정보 처리가 필요함 :  active attack overlap을 참고 할것

	// 시간 차로 넣기
	//if (m_TAttackCoolTime.x == m_TAttackCoolTime.y)
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
			tExtra.iDamageFlag = ENUM_TO_UINT(EPlayerAttackFlag::MOON) | ENUM_TO_UINT(EPlayerAttackFlag::SKILLQ);

			desc.tExtraDesc = tExtra;
		}

		m_pGameInstance->Push_CollidedData(desc);

		// acc time reset
		m_TAttackCoolTime.x = 0.f;
	}
}

HRESULT CMoon_SkillQAttack_Obj::Ready_Components()
{
	// For. Component_EffectHandler
	{
		CEffectHandler::ANIM_EFFECT_HANDLER_DESC Desc{};
		CEffectHandler::STATE_VFX_DESC SkillDesc{};

		// SPAWN EFFECT
		{
			SkillDesc.EffectPrefabTag = "Player_Moon_QSkill_AOE";//"PlayerMoon_ESkillObject";
			SkillDesc.pParentTransformMatrix = &Get_Component<CTransform>()->Get_WorldMatrix();
			SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_WORLD };
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
			SkillDesc.bWorld = { CEffectHandler::E_WORLD::E_WORLD };
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
			cloneDesc.eShape = EPhysicsShape::SPHERE;
			cloneDesc.eFilterLayer = tagPhysicsFilterGroup::ATTACK; // todo_eunbi : player?
			cloneDesc.iFilterMask = 
			{
				PHYSICSFILTERGROUP::Enum::MONSTER
				| PHYSICSFILTERGROUP::Enum::OBJECT1
				| PHYSICSFILTERGROUP::Enum::OBJECT2
			};

			cloneDesc.bIsTrigger = true;
			cloneDesc.bSetOnlyFilter = false;
			cloneDesc.bIsActive = true;
			cloneDesc.vCenter = { 0.f, 0.f, 0.f };
			cloneDesc.fRadius = { 8.f };
			cloneDesc.strAttackPresetTag = "MoonSkill_Q";
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

void CMoon_SkillQAttack_Obj::Count_CoolTime(const _float fTimeDelta)
{
	m_TAttackCoolTime.x += fTimeDelta;
	if (m_TAttackCoolTime.x > m_TAttackCoolTime.y)
	{
		m_TAttackCoolTime.x = m_TAttackCoolTime.y;
	}

}

CMoon_SkillQAttack_Obj* CMoon_SkillQAttack_Obj::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CMoon_SkillQAttack_Obj* pInstance = new CMoon_SkillQAttack_Obj(pDevice, pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CMoon_SkillQAttack_Obj::Create, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMoon_SkillQAttack_Obj::Clone(void* pArg)
{
    CMoon_SkillQAttack_Obj* pInstance = new CMoon_SkillQAttack_Obj(*this);
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("CMoon_SkillQAttack_Obj::Clone, Failed");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMoon_SkillQAttack_Obj::Free()
{
    Super::Free();
}
