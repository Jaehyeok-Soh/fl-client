#include "pch.h"
#include "UITargetAction_Tool.h"
#include "Tool_Defines.h"

#include "ToolUI.h"

CUITargetAction_Tool::CUITargetAction_Tool(CToolUI* pUI)
    :m_pOwner(pUI)
{
}


CGameObject* CUITargetAction_Tool::Find_GameObject(const _string& strTag)
{
    return nullptr;
}

CComponent* CUITargetAction_Tool::Find_Component(const _string& strTag)
{
    return nullptr;
}


CUITargetAction_Tool* CUITargetAction_Tool::Create(CToolUI* pUI)
{
    if (nullptr == pUI)
        return nullptr;

    return new CUITargetAction_Tool(pUI);
}

void CUITargetAction_Tool::Free()
{
    Super::Free();
}
