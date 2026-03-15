#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CDialogueManager final : public CBase
{
	DECLARE_SINGLETON(CDialogueManager);

	using Super = CBase;
private:
	CDialogueManager();
	virtual ~CDialogueManager() = default;

private:
	HRESULT Initialize();

	void Bind_Events();
	void EventCallback();

public:

private:
	CGameInstance* m_pGameInstance = { nullptr };

	vector<DelegateHandle> m_vecEventHandles;

public:
	virtual void Free() override;
};

NS_END