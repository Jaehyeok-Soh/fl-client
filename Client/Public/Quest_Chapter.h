#pragma once
#include "Base.h"

NS_BEGIN(Client)

class CQuest_Chapter final : public CBase
{
	using Super = CBase;
private:
	CQuest_Chapter();
	virtual ~CQuest_Chapter() = default;

public:
	HRESULT Initialize(QUEST_CHAPTERDESC desc, class CGameObject* pObj);

public:
	void Enter();
	void Exit();

	void UpdateProgress(QUEST_EVENT_SIGNATURE ID);

	_bool IsComplete();

public:
	QUEST_CHAPTERDESC GetDesc() { return m_tDesc; }

private:
	class CGameInstance* m_pGameInstance = { nullptr };

	QUEST_CHAPTERDESC m_tDesc = {};

public:
	static CQuest_Chapter* Create(QUEST_CHAPTERDESC desc, class CGameObject* pObj);
	virtual void Free() override;
};

NS_END