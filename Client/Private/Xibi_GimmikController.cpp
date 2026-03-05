#include "pch.h"
#include "Xibi_GimmikController.h"

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

CXibi_GimmikController::CXibi_GimmikController()
	: Super()
{
}

CXibi_GimmikController::CXibi_GimmikController(const CXibi_GimmikController& rhs)
	: Super(rhs)
{
}

HRESULT CXibi_GimmikController::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_GimmikController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Spawner()))
		return E_FAIL;

	return S_OK;
}

HRESULT CXibi_GimmikController::Awake(const _uint iCurLevelIndex)
{
	Bind_ModelAnimNotify();

	if (FAILED(Set_Event()))
		return E_FAIL;

	m_pOneshotThunderSpawner->Awake(iCurLevelIndex);
	m_pRandomThunderSpawner->Awake(iCurLevelIndex);
	m_p3wayThunderSpawner->Awake(iCurLevelIndex);
	m_p360CircleSpawner->Awake(iCurLevelIndex);
	m_p360ThunderSpawner->Awake(iCurLevelIndex);

	m_vSpawnPosition = Get_Owner()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	return S_OK;
}

void CXibi_GimmikController::Update(const _float fTiemDelta)
{
	m_pRandomThunderSpawner->Update(fTiemDelta);
	m_p3wayThunderSpawner->Update(fTiemDelta);
	m_pOneshotThunderSpawner->Update(fTiemDelta);
	m_p360CircleSpawner->Update(fTiemDelta);
	m_p360ThunderSpawner->Update(fTiemDelta);
}

HRESULT CXibi_GimmikController::Set_Event()
{
	if (m_pOwnerModel == nullptr)
		return E_FAIL;

	// Teleport Random
	{
		_int iXibiSkill03_AnimIndex = Get_Owner()->Get_AnimationIndex(L"Animation_Xibi_Skill03");
		if (iXibiSkill03_AnimIndex == -1)
			return E_FAIL;

		CModelAnimation* pAnim = m_pOwnerModel->Get_Animation(iXibiSkill03_AnimIndex);
		if (pAnim == nullptr)
			return E_FAIL;

		// Teleport Random Disppear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 14.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportRandom_Disappear);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}

		// Teleport Random Appear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 19.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportRandom_Appear);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
	}

	// Teleport Center
	{
		_int iXibiSkill10_AnimIndex = Get_Owner()->Get_AnimationIndex(L"Animation_Xibi_Skill10");
		if (iXibiSkill10_AnimIndex == -1)
			return E_FAIL;

		CModelAnimation* pAnim = m_pOwnerModel->Get_Animation(iXibiSkill10_AnimIndex);
		if (pAnim == nullptr)
			return E_FAIL;

		// Teleport Center Appear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 6.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportCenter_Disappear);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}

		// Teleport Center Disappear
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 22.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::TeleportCenter_Appear);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
	}

	// SpawnProjectile
	{
		_int iXibiSkill09_AnimIndex = Get_Owner()->Get_AnimationIndex(L"Animation_Xibi_Skill09");
		if (iXibiSkill09_AnimIndex == -1)
			return E_FAIL;

		CModelAnimation* pAnim = m_pOwnerModel->Get_Animation(iXibiSkill09_AnimIndex);
		if (pAnim == nullptr)
			return E_FAIL;

		// SpawnThunder3way
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 24.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::SpawnThunder3way);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}

		// Spawn360Circle
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 35.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::Spawn360Circle);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
	}

	// SpawnProjectile
	{
		_int iXibiSkill10_AnimIndex = Get_Owner()->Get_AnimationIndex(L"Animation_Xibi_Skill10");
		if (iXibiSkill10_AnimIndex == -1)
			return E_FAIL;

		CModelAnimation* pAnim = m_pOwnerModel->Get_Animation(iXibiSkill10_AnimIndex);
		if (pAnim == nullptr)
			return E_FAIL;

		// Spawn360Thunder
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 26.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::Spawn360Thunder);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
		// SpawnRandomThunder
		{
			AnimNotifyKey key{};
			key.eID = EAnimNotifyId::Trigger_Gimmik;
			key.fTrackPosition = 60.f;
			key.iParam0 = ENUM_TO_UINT(EGimmikType::SpawnThunderRandom);
			pAnim->Pushback_Notifies(EAnimNotifyPhase::Late, key);
		}
	}
	return S_OK;
}

