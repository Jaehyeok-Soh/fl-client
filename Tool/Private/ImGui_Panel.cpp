#include "Tool_Defines.h"
#include "ImGui_Layout.h"
#include "ImGui_Panel.h"

CImGui_Panel::CImGui_Panel(const _char* pLabel, CLevel* pOwner, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : Super(pLabel, pDevice, pDeviceContext)
    , m_pOwnerLevel(pOwner)
{
}

HRESULT CImGui_Panel::Add_Element(_uint _iType, CImGui_Layout* pElement)
{
    if (_iType < 0 || !pElement)
        return E_FAIL;

    if (m_vecLayouts[_iType] != nullptr)
        return E_FAIL;

    m_vecLayouts[_iType] = pElement;
    return S_OK;
}

CImGui_Layout* CImGui_Panel::Get_Layout(_uint _iType)
{
    if (_iType < 0)
        return nullptr;

    return m_vecLayouts[_iType];
}

void CImGui_Panel::Free()
{
    for (size_t i = 0; i < m_vecLayouts.size(); ++i)
    {
        Safe_Release(m_vecLayouts[i]);
    }
    m_vecLayouts.clear();
    Super::Free();
}
