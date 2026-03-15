#include "pch.h"
#include "Engine_Define.h"
#include "IQuest.h"

#include "GameInstance.h"

void IQuest::OnQuestEnter(DTO::QUEST_CHAPTERDESC desc)
{
	Set_Quest_Enable();

	m_iScenarioId = desc.tQuestDesc.iParentId;
	m_iChapterId = desc.tQuestDesc.iId;

	m_eQuestEvent = desc.eEvent;

	QuestEnter();
}

void IQuest::OnQuestExit()
{
	Set_Quest_Disable();
	QuestExit();
}

void IQuest::CallQuestEvent(OBJECT_ENUM_TAG::Enum eTag, _int iCount)
{
	if (Is_Quest_Enabled() == false)
		return;

	DTO::QUEST_EVENT_SIGNATURE callback;
	callback.eEvent = m_eQuestEvent;
	callback.eTargetType = eTag;
	callback.iCount = iCount;

	CGameInstance::GetInstance()->Broadcast<QUEST_NOTIFY>(callback);
}
