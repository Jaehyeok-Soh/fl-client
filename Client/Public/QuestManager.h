#pragma once
#include "Base.h"
#include "Quest_DataModel.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CQuest_Scenario;

class CQuestManager final : public CBase
{
private:
	DECLARE_SINGLETON(CQuestManager);

	using Super = CBase;
private:
	CQuestManager();
	virtual ~CQuestManager() = default;

private:
	HRESULT Initialize();

	void Bind_Events();
	void EventCallback(DTO::QUEST_EVENT_SIGNATURE ID);

public:
	void Start_Quest(_int iFirstScenarioId);

	void Register_Scenario(DTO::QUESTDESC scenarioDesc);
	void Register_QuestObject(DTO::QUEST_CHAPTERDESC chapterDesc, class CGameObject* pObj);

	void Change_Scenario();

	DTO::QUEST_INFOBUCKET Get_QuestInfo();

private:
	CGameInstance* m_pGameInstance = { nullptr };
	
	vector<DelegateHandle> m_vecEventHandles;

	_int m_iCurScenarioId = { -1 };
	CQuest_Scenario* m_pCurScenario = { nullptr };

	unordered_map<_int, CQuest_Scenario*> m_scenario;

public:
	virtual void Free() override;
};

NS_END