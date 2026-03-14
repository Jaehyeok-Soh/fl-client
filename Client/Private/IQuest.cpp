#include "pch.h"
#include "Engine_Define.h"
#include "IQuest.h"

IQuest::IQuest()
{
}

void IQuest::OnQuestEnter(DTO::QUEST_CHAPTERDESC desc)
{
	Set_Quest_Enable();

	m_iScenarioId = desc.tQuestDesc.iParentId;
	m_iChapterId = desc.tQuestDesc.iId;

	m_eQuestEvent = desc.eEvent;
	m_eTargetType = desc.eTargetType;

	QuestEnter();
}

void IQuest::OnQuestExit()
{
	Set_Quest_Disable();
	QuestExit();
}

void IQuest::Set_Quest_Enable()
{
	m_bIsEnabled = true;
}

void IQuest::Set_Quest_Disable()
{
	m_bIsEnabled = false;
}

_bool IQuest::Is_Quest_Enabled()
{
	return m_bIsEnabled;
}
