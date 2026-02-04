#include "pch.h"
#include "UIAction_Tool.h"
#include "Tool_Defines.h"

#include "ToolUI.h"

CUIAction_Tool::CUIAction_Tool(CToolUI* pUI)
    :m_pOwner(pUI)
{
}

void CUIAction_Tool::Set_Visible(bool isVisible)
{
    if (nullptr == m_pOwner)
        return;

    if (isVisible)
        m_pOwner->Set_Visible();
    else
        m_pOwner->Set_Invisible();
}

void CUIAction_Tool::Set_TextureIndex(uint32_t index)
{
    if (nullptr == m_pOwner)
        return;

    m_pOwner->Set_TextureIndex(index);
}

const _string& CUIAction_Tool::Get_Tag() const
{
    static const _string sEmpty = "";
    return (m_pOwner != nullptr) ? m_pOwner->Get_Name() : sEmpty;
}

CUIAction_Tool* CUIAction_Tool::Create(CToolUI* pUI)
{
    if (nullptr == pUI)
        return nullptr;

    return new CUIAction_Tool(pUI);
}

void CUIAction_Tool::Free()
{
    Super::Free();
}
