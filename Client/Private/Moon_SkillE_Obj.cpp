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

void CMoon_SkillE_Obj::Try_Attack(const HIT_DESC& hitDesc)
{
	// damage 폰트 : iDamageFlag에 따라 크리티컬 || 일반 판정

	// 일반 공격 데미지 폰트
	{
		Vec3 vPos =  Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
		vPos.y += 0.5f;

		UI_PREFAB_DATA tPrefabData = {};
		tPrefabData.DamageFontData.iDamage		= static_cast<_uint>(hitDesc.fFinalDamage); // 데미지 폰트에 뜰 숫자 // 플레이어 공격력 // 랜덤은 보여주기용
		tPrefabData.DamageFontData.vFontColor	= Vec4{ 1.f, 0.95f, 0.47f, 1.f }; // 데미지 폰트 색 // 캐릭터 고유 색
		tPrefabData.DamageFontData.vHitPos = vPos; // 데미지 폰트를 띄울 World 위치 // 
		tPrefabData.DamageFontData.vRandOffset = Vec3{
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(-1.f, 1.f),
			m_pGameInstance->Rand_Float(-1.f, 1.f) }; // 랜덤 오프셋 // 더 커지면 이상함

		CUI_Manager::GetInstance()->Request_Add_Prefab(
			m_pGameInstance->Get_CurrentLevelIndex(), EUIPrefabType::DAMAGE_FONTS_COMMON, m_pGameInstance->Get_CurrentLevelIndex(), &tPrefabData);
	}
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
		colliderDesc.eFilterLayer = tagPhysicsFilterGroup::ATTACK;
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
		colliderDesc.vCenter = { 0.f, 0.3f, 0.f };
		colliderDesc.vExtents = { 1.f, 4.f,1.f };
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