void CXibi_GimmikController::On_TeleportRandom_Disappear()
{
	Get_Owner()->Set_Render(false);

	// Todo. BattleField에서 랜덤 위치 계산
	// Todo. 순간이동시 Physics 처리
	SpawnSingleThunder();
	Vec3 vCurPos = Get_Owner()->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	Teleport_To(Compute_RandomTeleportPosition(vCurPos));
}

void CXibi_GimmikController::On_TeleportRandom_Appear()
{
	Get_Owner()->Set_Render(true);
}

void CXibi_GimmikController::On_TeleportCenter_Disappear()
{
	Get_Owner()->Set_Render(false);

	// Todo. BattleField에서 센터 위치 계산
	// Todo. 순간이동시 Physics 처리
	Teleport_To(m_vSpawnPosition);
}

void CXibi_GimmikController::On_TeleportCenter_Appear()
{
	Get_Owner()->Set_Render(true);
}

void CXibi_GimmikController::On_ModelAnimNotify(const AnimNotifyKey& key)
{
	if (key.eID != EAnimNotifyId::Trigger_Gimmik)
		return;

	EGimmikType eType = static_cast<EGimmikType>(key.iParam0);
	if (eType >= EGimmikType::COUNT)
		return;

	switch (eType)
	{
	case Client::CXibi_GimmikController::EGimmikType::TeleportRandom_Disappear:
		On_TeleportRandom_Disappear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportRandom_Appear:
		On_TeleportRandom_Appear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportCenter_Disappear:
		On_TeleportCenter_Disappear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::TeleportCenter_Appear:
		On_TeleportCenter_Appear();
		break;
	case Client::CXibi_GimmikController::EGimmikType::SpawnThunderRandom:
		On_SpawnThunderRandom();
		break;
	case Client::CXibi_GimmikController::EGimmikType::SpawnThunder3way:
		On_SpawnThunder3way();
		break;
	case Client::CXibi_GimmikController::EGimmikType::Spawn360Circle:
		On_SpawnCircle360();
		break;
	case Client::CXibi_GimmikController::EGimmikType::Spawn360Thunder:
		On_SpawnThunder360();
		break;
	}
}

void CXibi_GimmikController::Teleport_To(const Vec3& vPos)
{
	CTransform* pTransform = Get_Owner()->Get_Component<CTransform>();
	CPhysicsCCT* pCCT = Get_Owner()->Get_Component<CPhysicsCCT>();

	pCCT->SetFootPosition(vPos);
	pCCT->Move(Vec3{ 0.01f }, 0.01f, 1.f / 60.f);

	Vec3 vFinal = pCCT->GetFootPosition();
	pTransform->Set_Info(TRANSFORM_INFO_STATE::POS, vFinal);
}

Vec3 CXibi_GimmikController::Compute_RandomTeleportPosition(const Vec3& vCurrentPos)
{
	CGameInstance* pGameinstance = CGameInstance::GetInstance();

	// 타겟 위치 (겹침 방지용)
	Vec3 vTargetPos = Vec3::Zero;
	_float m_fMinTargetDist = 1.3f;
	if (CGameObject* pTarget = pGameinstance->Get_GameObject_Front(0, L"Player_Layer"))
		vTargetPos = pTarget->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	constexpr _uint iMaxAttempt = 5;

	for (_uint i = 0; i < iMaxAttempt; ++i)
	{
		// 현재 위치 기준 랜덤 오프셋
		_float fAngle = pGameinstance->Rand_Float(0.f, XM_2PI);
		_float fDist = pGameinstance->Rand_Float(m_fRandomTeleportRange * 0.5f, m_fRandomTeleportRange);

		Vec3 vCandidate = vCurrentPos;
		vCandidate.x += cosf(fAngle) * fDist;
		vCandidate.z += sinf(fAngle) * fDist;

		// 필드 범위 체크 (스폰 기준)
		Vec3 vFromSpawn = vCandidate - m_vSpawnPosition;
		vFromSpawn.y = 0.f;
		if (vFromSpawn.Length() > m_fFieldMaxRange)
			continue;

		// 타겟 겹침 체크
		Vec3 vFromTarget = vCandidate - vTargetPos;
		vFromTarget.y = 0.f;
		if (vFromTarget.Length() < m_fMinTargetDist)
			continue;

		return vCandidate;
	}

	// 실패 시 스폰 위치 반대편
	Vec3 vFromSpawn = vCurrentPos - m_vSpawnPosition;
	vFromSpawn.y = 0.f;
	if (vFromSpawn.Length() > 0.001f)
	{
		vFromSpawn.Normalize();
		return m_vSpawnPosition - vFromSpawn * m_fRandomTeleportRange;
	}

	return m_vSpawnPosition;
}

