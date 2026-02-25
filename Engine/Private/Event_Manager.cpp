#include "Engine_pch.h"
#include "Event_Manager.h"
#include "Level.h"
#include "GameObject.h"
#include "GameInstance.h"

CEvent_Manager::CEvent_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CEvent_Manager::Initialize()
{
	return S_OK;
}

void CEvent_Manager::Push_SpawnEvent(const SpawnEventDesc& desc)
{
	if (m_bFlushing == false)
		m_queSpawn.push(desc);
	else
		m_queSpawn_Pending.push(desc);
}

void CEvent_Manager::Push_DespawnEvent(const DespawnEventDesc& desc)
{
	if (m_bFlushing == false)
		m_queDespawn.push(desc);
	else
		m_queDespawn_Pending.push(desc);
}

void CEvent_Manager::Push_ChangeLevelEvet(const ChangeLevelEventDesc& desc)
{
	if (m_bFlushing == false)
		m_queChangeLevel.push(desc);
	else
		m_queChangeLevel_Pending.push(desc);
}

void CEvent_Manager::Flush_All()
{
	m_bFlushing = true;

	const _bool bHasLevelChange = !m_queChangeLevel.empty() || !m_queChangeLevel_Pending.empty();

	if (bHasLevelChange)
	{
		// 삭제는 처리
		Flush(EventType::OBJECT_REMOVE);

		_bool bShouldChange = { false };
		ChangeLevelEventDesc shouldLevelChangeDesc = {};

		// 이번프레임에 들어온것 중 후처리 우선
		if (m_queChangeLevel_Pending.empty() == false)
		{
			shouldLevelChangeDesc = m_queChangeLevel_Pending.back();
			m_queChangeLevel_Pending.pop();
			bShouldChange = true;
		}
		else if (m_queChangeLevel.empty() == false)
		{
			shouldLevelChangeDesc = m_queChangeLevel.back();
			m_queChangeLevel.pop();
			bShouldChange = true;
		}
				

		// 나머지 필요없으니 폐기
		Clear_All();
		m_bFlushing = false;

		if (bShouldChange)
			Change_Level(shouldLevelChangeDesc);

		return;
	}
	else
	{
		// Delete 우선
		Flush(EventType::OBJECT_REMOVE);
		Flush(EventType::OBJECT_CREATE);
	}

	m_bFlushing = false;

	// Delete 우선
	Flush_Pending(EventType::OBJECT_REMOVE);
	Flush_Pending(EventType::OBJECT_CREATE);
}

void CEvent_Manager::Clear(EventType eType)
{
	switch (eType)
	{
	case EventType::OBJECT_CREATE:
	{
		while (!m_queSpawn.empty())
		{
			Clear_SpawnEvent(m_queSpawn.front());
			m_queSpawn.pop();
		}
		while (!m_queSpawn_Pending.empty())
		{
			Clear_SpawnEvent(m_queSpawn_Pending.front());
			m_queSpawn_Pending.pop();
		}
	} break;
	case EventType::OBJECT_REMOVE:
	{
		while (!m_queDespawn.empty()) m_queDespawn.pop();
		while (!m_queDespawn_Pending.empty()) m_queDespawn_Pending.pop();
	} break;
	case EventType::LEVEL_CHANGE:
	{
		while (!m_queChangeLevel.empty())
		{
			Clear_ChangeLevelEvent(m_queChangeLevel.front());
			m_queChangeLevel.pop();
		}
		while (!m_queChangeLevel_Pending.empty())
		{
			Clear_ChangeLevelEvent(m_queChangeLevel_Pending.front());
			m_queChangeLevel_Pending.pop();
		}
	} break;
	default:
		MSG_BOX("CEvent_Manager::Clear, type is invalid!");
		break;
	}
}

void CEvent_Manager::Clear_All()
{
	Clear(EventType::OBJECT_CREATE);
	Clear(EventType::OBJECT_REMOVE);
	Clear(EventType::LEVEL_CHANGE);
}

