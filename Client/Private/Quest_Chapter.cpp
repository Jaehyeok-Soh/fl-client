#include "pch.h"
#include "Quest_Chapter.h"
#include "IQuest.h"
#include "GameObject.h"
#include "GameInstance.h"

CQuest_Chapter::CQuest_Chapter()
	: Super(),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CQuest_Chapter::Initialize(QUEST_CHAPTERDESC desc, class CGameObject* pObj)
{
	m_tDesc = desc;
	m_tDesc.tQuestDesc.eType = QUESTLAYER::CHAPTER;
	m_tDesc.tQuestDesc.eState = QUESTSTATE::LOCKED;

	m_tDesc.iObjectId = pObj->Get_ID();
	m_tDesc.pObject = pObj;
	m_tDesc.wstrObjectLayer = pObj->Get_Layer();
	m_tDesc.vObjectPosition = pObj->Get_Component<CTransform>()->Get_Info(TRANSFORM_INFO_STATE::POS);

	return S_OK;
}

void CQuest_Chapter::Enter()
{
	if (m_tDesc.pObject == nullptr || !m_tDesc.pObject->IsAlive())
	{
		
		m_tDesc.pObject = m_pGameInstance->Get_GameObject_By_ID(m_pGameInstance->Get_CurrentLevelIndex(), m_tDesc.wstrObjectLayer, m_tDesc.iObjectId);
		if (m_tDesc.pObject == nullptr)
			return;
	}

	m_tDesc.tQuestDesc.eState = QUESTSTATE::IN_PROGRESS;

	IQuest* pQuestObject = dynamic_cast<IQuest*>(m_tDesc.pObject);

	if (pQuestObject != nullptr)
		pQuestObject->OnQuestEnter(m_tDesc);
}

void CQuest_Chapter::Exit()
{
	if (m_tDesc.pObject == nullptr || !m_tDesc.pObject->IsAlive())
		return;

	m_tDesc.tQuestDesc.eState = QUESTSTATE::COMPLETE;

	IQuest* pQuestObject = dynamic_cast<IQuest*>(m_tDesc.pObject);

	if (pQuestObject != nullptr)
		pQuestObject->OnQuestExit();
}

void CQuest_Chapter::UpdateProgress(QUEST_EVENT_SIGNATURE ID)
{
	if (m_tDesc.eEvent == ID.eEvent && m_tDesc.eTargetType == ID.eTargetType)
		m_tDesc.iCurrentCount += ID.iCount;
}

_bool CQuest_Chapter::IsComplete()
{
	return m_tDesc.iCurrentCount >= m_tDesc.iCount;
}

CQuest_Chapter* CQuest_Chapter::Create(QUEST_CHAPTERDESC desc, class CGameObject* pObj)
{
	CQuest_Chapter* pInstance = new CQuest_Chapter();
	if (FAILED(pInstance->Initialize(desc, pObj)))
	{
		MSG_BOX("CQuest_Chapter::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuest_Chapter::Free()
{
	Safe_Release(m_pGameInstance);

	Super::Free();
}