void CXibi_GimmikController::SpawnSingleThunder()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pOneshotThunderSpawner->Trigger(desc);
}

void CXibi_GimmikController::On_SpawnThunderRandom()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pRandomThunderSpawner->Trigger(desc);
}

void CXibi_GimmikController::On_SpawnThunder3way()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_p3wayThunderSpawner->Trigger(desc);
}

void CXibi_GimmikController::On_SpawnCircle360()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vOrigin.y += 0.55f;
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_p360CircleSpawner->Trigger(desc);
}

void CXibi_GimmikController::On_SpawnThunder360()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_p360ThunderSpawner->Trigger(desc);
}

HRESULT CXibi_GimmikController::Ready_Spawner()
{
	_uint iLevelId = ENUM_TO_UINT(ELevelType::TUTORIAL_BOSS);

	// Oneshot
	{
		CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_XibiOneshotSingleThunder, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pOneshotThunderSpawner = static_cast<CSingleSkillSpawner*>(pResult);
	}

	// Rand
	{
		CSkillObjectSpawner_RandomXZ::SPAWNER_RAND_COPY_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;
		desc.fRadiusMin = 1.3f;
		desc.fRadiusMax = m_fFieldMaxRange;
		desc.iPerTick = 1;
		desc.fDurationSec = 3.5f;
		desc.bUseForwardDir = false;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_XibiOneshotRandomThunder, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pRandomThunderSpawner = static_cast<CSkillObjectSpawner_RandomXZ*>(pResult);
	}

	// 3wayThunder
	{
		CProjectileSpawner_Fan::PR_SPAWNER_FAN_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;
		desc.iCount = 3;
		desc.fSpreadDeg = 30.f;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_Xibi3wayLoopThunder, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_p3wayThunderSpawner = static_cast<CProjectileSpawner_Fan*>(pResult);
	}

	// 360도 원형 투사체
	{
		CProjectileSpawner_Radial360::PR_SPAWNER_RADIAL_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;
		desc.iCount = 12;
		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_Xibi360CircleProjectile, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_p360CircleSpawner = static_cast<CProjectileSpawner_Radial360*>(pResult);
	}

	// 360도 번개
	{
		CProjectileSpawner_Radial360::PR_SPAWNER_RADIAL_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;
		desc.iCount = 8;
		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_Xibi360ThunderProjectile, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_p360ThunderSpawner = static_cast<CProjectileSpawner_Radial360*>(pResult);
	}

	return S_OK;
}

CXibi_GimmikController* CXibi_GimmikController::Create()
{
	CXibi_GimmikController* pInstance = new CXibi_GimmikController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CXibi_GimmikController::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CXibi_GimmikController::Clone(void* pArg)
{
	CXibi_GimmikController* pInstance = new CXibi_GimmikController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CXibi_GimmikController::Clone, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CXibi_GimmikController::Free()
{
	Safe_Release(m_pOneshotThunderSpawner);
	Safe_Release(m_pRandomThunderSpawner);
	Safe_Release(m_p360ThunderSpawner);
	Safe_Release(m_p3wayThunderSpawner);
	Safe_Release(m_p360CircleSpawner);
	Super::Free();
}
