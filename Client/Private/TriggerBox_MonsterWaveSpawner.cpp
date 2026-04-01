#include "pch.h"
#include "TriggerBox_MonsterWaveSpawner.h"
#include "Transform.h"
#include "GameInstance.h"
#include "Level_Loading.h"

//=================
// Builder
//=================
#include "Builder_Map.h"

//=================
// Monster
//=================
#include "Monster_Base.h"
#include "Monster_Body_Base.h"
#include "MonsterState_Factory.h"
#include "MonsterControlContext.h"
#include "MonsterActionState.h"

#include "Monster_Dog.h"
#include "Monster_Boomer.h"
#include "Monster_Fly.h"
#include "Monster_Veteran.h"

CTriggerBox_MonsterWaveSpawner::CTriggerBox_MonsterWaveSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CTriggerBox(pDevice, pContext)
	, m_tWaveData{}
{
	m_eTriggerBoxType = CTriggerBox::Type::MONSTER_SPAWNER;
}

CTriggerBox_MonsterWaveSpawner::CTriggerBox_MonsterWaveSpawner(const CTriggerBox_MonsterWaveSpawner& rhs)
	: CTriggerBox(rhs)
	, m_tWaveData{ rhs.m_tWaveData }
{
}

HRESULT CTriggerBox_MonsterWaveSpawner::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	Set_Object_Enum_Tag(OBJECT_ENUM_TAG::TRIGGER_BOX_MILESTONE_DEFAULT);

	return S_OK;
}

