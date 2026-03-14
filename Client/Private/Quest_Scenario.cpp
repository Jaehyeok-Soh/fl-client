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
HRESULT CQuest_Scenario::Initialize(QUESTDESC desc)
{
	m_tQuestDesc = desc;
	m_tQuestDesc.eType = QUESTLAYER::SCENARIO;
	m_tQuestDesc.eState = QUESTSTATE::LOCKED;

	return S_OK;
}

void CQuest_Scenario::Enter()
{
	m_iCurChapterId = m_iFirstChapterId;
	m_pCurChapter = m_chapter[m_iFirstChapterId];

	m_tQuestDesc.eState = QUESTSTATE::IN_PROGRESS;

	if (m_pCurChapter)
		m_pCurChapter->Enter();
}

void CQuest_Scenario::Exit()
{
	m_tQuestDesc.eState = QUESTSTATE::COMPLETE;
}

void CQuest_Scenario::Change_Chapter()
{
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

	m_pGameInstance->Broadcast<QUEST_CHANGE_CHAPTER_NOTIFY>(m_pCurChapter->GetDesc());
}

_bool CQuest_Scenario::IsComplete()
{
	return m_isComplete;
}

void CQuest_Scenario::Register_QuestObject(QUEST_CHAPTERDESC chapterDesc, CGameObject* pObj)
{
	if (chapterDesc.tQuestDesc.iId < 0 || m_chapter.find(chapterDesc.tQuestDesc.iId) != m_chapter.end())
	{
		MSG_BOX("Chapter register failed");
		return;
	}

	m_chapter[chapterDesc.tQuestDesc.iId] = CQuest_Chapter::Create(chapterDesc, pObj);
}

void CQuest_Scenario::UpdateProgress(QUEST_EVENT_SIGNATURE ID)
{
	m_chapter[m_iCurChapterId]->UpdateProgress(ID);
	if (m_chapter[m_iCurChapterId]->IsComplete())
	{
		Change_Chapter();
	}
}

CQuest_Scenario* CQuest_Scenario::Create(QUESTDESC desc)
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
