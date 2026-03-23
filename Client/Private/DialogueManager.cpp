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

	// 튜토리얼 진입 대화 : 마령
	{
		CreateNode(0, -1, -1, L"마령 판신", L"눈을 떠, 일어나!.")
			//.AddChoice(L"눈을 뜨지 않는다.", 10)
			//.AddChoice(L"눈을 뜬다.", 15)
			//.AddTrigger(0)
			;

		CreateNode(1, -1, 2, L"마령 판신", L"마을에 필토이드들이 습격했어 얼른!");
		CreateNode(2, 1, 3, L"은비", L"(일어난다.)");
		CreateNode(3, 2, -1, L"마령 판신", L"일단, 집으로 돌아가자!");
	}

	// 마령 일반 대화
	{
		CreateNode(4, -1, -1, L"마령 판신", L"나는 마령 판신!");
	}

	// 튜토리얼 총기 주운 후 대화
	{
		CreateNode(5, -1, 6, L"마령 판신", L"미처 가져가지 못한 총인가봐,\n일단 우리가 챙기자.");
		CreateNode(6, 5, 7, L"마령 판신", L"부둣가에 배는 찾아볼 수 없어.");
		CreateNode(7, 6, 8, L"마령 판신", L"어쩔 수 없지,\n산 위의 배를 타고가자.");
		CreateNode(8, 7, 9, L"은비", L"산 위의..... 배?");
		CreateNode(9, 8, -1, L"마령 판신", L"응, 하늘을 나는 배.\n일단 산 위로 올라가자.");
	}

	// 주점 진입, 퀘스트 수주
	{
		CreateNode(100, -1, 101, L"카밀라", L"빈민촌 분위기가 흉흉해.\n도와줄 용병이 필요하다더군.");
		CreateNode(101, 100, -1, L"은비", L"제가 빈민촌으로 가보겠습니다.");
	}

	{
		CreateNode(200, -1, -1, L"카밀라", L"한잔 드릴까요?");
	}

	// 빈민촌 진입, 퀘스트 수주
	{
		CreateNode(102, -1, 103, L"아이를 찾는 주민", L"제발 우리 아이 좀 찾아주세요!\n빈민촌 구석으로 갔는데 안 보여요.");
		CreateNode(103, 102, -1, L"은비", L"금방 찾아올 테니 너무 걱정 마십시오");

		CreateNode(104, -1, -1, L"아이", L"흑흑...\n괴물 소리가 나서 숨어 있었어요...");

		CreateNode(105, -1, 106, L"아이를 찾은 주민", L"정말 감사합니다! 약소하지만 보상입니다.\n참, 갱도 쪽에서 이상한 놈들이 수군거리는 걸 봤어요.");
		CreateNode(106, 105, -1, L"은비", L"(갱도라... 당장 확인해 봐야겠군)");
	}

	// 갱도 진입
	{
	}

	// 최종 보스
	{
	}
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
