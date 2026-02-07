#include "pch.h"
#include "UITargetAction_Client.h"
#include "Client_Defines.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUITargetAction_Client::CUITargetAction_Client(CGenericUI* pUI)
	:m_pOwner(pUI),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUITargetAction_Client::Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag, DTO::EUIAction eAction, const json& jTargetActionParam)
{
	auto* AllUIVec = CUI_Manager::GetInstance()->Get_Level_All_GenericUI(iLevelIndex);
	if (nullptr == AllUIVec)
		return;
	
	for (auto* pUI : *AllUIVec)          
		m_pGameInstance->Get_UIAction_Registry()->Build_Action(eAction, jTargetActionParam)(pUI->Get_ActionForMe(), this);
}

void CUITargetAction_Client::Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag, DTO::EUIAction eAction, const json& jTargetActionParam)
{
	auto* pUI = CUI_Manager::GetInstance()->Find_GenericUI(iLevelIndex, strUITag);
	if (nullptr == pUI)
		return;

	m_pGameInstance->Get_UIAction_Registry()->Build_Action(eAction, jTargetActionParam)(pUI->Get_ActionForMe(), this);
}

CUITargetAction_Client* CUITargetAction_Client::Create(CGenericUI* pUI)
{
	return new CUITargetAction_Client(pUI);
}

void CUITargetAction_Client::Free()
{
	Safe_Release(m_pGameInstance);
}
