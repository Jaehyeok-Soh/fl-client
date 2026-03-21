#include "pch.h"
#include "Quest_Scenario.h"

#include "GameInstance.h"

#include "Quest_Chapter.h"

CQuest_Scenario::CQuest_Scenario()
	: Super(),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}
HRESULT CQuest_Scenario::Initialize(DTO::QUESTDESC desc)
{
	m_tQuestDesc = desc;
	m_tQuestDesc.eType = DTO::QUESTLAYER::SCENARIO;
	m_tQuestDesc.eState = DTO::QUESTSTATE::LOCKED;

	return S_OK;
}

void CQuest_Scenario::Enter()
{
	m_iCurChapterId = m_iFirstChapterId;
	m_pCurChapter = m_chapter[m_iFirstChapterId];

	m_tQuestDesc.eState = DTO::QUESTSTATE::IN_PROGRESS;

	m_isComplete = false;

	if (m_pCurChapter)
		m_pCurChapter->Enter();
}

void CQuest_Scenario::Exit()
{
	m_tQuestDesc.eState = DTO::QUESTSTATE::COMPLETE;
}

void CQuest_Scenario::Change_Chapter()
{
	if (m_pCurChapter)
		m_pCurChapter->Exit();
	
	_int nextId = m_pCurChapter->GetDesc().tQuestDesc.iNextId;

	if (nextId < 0 || m_chapter.find(nextId) == m_chapter.end())
	{
		m_isComplete = true;
		return;
	}

	m_iCurChapterId = nextId;
	
	m_pCurChapter = m_chapter[m_iCurChapterId];
	m_pCurChapter->Enter();

	m_pGameInstance->Broadcast<QUEST_CHANGE_CHAPTER_NOTIFY>();
}

void CQuest_Scenario::Change_Chapter(_int changeChapterId)
{
	if (m_chapter.find(changeChapterId) == m_chapter.end())
		return;

	if (m_pCurChapter)
		m_pCurChapter->Exit();

	m_iCurChapterId = changeChapterId;

	m_pCurChapter = m_chapter[m_iCurChapterId];
	m_pCurChapter->Enter();

	m_pGameInstance->Broadcast<QUEST_CHANGE_CHAPTER_NOTIFY>();
}

_bool CQuest_Scenario::IsComplete()
{
	return m_isComplete;
}

void CQuest_Scenario::Register_QuestObject(DTO::QUEST_CHAPTERDESC chapterDesc, CGameObject* pObj)
{
	if (chapterDesc.tQuestDesc.iId < 0 || m_chapter.find(chapterDesc.tQuestDesc.iId) != m_chapter.end())
		return;

	m_chapter[chapterDesc.tQuestDesc.iId] = CQuest_Chapter::Create(chapterDesc, pObj);
}

void CQuest_Scenario::UpdateProgress(DTO::QUEST_EVENT_SIGNATURE ID)
{
	if (m_chapter[m_iCurChapterId] == nullptr || m_chapter.find(m_iCurChapterId) == m_chapter.end())
		return;

	m_chapter[m_iCurChapterId]->UpdateProgress(ID);
	if (m_chapter[m_iCurChapterId]->IsComplete())
	{
		Change_Chapter();
	}
}

CQuest_Scenario* CQuest_Scenario::Create(DTO::QUESTDESC desc)
{
	CQuest_Scenario* pInstance = new CQuest_Scenario();
	if (FAILED(pInstance->Initialize(desc)))
	{
		MSG_BOX("CQuest_Scenario::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuest_Scenario::Free()
{
	for (auto& chapter : m_chapter)
		Safe_Release(chapter.second);

	m_chapter.clear();

	Safe_Release(m_pGameInstance);

	Super::Free();
}
