#include "pch.h"
#include "Monster_GimmikController.h"

#include "Model.h"
#include "GameObject.h"
#include "ModelAnimation.h"
#include "ActionState.h"
#include "PhysicsCCT.h"
#include "SingleSkillSpawner.h"
#include "SkillObjectSpawner_RandomXZ.h"
#include "ProjectileSpawner_Fan.h"
#include "ProjectileSpawner_Radial360.h"
#include "GameInstance.h"

CMonster_GimmikController::CMonster_GimmikController()
	: Super()
{
}

CMonster_GimmikController::CMonster_GimmikController(const CMonster_GimmikController& rhs)
	: Super(rhs)
{
}

HRESULT CMonster_GimmikController::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_GimmikController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster_GimmikController::Awake(const _uint iCurLevelIndex)
{
	for (auto& spawner : m_vecSpawner)
		spawner->Awake(iCurLevelIndex);

	return S_OK;
}

void CMonster_GimmikController::Update(const _float fTimeDelta)
{
	for (auto& spawner : m_vecSpawner)
		spawner->Update(fTimeDelta);
}

void CMonster_GimmikController::AddSkillSpawner(CSkillObjectSpawnerBase* pSpawner)
{
	m_vecSpawner.push_back(pSpawner);
}

void CMonster_GimmikController::SpawnTrigger(_uint iIndex)
{
	//_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	_uint iLevelIndex = ENUM_TO_UINT(ELevelType::STATIC);

	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CGameObject* pPlayer = m_pGameInstance->Get_GameObject_Front(0, L"Player_Layer");
	if (pPlayer == nullptr || pPlayer->IsDead())
		return;

	Vec3 vSpawnPos = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Vec3 vBaseDir = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);;
	if (pPlayer && pPlayer->IsDead() == false)
	{
		CTransform* pTargetTransform = pPlayer->Get_Component<CTransform>();
		if (pTargetTransform)
		{
			Vec3 vTargetPos = pTargetTransform->Get_Info(TRANSFORM_INFO_STATE::POS);
			vBaseDir = vTargetPos - vSpawnPos;
			if (vBaseDir != Vec3::Zero)
				vBaseDir.Normalize();
		}
	}
	Vec3 vLookDir = vBaseDir;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vOrigin.y += 0.55f;
	desc.vOrigin.z += 0.55f;
	desc.vForward = vLookDir;

	m_vecSpawner[iIndex]->Trigger(desc);
}

CMonster_GimmikController* CMonster_GimmikController::Create()
{
	CMonster_GimmikController* pInstance = new CMonster_GimmikController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CMonster_GimmikController::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMonster_GimmikController::Clone(void* pArg)
{
	CMonster_GimmikController* pInstance = new CMonster_GimmikController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CMonster_GimmikController::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMonster_GimmikController::Free()
{
	for (auto& spawner : m_vecSpawner)
		Safe_Release(spawner);
	m_vecSpawner.clear();

	Super::Free();
}
