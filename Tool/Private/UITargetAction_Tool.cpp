#include "pch.h"
#include "UITargetAction_Tool.h"
#include "Tool_Defines.h"

#include "ToolCanvas.h"
#include "ToolUI.h"
#include "GameInstance.h"
#include "ImGui_UIManager.h"

CUITargetAction_Tool::CUITargetAction_Tool(CToolUI* pUI)
    :m_pOwner(pUI),
    m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

CToolCanvas* CUITargetAction_Tool::Find_Canvas(const _string& Find_Canvas)
{
    auto* pCanvas = CImGui_UIManager::GetInstance()->Find_Canvas(Find_Canvas);
    if (nullptr == pCanvas)
        return nullptr;
    return pCanvas;
}

void CUITargetAction_Tool::Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag, DTO::EUIAction eAction, const json& jTargetActionParam)
{
    auto action = m_pGameInstance->Get_UIAction_Registry()->Build_Action(eAction, jTargetActionParam);
    if (!action)
        return;

    auto* pCanvas = Find_Canvas(strCanvasTag);
    if (nullptr == pCanvas)
        return;

    auto* pUIVec = pCanvas->Safe_Access_UI_Vector();
    if (nullptr == pUIVec)
        return;

    for (auto* pUI : *pUIVec)
    {
        /* 액션중이면 외부 입력 무시 */
        if (pUI->Get_isAction())
            continue;

        action(pUI->Get_ActionForMe(), this);
    }
}

void CUITargetAction_Tool::Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag, DTO::EUIAction eAction, const json& jTargetActionParam)
{
    auto action = m_pGameInstance->Get_UIAction_Registry()->Build_Action(eAction, jTargetActionParam);
    if (!action)
        return;

    auto* pUI = CImGui_UIManager::GetInstance()->Find_UI(strUITag);
    if (nullptr == pUI)
        return;

    /* 액션중이면 외부 입력 무시 */
    if (pUI->Get_isAction())
        return;

    action(pUI->Get_ActionForMe(), this);
}

CUITargetAction_Tool* CUITargetAction_Tool::Create(CToolUI* pUI)
{
    if (nullptr == pUI)
        return nullptr;

    return new CUITargetAction_Tool(pUI);
}

void CUITargetAction_Tool::Free()
{
    Safe_Release(m_pGameInstance);
    Super::Free();
}





