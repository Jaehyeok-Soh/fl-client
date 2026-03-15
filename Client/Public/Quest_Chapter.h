#pragma once
#include "Base.h"
#include "Quest_DataModel.h"

NS_BEGIN(Client)

class CQuest_Chapter final : public CBase
{
	using Super = CBase;
private:
	CQuest_Chapter();
	virtual ~CQuest_Chapter() = default;

public:
	HRESULT Initialize(DTO::QUEST_CHAPTERDESC desc, class CGameObject* pObj);

public:
	void Enter();
	void Exit();

	void UpdateProgress(DTO::QUEST_EVENT_SIGNATURE ID);

	_bool IsComplete();

public:
	DTO::QUEST_CHAPTERDESC GetDesc() { return m_tDesc; }

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	DTO::QUEST_CHAPTERDESC m_tDesc = {};

public:
	static CQuest_Chapter* Create(DTO::QUEST_CHAPTERDESC desc, class CGameObject* pObj);
	virtual void Free() override;
};

NS_END