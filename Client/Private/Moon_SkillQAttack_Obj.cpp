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

    if (FAILED(Ready_Modules()))
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
			tExtra.iDamageFlag = ENUM_TO_UINT(EPlayerAttackFlag::SWORD) | ENUM_TO_UINT(EPlayerAttackFlag::SKILLQ);

			desc.tExtraDesc = tExtra;
		}

		m_pGameInstance->Push_CollidedData(desc);

		// acc time reset
		m_TAttackCoolTime.x = 0.f;
	}
}

HRESULT CMoon_SkillQAttack_Obj::Ready_Modules()
{
	wstring wstrDefaultPrototypeTag = L"Prototype_GameObject_Effect";

	// EffectModule
	{
		// FLY ( Move flag 없으면 제자리 )
		{
			if (FAILED(Add_EffectModule(
				0 /* static */,
				"Player_Moon_QSkill_AOE",
				wstrDefaultPrototypeTag,
				ENUM_TO_UINT(EState::FLY))))
				return E_FAIL;
		}
	}	

	// ColliderModule
	{
		PHYSICSCOLLIDER_DESC colliderDesc{};
		colliderDesc.eShape = EPhysicsShape::SPHERE;
		colliderDesc.eFilterLayer = PHYSICSFILTERGROUP::ATTACK; // todo_eunbi : player?
		colliderDesc.iFilterMask =
		{
			PHYSICSFILTERGROUP::Enum::MONSTER
			| PHYSICSFILTERGROUP::Enum::OBJECT1
			| PHYSICSFILTERGROUP::Enum::OBJECT2
		};

		colliderDesc.bIsTrigger = true;
		colliderDesc.bSetOnlyFilter = false;
		colliderDesc.bIsActive = true;
		colliderDesc.vCenter = { 0.f, 0.f, 0.f };
		colliderDesc.fRadius = { 8.f };
		colliderDesc.strAttackPresetTag = "MoonSkill_Q";
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
