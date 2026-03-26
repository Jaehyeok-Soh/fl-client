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
		CreateNode(100, -1, 101, L"카밀라", L"월석 사냥꾼, 연옥도가 기습 당했다는건 들었어.\n여기까지 오느라 고생이 많았네.");
		CreateNode(101, 100, 102, L"카밀라", L"당분간은 이곳에서 활동하면서 월석에 대한 정보를 얻어가는게 어때?");
		CreateNode(102, 101, 103, L"카밀라", L"좋아요, 당장 할 만한 일이 있나요?");
		CreateNode(103, 102, 104, L"카밀라", L"마침 당신 같은 실력자가 알아봐 줬으면 하는 일이 하나 있어.");
		CreateNode(104, 103, 105, L"카밀라", L"빈민촌 분위기가 흉흉해.\n도와줄 용병이 필요하다더군.");
		CreateNode(105, 104, 106, L"카밀라", L"근처의 갱도에서 원인 불명의 일들이 벌어지고 있다는 정보야.");
		CreateNode(106, 105, -1, L"은비", L"제가 빈민촌으로 가보겠습니다.");
	}

	// 빈민촌 진입, 퀘스트 수주
	{
		CreateNode(107, -1, 108, L"아이를 찾는 주민", L"제발 우리 아이 좀 찾아주세요!\n빈민촌 구석으로 갔는데 안 보여요.");
		CreateNode(108, 107, -1, L"은비", L"금방 찾아올 테니 너무 걱정 마십시오");

		CreateNode(109, -1, 110, L"아이", L"흑흑...\n괴물 소리가 나서 숨어 있었어요...");
		CreateNode(110, 109, 111, L"아이", L"친구들과 갱도로 모험을 떠났다가 괴물들에 쫒겼어요.");
		CreateNode(111, 110, -1, L"아이", L"다른 친구들은 아직 갱도에 있어요, 제 친구들을 구해주세요!");

		CreateNode(112, -1, 113, L"아이를 찾은 주민", L"정말 감사합니다!\n참, 갱도에서 원인 불명의 열기가 발생한다고 하네요.");
		CreateNode(113, 112, -1, L"은비", L"(갱도라... 당장 확인해 봐야겠군)");
	}

	// 갱도 진입
	{
		CreateNode(114, -1, 115, L"두려움에 떨고있는 아이", L"흑흑... 살려주세요...");
		CreateNode(115, 114, -1, L"은비", L"일단 여기 숨어있어!");
		CreateNode(116, -1, 117, L"은비", L"이제 안전해, 어서 집으로 돌아가.");
		CreateNode(117, 116, -1, L"두려움에 떨고있는 아이", L"감사합니다!");
	}

	// 최종 보스
	{
	}

	{
		CreateNode(200, -1, -1, L"카밀라", L"한잔 드릴까요?");
		
		CreateNode(210, -1, -1, L"아이를 찾는 주민", L"우리 아이를 찾아주세요...");
		CreateNode(211, -1, -1, L"아이를 찾은 주민", L"월석 사냥꾼, 정말 고마워요.");

		CreateNode(220, -1, -1, L"아이", L"흑흑.....");

		CreateNode(225, -1, -1, L"두려움에 떨고있는 아이", L"흑흑..., 도와주세요!");

		CreateNode(230, -1, 231, L"베테랑 월석 사냥꾼", L"자네가 연옥도에서 온 신입 월석 사냥꾼인가?");
		CreateNode(231, 230, 232, L"베테랑 월석 사냥꾼", L"실력이 좀 있다던데... 왕년의 나를 보는것 같아.");
		CreateNode(232, 231, 233, L"추억에 젖은 베테랑 월석 사냥꾼", L"나도 자네 같은 신입 시절에 선배에게 받은 소중한 것이 있네.");
		CreateNode(233, 232, 234, L"추억에 젖은 베테랑 월석 사냥꾼", L"이젠 자네에게 넘겨주도록 하지.");
		CreateNode(234, 233, -1, L"선배 월석 사냥꾼", L"좋은 곳에 쓰도록 하게.");

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
