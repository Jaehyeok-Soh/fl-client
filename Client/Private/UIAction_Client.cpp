#include "pch.h"
#include "UIAction_Client.h"
#include "Client_Defines.h"

#include "GenericUI.h"

CUIAction_Client::CUIAction_Client(CGenericUI* pUI)
    :m_pOwner(pUI)
{
}

void CUIAction_Client::Set_Visible(bool isVisible)
{
    if (nullptr == m_pOwner)
        return;

    if (isVisible)
        m_pOwner->Set_Visible();
    else
        m_pOwner->Set_Invisible();
}

void CUIAction_Client::Set_TextureIndex(_uint index)
{
    if (nullptr == m_pOwner)
        return;

    m_pOwner->Set_TextureIndex(index);
}

const _string& CUIAction_Client::Get_Tag() const
{
    static const _string sEmpty = "";
    return (m_pOwner != nullptr) ? m_pOwner->Get_Tag() : sEmpty;
}

CUIAction_Client* CUIAction_Client::Create(CGenericUI* pUI)
{
    if (nullptr == pUI)
        return nullptr;

    return new CUIAction_Client(pUI);
}

void CUIAction_Client::Free()
{
    Super::Free();
}


