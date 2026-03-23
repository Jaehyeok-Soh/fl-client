#include "pch.h"
#include "QuestManager.h"

#include "GameInstance.h"

#include "Quest_Scenario.h"
#include "Quest_Chapter.h"

IMPLEMENT_SINGLETON(CQuestManager)

CQuestManager::CQuestManager()
	: Super(),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuestManager::Initialize()
{
	Bind_Events();

	return S_OK;
}

void CQuestManager::Start_Quest(_int iStartScenarioId, _int iStartChapterId)
{
	if (m_scenario.find(iStartScenarioId) == m_scenario.end())
		return;

	Initialize();

	m_iCurScenarioId = iStartScenarioId;
	m_pCurScenario = m_scenario[iStartScenarioId];

	if (iStartChapterId != -1)
		m_pCurScenario->Enter(iStartChapterId);
	else
		m_pCurScenario->Enter();

	m_pGameInstance->Broadcast<QUEST_CHANGE_SCENARIO_NOTIFY>();
}

void CQuestManager::Register_Scenario(DTO::QUESTDESC scenarioDesc)
{
	if (scenarioDesc.iId < 0 || m_scenario.find(scenarioDesc.iId) != m_scenario.end())
		return;

	m_scenario[scenarioDesc.iId] = CQuest_Scenario::Create(scenarioDesc);
}

void CQuestManager::Register_QuestObject(DTO::QUEST_CHAPTERDESC chapterDesc, CGameObject* pObj)
{
	if (chapterDesc.tQuestDesc.iParentId < 0 || m_scenario.find(chapterDesc.tQuestDesc.iParentId) == m_scenario.end())
		return;

	m_scenario[chapterDesc.tQuestDesc.iParentId]->Register_QuestObject(chapterDesc, pObj);
}

void CQuestManager::Change_Scenario()
{
	m_pCurScenario->Exit();

	_int nextId = m_pCurScenario->GetDesc().iNextId;

	if (nextId < 0 || m_scenario.find(nextId) == m_scenario.end())
	{
		m_pCurScenario = { nullptr };
		m_iCurScenarioId = -1;

		m_pGameInstance->Broadcast<QUEST_ALL_COMPLETE>();

		return;
	}

	m_iCurScenarioId = nextId;

	m_pCurScenario = m_scenario[m_iCurScenarioId];
	m_pCurScenario->Enter();

	m_pGameInstance->Broadcast<QUEST_CHANGE_SCENARIO_NOTIFY>();
}

DTO::QUEST_INFOBUCKET CQuestManager::Get_QuestInfo()
{
	DTO::QUEST_INFOBUCKET desc;

	if (m_pCurScenario == nullptr)
		return desc;

	if (m_pCurScenario->GetCurrentChapter() == nullptr)
		return desc;

	desc.tScenarioInfo = m_pCurScenario->GetDesc();
	desc.tChapterInfo = m_pCurScenario->GetCurrentChapter()->GetDesc();

	return desc;
}

DTO::QUEST_CHAPTERDESC CQuestManager::Get_QuestChapterInfo()
{
	DTO::QUEST_CHAPTERDESC desc;

	if (m_pCurScenario == nullptr)
		return desc;

	if (m_pCurScenario->GetCurrentChapter() == nullptr)
		return desc;

	desc = m_pCurScenario->GetCurrentChapter()->GetDesc();

	return desc;
}

void CQuestManager::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<QUEST_NOTIFY>([this](DTO::QUEST_EVENT_SIGNATURE ID)
			{
				this->EventCallback(ID);
			})
	);
}

void CQuestManager::EventCallback(DTO::QUEST_EVENT_SIGNATURE ID)
{
	if (m_scenario[m_iCurScenarioId] == nullptr || m_scenario.find(m_iCurScenarioId) == m_scenario.end())
		return;

	m_scenario[m_iCurScenarioId]->UpdateProgress(ID);

	if (m_scenario[m_iCurScenarioId]->IsComplete())
		Change_Scenario();
}

void CQuestManager::Free()
{
	for (auto& handle : m_vecEventHandles)
		m_pGameInstance->Unsubscribe<QUEST_NOTIFY>(handle);
	m_vecEventHandles.clear();

	for (auto& scenario : m_scenario)
		Safe_Release(scenario.second);

	m_scenario.clear();

	Safe_Release(m_pGameInstance);

	Super::Free();
}