HRESULT CTriggerBox_MonsterWaveSpawner::Initialize(void* pArg)
{
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	MONSTERWAVESPAWNER_DESC* pDesc = static_cast<MONSTERWAVESPAWNER_DESC*>(pArg);

	m_tWaveData = pDesc->tWaveData;

	if (FAILED(Ready_Component(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_SpawnPool(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTriggerBox_MonsterWaveSpawner::Ready_Component(MONSTERWAVESPAWNER_DESC* pDesc)
{
	return S_OK;
}

HRESULT CTriggerBox_MonsterWaveSpawner::Ready_SpawnPool(MONSTERWAVESPAWNER_DESC* pDesc)
{
	_uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);

	map<DTO::EMakeMonsterType, _int> poolAggregate;

	for (auto& waveInfo : m_tWaveData.vecWaveInfo)
	{
		for (auto& tData : waveInfo.vecMonsterSpawnData)
		{
			poolAggregate[tData.eMakeMonsterType]++;
			//auto item = poolAggregate.find(tData.eMakeMonsterType);
			//(*item).second += 1;
		}
	}

	for (auto& item : poolAggregate)
	{
		if (FAILED(Register_Pool(pDesc->iLevelIndex, iFindPrototypeIndex, item.first, item.second)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTriggerBox_MonsterWaveSpawner::Register_Pool(_uint iLevelId, _uint iFindPrototypeIndex, DTO::EMakeMonsterType eMakeMonsterType, _int numPool)
{
	switch (eMakeMonsterType)
	{
	case DTO::EMakeMonsterType::Dog:
	{
		auto desc = CMonster_Dog::Get_PreSetDesc(iLevelId);
		m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Dog, g_wszMonstereLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Dog_Prototype_Tag, &desc, numPool + 100);
	}
	break;
	case DTO::EMakeMonsterType::Boomer:
	{
		auto desc = CMonster_Boomer::Get_PreSetDesc(iLevelId);
		m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Boomer, g_wszMonstereLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Boomer_Prototype_Tag, &desc, numPool + 100);
	}
	break;
	case DTO::EMakeMonsterType::Shooter:
		break;
	case DTO::EMakeMonsterType::Fly:
	{
		auto desc = CMonster_Fly::Get_PreSetDesc(iLevelId);
		m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Fly, g_wszMonstereLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Fly_Prototype_Tag, &desc, numPool + 60);
	}
	break;
	case DTO::EMakeMonsterType::Veteran:
	{
		auto desc = CMonster_Veteran::Get_PreSetDesc(iLevelId);
		m_pGameInstance->Regist_Pool(iLevelId, g_wszPool_Monster_Veteran, g_wszBossLayer, ENUM_TO_UINT(ELevelType::STATIC), g_wszMonster_Veteran_Prototype_Tag, &desc, numPool + 1);
	}
	break;
	case DTO::EMakeMonsterType::Xibi:
		break;
	case DTO::EMakeMonsterType::END:
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CTriggerBox_MonsterWaveSpawner::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CTriggerBox_MonsterWaveSpawner::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CTriggerBox_MonsterWaveSpawner::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	if (m_bIsAction)
	{
		switch (m_tWaveData.eType)
		{
		case MONSTERSPAWN_WAVE_TYPE::LOOP:
			Update_WaveLoop(fTimeDelta);
			break;
		case MONSTERSPAWN_WAVE_TYPE::ALL_KILL:
			Update_WaveAllKill(fTimeDelta);
			break;
		case MONSTERSPAWN_WAVE_TYPE::TIMER:
			Update_WaveTimer(fTimeDelta);
			break;
		default:
			break;
		}
	}
}

void CTriggerBox_MonsterWaveSpawner::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CTriggerBox_MonsterWaveSpawner::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CTriggerBox_MonsterWaveSpawner::Render()
{
	return S_OK;
}

void CTriggerBox_MonsterWaveSpawner::OnCollision(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	Super::OnCollision(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_MonsterWaveSpawner::OnCollision_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
	Super::OnCollision_Enter(iMyColliderLayer, iOtherLayer, pOther, tHitInfo);
}

void CTriggerBox_MonsterWaveSpawner::OnCollision_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
	if (Super::IsEnabled() == false)
		return;

	Super::OnCollision_Exit(iMyColliderLayer, iOtherLayer, pOther);
}

void CTriggerBox_MonsterWaveSpawner::OnTrigger_Enter(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther, const COL_HIT_INFO& tHitInfo)
{
}

void CTriggerBox_MonsterWaveSpawner::OnTrigger_Exit(_uint iMyColliderLayer, _uint iOtherLayer, CGameObject* pOther)
{
}

void CTriggerBox_MonsterWaveSpawner::QuestEnter()
{
	Super::QuestEnter();

	m_bIsAction = true;

	m_tWaveData.iCurrentWaveCount = 0;
	m_tWaveData.fCurrentWaveTime = 0.f;
}

void CTriggerBox_MonsterWaveSpawner::QuestExit()
{
	Super::QuestExit();
}

HRESULT CTriggerBox_MonsterWaveSpawner::SpawnMonster(MonsterWaveInfo& waveInfo)
{
	_uint iCurLevelIndex = m_pGameInstance->Get_CurrentLevelIndex();
	CGameObject* pResult = { nullptr };

	_uint iFindPrototypeIndex = ENUM_TO_UINT(ELevelType::STATIC);
	wstring wstrAddLayerName{};
	wstring wstrFindPrototypeName{};

	CTransform::TRANSFORM_DESC tTransformDesc = {};

	wstring poolTag = {};

	for (auto& tData : waveInfo.vecMonsterSpawnData)
	{
		tTransformDesc.ScaleMatrix = Matrix::CreateScale(tData.vScale);
		tTransformDesc.TranslationMatrix = Matrix::CreateTranslation(tData.vPosition);
		tTransformDesc.RotationMatrix = Matrix::CreateFromYawPitchRoll(
			XMConvertToRadians(tData.vPitchYawRoll.y), XMConvertToRadians(tData.vPitchYawRoll.x), XMConvertToRadians(tData.vPitchYawRoll.z)
		);

		//if (FAILED(CMonster_Base::Create_Mosnter(CBuilder_Map::Change_MakeMonsterType_To_MonsterType(tData.eMakeMonsterType) , iFindPrototypeIndex , iCurLevelIndex  , &tTransformDesc)))
		//    return E_FAIL;

		switch (tData.eMakeMonsterType)
		{
		case DTO::EMakeMonsterType::Dog:
			poolTag = g_wszPool_Monster_Dog;
			break;
		case DTO::EMakeMonsterType::Boomer:
			poolTag = g_wszPool_Monster_Boomer;
			break;
		case DTO::EMakeMonsterType::Fly:
			poolTag = g_wszPool_Monster_Fly;
			break;
		case DTO::EMakeMonsterType::Veteran:
			poolTag = g_wszPool_Monster_Veteran;
			break;

			// todo
		case DTO::EMakeMonsterType::Shooter:
		case DTO::EMakeMonsterType::Xibi:
		default:
			continue;
		}

		m_pGameInstance->Request_AddObject(iCurLevelIndex, poolTag, iCurLevelIndex, nullptr,
			[this, tTransformDesc](CGameObject* p)
			{
				auto* pMonsterObject = static_cast<CMonster_Base*>(p);
				if (nullptr == pMonsterObject)
					return;
				pMonsterObject->SetSpawnPos(tTransformDesc);
			});
	}

	return S_OK;
}

void CTriggerBox_MonsterWaveSpawner::Update_WaveLoop(_float fTimeDelta)
{
}

void CTriggerBox_MonsterWaveSpawner::Update_WaveAllKill(_float fTimeDelta)
{
}

void CTriggerBox_MonsterWaveSpawner::Update_WaveTimer(_float fTimeDelta)
{
	m_tWaveData.fCurrentWaveTime += fTimeDelta;

	if (m_tWaveData.fCurrentWaveTime >= m_tWaveData.fWaveTime)
	{
		m_bIsAction = false;
		CallQuestEvent(Get_Object_Enum_Tag(), 1);
		return;
	}

	if (m_tWaveData.iCurrentWaveCount >= m_tWaveData.iTotalWaveCount)
		return;

	MonsterWaveInfo& waveInfo = m_tWaveData.vecWaveInfo[m_tWaveData.iCurrentWaveCount];
	_float nextSpawnTime = { -1.f };
	if (m_tWaveData.iCurrentWaveCount + 1 < m_tWaveData.iTotalWaveCount)
		nextSpawnTime = m_tWaveData.vecWaveInfo[m_tWaveData.iCurrentWaveCount + 1].fSpawnTime;
	else
		nextSpawnTime = m_tWaveData.vecWaveInfo[m_tWaveData.iTotalWaveCount - 1].fSpawnTime;

	m_fCurWaveTime += fTimeDelta;

	if (waveInfo.fSpawnTime <= m_tWaveData.fCurrentWaveTime)
	{
		if (waveInfo.iCurrentSpawnCount == 0)
		{
			if (FAILED(SpawnMonster(waveInfo)))
				MSG_BOX("Mosnter Spawner 작동 오류");

			waveInfo.iCurrentSpawnCount++;
			waveInfo.fAccTime = 0.f;
			m_fCurWaveTime = 0.f;
		}
		else
		{
			waveInfo.fAccTime += fTimeDelta;
			if (waveInfo.fAccTime >= waveInfo.fSpawnInterval && waveInfo.iCurrentSpawnCount <= waveInfo.iTotalSpawnCount)
			{


				if (FAILED(SpawnMonster(waveInfo)))
					MSG_BOX("Mosnter Spawner 작동 오류");

				waveInfo.iCurrentSpawnCount++;
				waveInfo.fAccTime = 0.f;
			}
		}
		if (waveInfo.iCurrentSpawnCount >= waveInfo.iTotalSpawnCount && m_tWaveData.fCurrentWaveTime >= nextSpawnTime)
		{
			m_tWaveData.iCurrentWaveCount++;
			m_fCurWaveTime = 0.f;
		}
	}
}

CTriggerBox_MonsterWaveSpawner* CTriggerBox_MonsterWaveSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTriggerBox_MonsterWaveSpawner* pTriggerBox = new CTriggerBox_MonsterWaveSpawner(pDevice, pContext);

	if (FAILED(pTriggerBox->Initialize_Prototype()))
	{
		Safe_Release(pTriggerBox);
		MSG_BOX(" Trigger Box Monster Wave Spawner Is Failed To Craete ");
		return nullptr;
	}

	return pTriggerBox;
}

CGameObject* CTriggerBox_MonsterWaveSpawner::Clone(void* pArg)
{
	CTriggerBox_MonsterWaveSpawner* pTriggerBox = new CTriggerBox_MonsterWaveSpawner(*this);

	if (FAILED(pTriggerBox->Initialize(pArg)))
	{
		Safe_Release(pTriggerBox);
		MSG_BOX(" Trigger Box Monster Wave Spawner Is Failed To Clone ");
		return nullptr;
	}

	return pTriggerBox;
}

void CTriggerBox_MonsterWaveSpawner::Free()
{
	Super::Free();
}
