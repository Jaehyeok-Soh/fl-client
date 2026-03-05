#include "Engine_pch.h"
#include "Event_Manager.h"
#include "Level.h"
#include "ObjectPool.h"
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

void CEvent_Manager::Push_AddEvent(const AddEventDesc& desc)
{
	if (m_bFlushing == false)
		m_queAddGameObject.push(desc);
	else
		m_queAddGameObject_Pending.push(desc);
}

void CEvent_Manager::Push_RemoveEvent(const RemoveEventDesc& desc)
{
	if (desc.pGo == nullptr)
		return;

	if (Cancel_AddQueued(desc.pGo))
		return;

	Safe_AddRef(desc.pGo);
	if (m_bFlushing == false)
		m_queRemoveGameObject.push(desc);
	else
		m_queRemoveGameObject_Pending.push(desc);
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
		Flush_Pending(EventType::OBJECT_REMOVE);

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
		Flush(EventType::OBJECT_ADD);
	}

	m_bFlushing = false;

	// Delete 우선
	Flush_Pending(EventType::OBJECT_REMOVE);
	Flush_Pending(EventType::OBJECT_ADD);
}

void CEvent_Manager::Clear(EventType eType)
{
	switch (eType)
	{
	case EventType::OBJECT_ADD:
	{
		while (!m_queAddGameObject.empty())
		{
			Clear_AddEvent(m_queAddGameObject.front());
			m_queAddGameObject.pop();
		}
		while (!m_queAddGameObject_Pending.empty())
		{
			Clear_AddEvent(m_queAddGameObject_Pending.front());
			m_queAddGameObject_Pending.pop();
		}
	} break;
	case EventType::OBJECT_REMOVE:
	{
		while (!m_queRemoveGameObject.empty())
		{
			Clear_RemoveEvent(m_queRemoveGameObject.front());
			m_queRemoveGameObject.pop();
		}
		while (!m_queRemoveGameObject_Pending.empty())
		{
			Clear_RemoveEvent(m_queRemoveGameObject_Pending.front());
			m_queRemoveGameObject_Pending.pop();
		}
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
	Clear(EventType::OBJECT_ADD);
	Clear(EventType::OBJECT_REMOVE);
	Clear(EventType::LEVEL_CHANGE);
}

void CEvent_Manager::Flush(EventType eType)
{
	switch (eType)
	{
	case EventType::OBJECT_ADD:
	{
		while (!m_queAddGameObject.empty())
		{
			Add_GameObject(m_queAddGameObject.front());
			m_queAddGameObject.pop();
		}
	} break;
	case EventType::OBJECT_REMOVE:
	{
		while (!m_queRemoveGameObject.empty())
		{
			Remove_GameObject(m_queRemoveGameObject.front());
			m_queRemoveGameObject.pop();
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
	case EventType::OBJECT_ADD:
	{
		while (!m_queAddGameObject_Pending.empty())
		{
			Add_GameObject(m_queAddGameObject_Pending.front());
			m_queAddGameObject_Pending.pop();
		}
	} break;
	case EventType::OBJECT_REMOVE:
	{
		while (!m_queRemoveGameObject_Pending.empty())
		{
			Remove_GameObject(m_queRemoveGameObject_Pending.front());
			m_queRemoveGameObject_Pending.pop();
		}
	} break;
	default:
		MSG_BOX("CEvent_Manager::Flush, type is Invalid!");
		break;
	}
}
HRESULT CEvent_Manager::Add_GameObject(AddEventDesc& addDesc)
{
	if (!addDesc.pClone)
	{
		MSG_BOX("CEvent_Manager::Add_GameObject, clone is invalid");
		return E_FAIL;
	}

	CGameObject* pResult = m_pGameInstance->Add_GameObject(addDesc.iCloneLevelIndex, addDesc.wstrLayerTag, addDesc.pClone);
	if (!pResult)
	{
		MSG_BOX("CEvent_Manager::Add_GameObject, add layer failed");
		Clear_AddEvent(addDesc);
		return E_FAIL;
	}

	if (addDesc.callback)
		addDesc.callback(pResult);

	return S_OK;
}

HRESULT CEvent_Manager::Remove_GameObject(RemoveEventDesc& removeDesc)
{
	if (removeDesc.pGo == nullptr)
	{
		MSG_BOX("CEvent_Manager::Remove_GameObject, gameObject is invalid");
		return E_FAIL;
	}

	if (removeDesc.pGo->Is_FromPool() == false)
		m_pGameInstance->Immediately_DeleteGameObject(removeDesc.iClonedLevelIndex, removeDesc.pGo);
	else
		m_pGameInstance->Immediately_DespawnGameObject(removeDesc.iClonedLevelIndex, removeDesc.pGo);

	Safe_Release(removeDesc.pGo);
	removeDesc.pGo = nullptr;
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

void CEvent_Manager::Clear_AddEvent(AddEventDesc& addDesc)
{
	if (!addDesc.pClone)
		return;

	if(addDesc.pClone->Is_FromPool() == false)
		Safe_Release(addDesc.pClone);
	else
		addDesc.pClone->Get_OwnerPool()->Despawn(addDesc.pClone);

	addDesc.pClone = nullptr;
}

void CEvent_Manager::Clear_RemoveEvent(RemoveEventDesc& removeDesc)
{
	if (!removeDesc.pGo)
		return;

	Safe_Release(removeDesc.pGo);
	removeDesc.pGo = nullptr;
}

void CEvent_Manager::Clear_ChangeLevelEvent(ChangeLevelEventDesc& changeLevelDesc)
{
	if (!changeLevelDesc.pNewLevel)
		return;

	Safe_Release(changeLevelDesc.pNewLevel);
	changeLevelDesc.pNewLevel = nullptr;
}

_bool CEvent_Manager::Cancel_AddQueued(CGameObject* pGo)
{
	_bool bCancelled{ false };

	{
		std::queue<AddEventDesc> tmp;
		while (m_queAddGameObject.empty() == false)
		{
			AddEventDesc desc = m_queAddGameObject.front();
			m_queAddGameObject.pop();
			if (desc.pClone == pGo)
			{
				Clear_AddEvent(desc);
				bCancelled = true;
			}
			else
				tmp.push(desc);
		}
		m_queAddGameObject.swap(tmp);
	}
	{
		std::queue<AddEventDesc> tmp;
		while (m_queAddGameObject_Pending.empty() == false)
		{
			AddEventDesc desc = m_queAddGameObject_Pending.front();
			m_queAddGameObject_Pending.pop();
			if (desc.pClone == pGo)
			{
				Clear_AddEvent(desc);
				bCancelled = true;
			}
			else
				tmp.push(desc);
		}
		m_queAddGameObject_Pending.swap(tmp);
	}

	return bCancelled;
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
