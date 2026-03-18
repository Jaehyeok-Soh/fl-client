#include "pch.h"
#include "DialogueManager.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CDialogueManager)

CDialogueManager::CDialogueManager()
	: Super(),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CDialogueManager::Initialize()
{
	Bind_Events();

	Ready_Dialogue();

	return S_OK;
}

void CDialogueManager::Ready_Dialogue()
{
	m_umapContents.clear();

	CreateNode(0, -1, -1, L"마령 판신", L"눈을 떠, 일어나!.")
		//.AddChoice(L"눈을 뜨지 않는다.", 10)
		//.AddChoice(L"눈을 뜬다.", 15)
		//.AddTrigger(0)
		;
	
	CreateNode(1, -1, -1, L"마령 판신", L"마을에 필토이드들이 습격했어 얼른!.");
	CreateNode(2, -1, -1, L"비타", L"(일어난다.)");
	CreateNode(3, -1, -1, L"마령 판신", L"나는 마령 판신!");

	CreateNode(4, 1, -1, L"마령 판신", L"다음대화");
}

void CDialogueManager::Bind_Events()
{
	m_arrEventHandles[EDialogueInnerEvent::SELECT] = (
		m_pGameInstance->Subscribe<DIALOGUE_SELECT>([this](_int iIndex)
			{
				this->OnSelectChoice(iIndex);
			})
	);

	m_arrEventHandles[EDialogueInnerEvent::NEXT] = (
		m_pGameInstance->Subscribe<DIALOGUE_NEXT>([this]()
			{
				this->OnInputNext();
			})
	);

	m_arrEventHandles[EDialogueInnerEvent::PREV] = (
		m_pGameInstance->Subscribe<DIALOGUE_PREV>([this]()
			{
				this->OnInputPrev();
			})
	);

	m_arrEventHandles[EDialogueInnerEvent::CANCEL] = (
		m_pGameInstance->Subscribe<DIALOGUE_CANCEL>([this]()
			{
				this->CancelCallback();
			})
	);
}

void CDialogueManager::Start_Dialogue(_int iId)
{
	auto iter = m_umapContents.find(iId);
	if (iter == m_umapContents.end())
	{
		FinishCurrentDialogue();
		return;
	}

	m_iCurrentNode = iId;
	m_pCurrentNode = &m_umapContents[iId];

	m_pGameInstance->Broadcast<DIALOGUE_BEGIN>(iId);

	FlushTrigger();
}

void CDialogueManager::OnInputNext()
{
	if (m_pCurrentNode->iNextId == -1)
	{
		FinishCurrentDialogue();
		return;
	}

	m_iCurrentNode = m_pCurrentNode->iNextId;
	m_pCurrentNode = &m_umapContents[m_iCurrentNode];

	FlushTrigger();
}

void CDialogueManager::OnInputPrev()
{
	if (m_pCurrentNode->iPrevId == -1)
	{
		FinishCurrentDialogue();
		return;
	}

	m_iCurrentNode = m_pCurrentNode->iPrevId;
	m_pCurrentNode = &m_umapContents[m_iCurrentNode];

	FlushTrigger();
}

void CDialogueManager::OnSelectChoice(_int choiceIndex)
{
	if (m_pCurrentNode->vecChoices.size() == 0
		|| m_pCurrentNode->vecChoices.size() < choiceIndex
		|| 0 > choiceIndex)
	{
		FinishCurrentDialogue();
		return;
	}

	m_iCurrentNode = m_pCurrentNode->vecChoices[choiceIndex].iTransitionId;
	m_pCurrentNode = &m_umapContents[m_iCurrentNode];

	FlushTrigger();
}

void CDialogueManager::FinishCurrentDialogue()
{
	m_iCurrentNode = -1;
	m_pCurrentNode = nullptr;

	FinishBroadcast();
}

void CDialogueManager::CancelCallback()
{
	FinishCurrentDialogue();
}

void CDialogueManager::FinishBroadcast()
{
	m_pGameInstance->Broadcast<DIALOGUE_END>();
}

void CDialogueManager::FlushTrigger()
{
	// broad cast global event

	return;

	for (auto& id : *m_pCurrentNode->GetChoices())
	{

	}
}

DIALOGUE_NODE& CDialogueManager::CreateNode(_int nodeId, _int prevId, _int nextId, const wstring& speakerName, const wstring& content)
{
	DIALOGUE_NODE node{};
	node.iNodeId = nodeId;
	node.iPrevId = prevId;
	node.iNextId = nextId;
	node.wstrSpeakerName = speakerName;
	node.wstrContentText = content;

	m_umapContents[nodeId] = node;
	return m_umapContents[nodeId];
}

void CDialogueManager::Free()
{
	m_pGameInstance->Unsubscribe<DIALOGUE_SELECT>(m_arrEventHandles[EDialogueInnerEvent::SELECT]);
	m_pGameInstance->Unsubscribe<DIALOGUE_NEXT>(m_arrEventHandles[EDialogueInnerEvent::NEXT]);
	m_pGameInstance->Unsubscribe<DIALOGUE_PREV>(m_arrEventHandles[EDialogueInnerEvent::PREV]);
	m_pGameInstance->Unsubscribe<DIALOGUE_CANCEL>(m_arrEventHandles[EDialogueInnerEvent::CANCEL]);

	Safe_Release(m_pGameInstance);

	Super::Free();
}
