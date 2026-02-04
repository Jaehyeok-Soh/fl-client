#include "pch.h"
#include "ImGui_Inspector.h"
#include "ImGui_Layout.h"
#include "GameInstance.h"

CImGui_Inspector::CImGui_Inspector(std::string_view label, _uint iLayoutCount)
	: Super(label)
{
	m_vecLayouts.resize(iLayoutCount);
}

HRESULT CImGui_Inspector::Add_Layout(_uint iIndex, CImGui_Layout* pLayout)
{
	if (iIndex < 0 || iIndex >= m_vecLayouts.size())
		return E_FAIL;
	if (pLayout == nullptr)
		return E_FAIL;
	if (m_vecLayouts[iIndex] != nullptr)
		return E_FAIL;

	m_vecLayouts[iIndex] = pLayout;
	return S_OK;
}

void CImGui_Inspector::Render_Layout(_uint iIndex, CGameObject* pGo)
{
	if (m_vecLayouts[iIndex] == nullptr)
		return;
	m_vecLayouts[iIndex]->Render(pGo);
}

void CImGui_Inspector::Free()
{
	for (CImGui_Layout* pElement : m_vecLayouts)
		Safe_Release(pElement);
	m_vecLayouts.clear();
	Super::Free();
}

