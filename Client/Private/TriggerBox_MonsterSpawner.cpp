#include "pch.h"
#include "TriggerBox_MonsterSpawner.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"


//=================
// Monster
//=================
#include "Monster_Base.h"
#include "Monster_Body_Base.h"
#include "MonsterState_Factory.h"
#include "MonsterControlContext.h"
#include "MonsterActionState.h"


CTriggerBox_MonsterSpawner::CTriggerBox_MonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CTriggerBox(pDevice ,pContext) 
	, m_vecMonsterSpawnData{}
{
    m_eTriggerBoxType = CTriggerBox::Type::MONSTER_SPAWNER;
}

CTriggerBox_MonsterSpawner::CTriggerBox_MonsterSpawner(const CTriggerBox_MonsterSpawner& rhs)
    : CTriggerBox(rhs)
    , m_vecMonsterSpawnData{rhs.m_vecMonsterSpawnData }
{
}

HRESULT CTriggerBox_MonsterSpawner::Initialize_Prototype()
{
    if (FAILED(Super::Initialize_Prototype()))
        return E_FAIL;



    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Initialize(void* pArg)
{
    if (FAILED(Super::Initialize(pArg)))
        return E_FAIL;




	TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc = static_cast<TRIGGERBOX_MONSTERSPAWNER_DESC*>(pArg);

	m_vecMonsterSpawnData = pDesc->vecMonsterSpawnData;

    if (FAILED(Ready_Component(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTriggerBox_MonsterSpawner::Ready_Component(TRIGGERBOX_MONSTERSPAWNER_DESC* pDesc)
{


    return S_OK;
}


HRESULT CTriggerBox_MonsterSpawner::Awake(const _uint iCurrentLevelID)
{
    if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

    return S_OK;
}

void CTriggerBox_MonsterSpawner::Update_Priority(const _float fTimeDelta)
{
    Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_MonsterSpawner::Update(const _float fTimeDelta)
{
    Super::Update(fTimeDelta);

	if (m_isTriggerEventPlay)
		return;

    /* 임시로 충돌처리 */

    CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
    if (pGameObject == nullptr) return;
    CTransform* pTs = pGameObject->Get_Component<CTransform>();
    if (pTs == nullptr) return;
    Vec3 vPosition = pTs->Get_Info(TRANSFORM_INFO_STATE::POS);


    /* 거리 체크 */
    Vec3 vDir = vPosition - Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
    float fLength = vDir.Length();

    /* 충돌 임시 로직 Check */
    if (fLength < 2.5f)
    {
		m_isTriggerEventPlay = true;
        if (FAILED(SpawnMonster()))
        {
            MSG_BOX(" Monster Spawn is failed");
            return;
        }
    }

}

void CTriggerBox_MonsterSpawner::Update_Late(const _float fTimeDelta)
{
    Super::Update_Late(fTimeDelta);
}

void CTriggerBox_MonsterSpawner::Ready_Before_Render(const _float fTimeDelta)
{
    Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTriggerBox_MonsterSpawner::Render()
{
    return S_OK;
}

void CTriggerBox_MonsterSpawner::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);

}

void CTriggerBox_MonsterSpawner::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
    Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);


}

void CTriggerBox_MonsterSpawner::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

void CTriggerBox_MonsterSpawner::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnTrigger_Enter(iMyColliderLayer, iOtherLayer, pOther);

}

void CTriggerBox_MonsterSpawner::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
    Super::OnTrigger_Exit(iMyColliderLayer, iOtherLayer, pOther);

}

HRESULT CTriggerBox_MonsterSpawner::SpawnMonster()
{
	CTransform::TRANSFORM_DESC transformDesc = {};

	_uint iCurLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();

	for (auto& tData : m_vecMonsterSpawnData)
	{
		DTO::EMakeMonsterType eMakeMonsterType{ tData.eMakeMonsterType};
		transformDesc.ScaleMatrix		= Matrix::CreateScale(tData.vScale);
		transformDesc.TranslationMatrix = Matrix::CreateTranslation(tData.vPosition);
		transformDesc.RotationMatrix	= Matrix::CreateFromYawPitchRoll(
			XMConvertToRadians(tData.vPitchYawRoll.y), XMConvertToRadians(tData.vPitchYawRoll.x), XMConvertToRadians(tData.vPitchYawRoll.z)
		);

		switch (eMakeMonsterType)
		{
		case DTO::EMakeMonsterType::Dog:
		{
			{
				CGameObject* pResult = { nullptr };

				CMonster_Base::MONSTER_DESC monsterDesc = {};
				monsterDesc.iLevelIndex = iCurLevelIndex;
				monsterDesc.wstrBodyModelTag = L"Prototype_Component_Model_Monster_Dog";
				monsterDesc.wstrPartBodyPrototypeTag = L"Prototype_GameObject_Monster_Dummy_Body";
				monsterDesc.wstrAttackOverlapPrototypeTag = L"Prototype_Component_AttackOverlap_Monster_Dog";
				monsterDesc.pTransform_Desc = &transformDesc;
				monsterDesc.wstrMonsterStateTag = L"Monster_Dog";
				{
					PHYSICSCCT_DESC desc;
					desc.pOwner = nullptr;
					desc.bIsPlayer = false;
					desc.eType = EPhysicsCCTType::CAPSULE;
					desc.pOwnerMatrix = nullptr;
					desc.fRadius = 1.f;
					desc.fHeight = 0.1f;
					desc.vExtens = { 2.f, 2.f, 2.f };

					PHYSICSMATERIAL_DESC mtrlDesc{};
					mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
					desc.tMaterial = mtrlDesc;

					desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::MONSTER;
					desc.iFilterMask =
						PHYSICSFILTERGROUP::Enum::MONSTER
						| PHYSICSFILTERGROUP::Enum::PLAYER
						| PHYSICSFILTERGROUP::Enum::ATTACK
						| PHYSICSFILTERGROUP::Enum::ATTACK_PROJECTTILE
						| PHYSICSFILTERGROUP::Enum::SKILL
						| PHYSICSFILTERGROUP::Enum::SKILL_PROJECTTILE
						| PHYSICSFILTERGROUP::Enum::MAP
						| PHYSICSFILTERGROUP::Enum::OBJECT1
						| PHYSICSFILTERGROUP::Enum::OBJECT2;

					monsterDesc.tCCTDesc = desc;
				}

				if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
					L"Prototype_GameObject_Monster_Dummy",
					iCurLevelIndex,
					L"Monster", &monsterDesc)))
					return E_FAIL;
			}
		}
		break;
		case DTO::EMakeMonsterType::Shooter:	return S_OK;
		case DTO::EMakeMonsterType::Xibi:
		{
			// BoneInfo
			vector<std::pair<_uint, string>> vecboneNames
			{
				{ENUM_TO_UINT(CMonster_Body_Base::EBone::RightHand), "hook_arm_r"}
			};

			{
				CGameObject* pResult = { nullptr };

				CMonster_Base::MONSTER_DESC monsterDesc = {};
				monsterDesc.iLevelIndex = iCurLevelIndex;
				monsterDesc.wstrBodyModelTag = L"Prototype_Component_Model_Xibi";
				monsterDesc.wstrPartBodyPrototypeTag = L"Prototype_GameObject_Boss_Xibi_Body";
				monsterDesc.pTransform_Desc = &transformDesc;
				monsterDesc.wstrMonsterStateTag = L"Boss_Xibi";

				{
					PHYSICSCCT_DESC desc;
					desc.pOwner = nullptr;
					desc.bIsPlayer = false;
					desc.eType = EPhysicsCCTType::CAPSULE;
					desc.pOwnerMatrix = nullptr;
					desc.fRadius = 1.f;
					desc.fHeight = 1.f;
					desc.vExtens = { 2.f, 2.f, 2.f };

					PHYSICSMATERIAL_DESC mtrlDesc{};
					mtrlDesc.eMaterial = EPhysicsMaterial::PLAYER;
					desc.tMaterial = mtrlDesc;

					desc.eFilterLayer = PHYSICSFILTERGROUP::Enum::MONSTER;
					desc.iFilterMask =
						PHYSICSFILTERGROUP::Enum::MONSTER
						| PHYSICSFILTERGROUP::Enum::PLAYER
						| PHYSICSFILTERGROUP::Enum::ATTACK
						| PHYSICSFILTERGROUP::Enum::ATTACK_PROJECTTILE
						| PHYSICSFILTERGROUP::Enum::SKILL
						| PHYSICSFILTERGROUP::Enum::SKILL_PROJECTTILE
						| PHYSICSFILTERGROUP::Enum::MAP
						| PHYSICSFILTERGROUP::Enum::OBJECT1
						| PHYSICSFILTERGROUP::Enum::OBJECT2;

					monsterDesc.tCCTDesc = desc;
				}

				if (!(pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC),
					L"Prototype_GameObject_Boss_Xibi",
					iCurLevelIndex,
					g_wszBossLayer, &monsterDesc)))
					return E_FAIL;
			}
		}
		break;
		default:
			break;
		}



	}

    return S_OK;
}

CTriggerBox_MonsterSpawner* CTriggerBox_MonsterSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTriggerBox_MonsterSpawner* pTriggerBox = new CTriggerBox_MonsterSpawner(pDevice ,pContext);

    if (FAILED(pTriggerBox->Initialize_Prototype()))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Monster Spawner Is Failed To Craete ");
        return nullptr;
    }

    return pTriggerBox;
}

CGameObject* CTriggerBox_MonsterSpawner::Clone(void* pArg)
{
    CTriggerBox_MonsterSpawner* pTriggerBox = new CTriggerBox_MonsterSpawner(*this);

    if (FAILED(pTriggerBox->Initialize(pArg)))
    {
        Safe_Release(pTriggerBox);
        MSG_BOX(" Trigger Box Monster Spawner Is Failed To Clone ");
        return nullptr;
    }

    return pTriggerBox;
}

void CTriggerBox_MonsterSpawner::Free()
{
    Super::Free();

    return;
}

