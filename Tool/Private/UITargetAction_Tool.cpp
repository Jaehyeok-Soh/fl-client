#include "pch.h"
#include "UITargetAction_Tool.h"
#include "Tool_Defines.h"

#include "ToolCanvas.h"
#include "ToolLayer.h"
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

CToolLayer* CUITargetAction_Tool::Find_Layer(const _string& strLayerTag)
{
    auto* pLayer = CImGui_UIManager::GetInstance()->Find_Layer(strLayerTag);
    if (nullptr == pLayer)
        return nullptr;
    return pLayer;
}

void CUITargetAction_Tool::Trigger_All_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag)
{
    auto* pCanvas = Find_Canvas(strCanvasTag);
    if (nullptr == pCanvas)
        return;

    auto* pLayerVec = pCanvas->Safe_Access_LayerObject_Vector_Ptr();
    if (nullptr == pLayerVec)
        return;

    for (auto* pLayer : *pLayerVec)
    {     
        auto* pUIVec = pLayer->Safe_Access_UIObject_Vector_Ptr();
        if (nullptr == pUIVec)
            continue;

        for (auto* pUI : *pUIVec)
            Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), DTO::EUIEvent_Flag::INVOKED);
    }
}

void CUITargetAction_Tool::Trigger_All_Layer(uint32_t iLevelIndex, const _string& strLayerTag)
{
    auto* pLayer = Find_Layer(strLayerTag);
    if (nullptr == pLayer)
        return;
    auto* pUIVec = pLayer->Safe_Access_UIObject_Vector_Ptr();
    if (nullptr == pUIVec)
        return;
    for (auto* pUI : *pUIVec)
        Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), DTO::EUIEvent_Flag::INVOKED);
}

void CUITargetAction_Tool::Trigger_TargetUI(uint32_t iLevelIndex, const _string& strUITag)
{
    auto* pUI = CImGui_UIManager::GetInstance()->Find_UI(strUITag);
    if (nullptr == pUI)
        return;

    Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), DTO::EUIEvent_Flag::INVOKED);
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





