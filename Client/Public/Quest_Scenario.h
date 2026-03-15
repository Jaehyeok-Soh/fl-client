#pragma once
#include "Base.h"
#include "Quest_DataModel.h"

NS_BEGIN(Client)

class CQuest_Chapter;

class CQuest_Scenario final : public CBase
{
	using Super = CBase;
private:
	CQuest_Scenario();
	virtual ~CQuest_Scenario() = default;

public:
	HRESULT Initialize(DTO::QUESTDESC desc);

public:
	void Enter();
	void Exit();

	void UpdateProgress(DTO::QUEST_EVENT_SIGNATURE ID);

	void Change_Chapter();
	_bool IsComplete();

	CQuest_Chapter* GetCurrentChapter() { return m_pCurChapter; }

	void Register_QuestObject(DTO::QUEST_CHAPTERDESC chapterDesc, class CGameObject* pObj);

public:
	DTO::QUESTDESC GetDesc() { return m_tQuestDesc; }

private:
	CGameInstance* m_pGameInstance = { nullptr };

	DTO::QUESTDESC m_tQuestDesc = {};

	_int m_iCurChapterId = {};
	CQuest_Chapter* m_pCurChapter = { nullptr };

	_int m_iFirstChapterId = { 0 };

	_bool m_isComplete = { false };

	unordered_map<_int, CQuest_Chapter*> m_chapter;

public:
	static CQuest_Scenario* Create(DTO::QUESTDESC desc);
	virtual void Free() override;
};

NS_END