#include "pch.h"
#include "Lianhuo_GimmikController.h"
#include "Boss_Lianhuo.h"
#include "Boss_Lianhuo_Body.h"
#include "SkillObjectSpawner_RandomXZ.h"
#include "SingleSkillSpawner.h"
#include "GameInstance.h"

CLianhuo_GimmikController::CLianhuo_GimmikController()
	: Super()
{
}

CLianhuo_GimmikController::CLianhuo_GimmikController(const CLianhuo_GimmikController& rhs)
	: Super(rhs)
{
}

HRESULT CLianhuo_GimmikController::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLianhuo_GimmikController::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Spawner()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLianhuo_GimmikController::Awake(const _uint iCurLevelIndex)
{
	// 구독
	if (FAILED(Super::Bind_Events()))
		return E_FAIL;

	//// 이벤트 하드코딩
	//if (FAILED(Set_Event()))
	//	return E_FAIL;

	m_tFirePlainTimer.Start(30.f);
	m_tChainThronTimer.Start(35.f);
	return S_OK;
}
void CLianhuo_GimmikController::Update(const _float fTimeDelta)
{
	Spawn_RandomSkill(fTimeDelta);
	m_pRandomFirePlain->Update(fTimeDelta);
	m_pRandomChainThron->Update(fTimeDelta);
	m_pXSpaceSpawner->Update(fTimeDelta);
	m_pStunChainSpawner->Update(fTimeDelta);
}

void CLianhuo_GimmikController::Set_SpawnPositionm(const Vec3& vPosition)
{
	m_vSpawnPosition = vPosition;
}

void CLianhuo_GimmikController::Trigger_XSpace(const Vec3 &vPosition)
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;
	
	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = vPosition;
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pXSpaceSpawner->Trigger(desc);
}

void CLianhuo_GimmikController::Trigger_StunChain(const Vec3& vPosition)
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = vPosition;
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pStunChainSpawner->Trigger(desc);
}

HRESULT CLianhuo_GimmikController::Bind_Events()
{
	return S_OK;
}

HRESULT CLianhuo_GimmikController::Ready_Spawner()
{
	_uint iLevelId = ENUM_TO_UINT(ELevelType::LIANHUO);

	// Rand ChainThron
	{
		CSkillObjectSpawner_RandomXZ::SPAWNER_RAND_COPY_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;
		desc.fRadiusMin = 2.f;
		desc.fRadiusMax = m_fFieldMaxRange;
		desc.iPerTick = 1;
		desc.fDurationSec = 0.1f;
		desc.bUseForwardDir = false;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_LianhuoSpawnerChainThron, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pRandomChainThron = static_cast<CSkillObjectSpawner_RandomXZ*>(pResult);
	}
	// Rand FirePlain
	{
		CSkillObjectSpawner_RandomXZ::SPAWNER_RAND_COPY_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;
		desc.fRadiusMin = 2.f;
		desc.fRadiusMax = m_fFieldMaxRange;
		desc.iPerTick = 1;
		desc.fDurationSec = 0.15f;
		desc.bUseForwardDir = false;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_LianhuoSpawnerFirePlain, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pRandomFirePlain = static_cast<CSkillObjectSpawner_RandomXZ*>(pResult);
	}
	// Oneshot
	{
		CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_LianhuoSpawnerXSpace, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pXSpaceSpawner = static_cast<CSingleSkillSpawner*>(pResult);
	}
	// Oneshot
	{
		CSingleSkillSpawner::SPAWNER_COPY_DESC desc{};
		desc.iLevelIndex = iLevelId;
		desc.iSpawnLevelIndex = iLevelId;

		CBase* pResult = m_pGameInstance->Clone_Prototype(EPrototypeType::GAMEOBJECT,
			iLevelId, g_wszSpawner_LianhuoSpawnerStunChain, &desc);
		if (pResult == nullptr)
			return E_FAIL;

		m_pStunChainSpawner = static_cast<CSingleSkillSpawner*>(pResult);
	}
	return S_OK;
}

void CLianhuo_GimmikController::Spawn_RandomSkill(const _float fTimeDelta)
{
	CGameObject* pOwner = Get_Owner();
	if (pOwner == nullptr || pOwner->IsDead())
		return;

	if (m_tFirePlainTimer.Tick(fTimeDelta))
	{
		Trigger_FirePlain();

		const _float fNextDelay = m_pGameInstance->Rand_Float(8.f, 17.f);
		m_tFirePlainTimer.Start(fNextDelay);
	}
	if (m_tChainThronTimer.Tick(fTimeDelta))
	{
		Trigger_ChainThron();

		const _float fNextDelay = m_pGameInstance->Rand_Float(8.f, 18.f);
		m_tChainThronTimer.Start(fNextDelay);
	}
}

void CLianhuo_GimmikController::Trigger_FirePlain()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = m_vSpawnPosition;
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pRandomFirePlain->Trigger(desc);
}

void CLianhuo_GimmikController::Trigger_ChainThron()
{
	_uint iLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pOwner = Get_Owner();
	if (pOwner->IsDead())
		return;

	CSkillObjectSpawnerBase::SPAWNER_COPY_DESC desc{};
	desc.iLevelIndex = iLevelIndex;
	desc.iSpawnLevelIndex = iLevelIndex;
	desc.vOrigin = m_vSpawnPosition;
	desc.vForward = pOwner->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::LOOK);
	m_pRandomChainThron->Trigger(desc);
}

void CLianhuo_GimmikController::On_ModelAnimNotify(const AnimNotifyKey& key)
{

}

CLianhuo_GimmikController* CLianhuo_GimmikController::Create()
{
	CLianhuo_GimmikController* pInstance = new CLianhuo_GimmikController();
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CLianhuo_GimmikController::Create(), Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CLianhuo_GimmikController::Clone(void* pArg)
{
	CLianhuo_GimmikController* pInstance = new CLianhuo_GimmikController(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CLianhuo_GimmikController::Create(), Clone");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLianhuo_GimmikController::Free()
{
	Safe_Release(m_pRandomFirePlain);
	Safe_Release(m_pRandomChainThron);
	Safe_Release(m_pXSpaceSpawner);
	Safe_Release(m_pStunChainSpawner);
	Super::Free();
}