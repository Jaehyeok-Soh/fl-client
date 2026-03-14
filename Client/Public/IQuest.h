#pragma once
#include "Quest_DataModel.h"

NS_BEGIN(Client)

class IQuest
{
public:
	IQuest();
	~IQuest() = default;

public:
	void OnQuestEnter(DTO::QUEST_CHAPTERDESC desc);
	void OnQuestExit();

	void Set_Quest_Enable();
	void Set_Quest_Disable();

	_bool Is_Quest_Enabled();

	virtual void QuestEnter() PURE;
	virtual void QuestExit() PURE;

private:
	DTO::QUESTEVENT::Enum m_eQuestEvent = DTO::QUESTEVENT::AREA_ENTER;
	DTO::QUEST_TARGETTYPE::Enum m_eTargetType = DTO::QUEST_TARGETTYPE::TRIGGER_BOX;

	_int m_iScenarioId = { -1 };
	_int m_iChapterId = { -1 };

	_bool m_bIsEnabled = { false };
};

NS_END