void CEvent_Manager::Flush(EventType eType)
{
	switch (eType)
	{
	case EventType::OBJECT_CREATE:
	{
		while (!m_queSpawn.empty())
		{
			Spawn_GameObject(m_queSpawn.front());
			m_queSpawn.pop();
		}
	} break;
	case EventType::OBJECT_REMOVE:
	{
		while (!m_queDespawn.empty())
		{
			Despawn_GameObject(m_queDespawn.front());
			m_queDespawn.pop();
		}
	} break;
	default:
		MSG_BOX("CEvent_Manager::Flush, type is Invalid!");
		break;
	}
}

void CEvent_Manager::Flush_Pending(EventType eType)
{
	switch (eType)
	{
	case EventType::OBJECT_CREATE:
	{
		while (!m_queSpawn_Pending.empty())
		{
			Spawn_GameObject(m_queSpawn_Pending.front());
			m_queSpawn_Pending.pop();
		}
	} break;
	case EventType::OBJECT_REMOVE:
	{
		while (!m_queDespawn_Pending.empty())
		{
			Despawn_GameObject(m_queDespawn_Pending.front());
			m_queDespawn_Pending.pop();
		}
	} break;
	default:
		MSG_BOX("CEvent_Manager::Flush, type is Invalid!");
		break;
	}
}
HRESULT CEvent_Manager::Spawn_GameObject(SpawnEventDesc& spawnDesc)
{
	if (!spawnDesc.pClone)
	{
		MSG_BOX("CEvent_Manager::Spawn_GameObject, clone is invalid");
		return E_FAIL;
	}

	CGameObject* pResult = m_pGameInstance->Add_GameObject(spawnDesc.iCloneLevelIndex, spawnDesc.wstrLayerTag, spawnDesc.pClone);
	if (!pResult)
	{
		MSG_BOX("CEvent_Manager::Spawn_GameObject, add layer failed");
		return E_FAIL;
	}

	if (spawnDesc.callback)
		spawnDesc.callback(pResult);

	return S_OK;
}

HRESULT CEvent_Manager::Despawn_GameObject(DespawnEventDesc& despawnDesc)
{
	if (!despawnDesc.pGo)
	{
		MSG_BOX("CEvent_Manager::Despawn_GameObject, gameObject is invalid");
		return E_FAIL;
	}

	m_pGameInstance->Immediately_DeleteGameObject(despawnDesc.iClonedLevelIndex, despawnDesc.wstrLayerTag, despawnDesc.pGo);
	return S_OK;
}

HRESULT CEvent_Manager::Change_Level(ChangeLevelEventDesc& changeLevelDesc)
{
	if (!changeLevelDesc.pNewLevel)
	{
		MSG_BOX("CEvent_Manager::Change_Level, newLevel is invalid");
		return E_FAIL;
	}

	m_pGameInstance->Immediately_ChangeLevel(changeLevelDesc.iNewLevelID, changeLevelDesc.pNewLevel);
	return S_OK;
}

void CEvent_Manager::Clear_SpawnEvent(SpawnEventDesc& spawnDesc)
{
	if (!spawnDesc.pClone)
		return;

	Safe_Release(spawnDesc.pClone);
	spawnDesc.pClone = nullptr;
}

void CEvent_Manager::Clear_ChangeLevelEvent(ChangeLevelEventDesc& changeLevelDesc)
{
	if (!changeLevelDesc.pNewLevel)
		return;

	Safe_Release(changeLevelDesc.pNewLevel);
	changeLevelDesc.pNewLevel = nullptr;
}

CEvent_Manager* CEvent_Manager::Create()
{
	CEvent_Manager* pInstance = new CEvent_Manager;
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CEvent_Manager::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEvent_Manager::Free()
{
	Clear_All();
	Safe_Release(m_pGameInstance);
	Super::Free();
}
