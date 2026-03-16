#pragma once
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)

class CDialogueManager final : public CBase
{
	DECLARE_SINGLETON(CDialogueManager);

	struct EDialogueInnerEvent
	{
		enum Enum
		{
			SELECT,
			NEXT,
			PREV,
			CANCEL,
			END
		};
	};

	using Super = CBase;
private:
	CDialogueManager();
	virtual ~CDialogueManager() = default;

private:
	HRESULT Initialize();

	void Ready_Dialogue();

	void Bind_Events();

public:
	void Start_Dialogue(_int iId);

	void OnInputNext();
	void OnInputPrev();
	void OnSelectChoice(_int choiceIndex);
	void FinishCurrentDialogue();
	void CancelCallback();
	void FinishBroadcast();

	void FlushTrigger();

	DIALOGUE_NODE* GetDialogue() { return m_pCurrentNode; }

private:
	DIALOGUE_NODE& CreateNode(_int key, _int nodeId,
		_int prevId,
		_int nextId,
		const wstring& speakerName,
		const wstring& content);

private:
	CGameInstance* m_pGameInstance = { nullptr };

	array<DelegateHandle, EDialogueInnerEvent::END> m_arrEventHandles;

	_int m_iCurrentNode = { -1 };
	DIALOGUE_NODE* m_pCurrentNode = { nullptr };
	unordered_map<_int, DIALOGUE_NODE> m_umapContents;

public:
	virtual void Free() override;
};

NS_END