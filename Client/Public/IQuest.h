#pragma once
#include "Quest_DataModel.h"

NS_BEGIN(Client)

class IQuest abstract
{
public:
	virtual ~IQuest() = default;

public:
	void OnQuestEnter(DTO::QUEST_CHAPTERDESC desc);
	void OnQuestExit();

	void Set_Quest_Enable() { m_bIsQuestEnabled = true; }
	void Set_Quest_Disable() { m_bIsQuestEnabled = false; }

	_bool Is_Quest_Enabled() { return m_bIsQuestEnabled; }

	virtual void QuestEnter() PURE;
	virtual void QuestExit() PURE;

protected:
	void CallQuestEvent(OBJECT_ENUM_TAG::Enum eTag, _int iCount);

protected:
	DTO::QUESTEVENT::Enum m_eQuestEvent = DTO::QUESTEVENT::AREA_ENTER;

	_int m_iScenarioId = { -1 };
	_int m_iChapterId = { -1 };

	_bool m_bIsQuestEnabled = { false };
};

NS_END