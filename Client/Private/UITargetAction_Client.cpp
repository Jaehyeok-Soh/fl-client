#include "pch.h"
#include "UITargetAction_Client.h"
#include "Client_Defines.h"
#include "Canvas.h"
#include "UILayer.h"
#include "GenericUI.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUITargetAction_Client::CUITargetAction_Client(CGenericUI* pUI)
	:m_pOwner(pUI),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CUITargetAction_Client::Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag)
{
	auto* AllUIVec = CUI_Manager::GetInstance()->Get_Level_All_GenericUI(iLevelIndex);
	if (nullptr == AllUIVec)
		return;
	
	for (auto* pUI : *AllUIVec)
		Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), DTO::EUIEvent_Flag::INVOKED);
}

void CUITargetAction_Client::Trigger_All_Layer(uint32_t iLevelIndex, const _string& strLayerTag)
{
	auto* LayerUIVec = CUI_Manager::GetInstance()->Find_GenericUI_Vector(iLevelIndex, strLayerTag);
	if (nullptr == LayerUIVec)
		return;

	for (auto* pUI : *LayerUIVec)
		Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), DTO::EUIEvent_Flag::INVOKED);
}

void CUITargetAction_Client::Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag)
{
	auto* pUI = CUI_Manager::GetInstance()->Find_GenericUI(iLevelIndex, strUITag);
	if (nullptr == pUI)
		return;
	
	Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), DTO::EUIEvent_Flag::INVOKED);
}

CUITargetAction_Client* CUITargetAction_Client::Create(CGenericUI* pUI)
{
	return new CUITargetAction_Client(pUI);
}

void CUITargetAction_Client::Free()
{
	Safe_Release(m_pGameInstance);
}
