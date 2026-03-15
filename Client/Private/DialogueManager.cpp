#include "pch.h"
#include "DialogueManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CDialogueManager)

CDialogueManager::CDialogueManager()
	: Super(),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CDialogueManager::Initialize()
{
	Bind_Events();

	return S_OK;
}

void CDialogueManager::Bind_Events()
{
	//m_vecEventHandles.push_back(
	//	m_pGameInstance->Subscribe<QUEST_NOTIFY>([this](DTO::QUEST_EVENT_SIGNATURE ID)
	//		{
	//			this->EventCallback(ID);
	//		})
	//);
}

void CDialogueManager::EventCallback()
{
}

void CDialogueManager::Free()
{
	for (auto& handle : m_vecEventHandles)
		m_pGameInstance->Unsubscribe<QUEST_NOTIFY>(handle);
	m_vecEventHandles.clear();

	Safe_Release(m_pGameInstance);

	Super::Free();
}
