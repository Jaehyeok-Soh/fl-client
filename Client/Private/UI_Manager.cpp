#include "ImGui_UIManager.h"
#include "pch.h"
#include "UI_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "GameInstance.h"

NS_BEGIN(Client)

IMPLEMENT_SINGLETON(CUI_Manager)
CUI_Manager::CUI_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

CCanvas* CUI_Manager::Find_Canvas(uint32_t iLevelIndex, const _string& strCanvasTag)
{
	auto iter = m_mapCanvasCache[iLevelIndex].find(strCanvasTag);
	if (iter == m_mapCanvasCache[iLevelIndex].end())
		return nullptr;
	return iter->second;
}

CGenericUI* CUI_Manager::Find_GenericUI(uint32_t iLevelIndex, const _string& strUITag)
{
	auto iter = m_mapUICache[iLevelIndex].find(strUITag);
	if (iter == m_mapUICache[iLevelIndex].end())
		return nullptr;
	return iter->second;
}

vector<CGenericUI*>* CUI_Manager::Find_GenericUI_Vector(uint32_t iLevelIndex, const _string& strUILayerTag)
{
	auto* pCanvas = Find_Canvas(iLevelIndex, strUILayerTag);
	if (nullptr == pCanvas)
		return nullptr;
	return pCanvas->Get_UIVector();
}

vector<CCanvas*>* CUI_Manager::Get_Level_All_Canvas(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecCanvasCache[iLevelIndex];
}

vector<CGenericUI*>* CUI_Manager::Get_Level_All_GenericUI(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecGenericUICache[iLevelIndex];
}

void CUI_Manager::Add_RenderGroup(uint32_t iLevelIndex)
{
	if (m_isSort)
	{
		Sort_UI(m_vecGenericUICache[iLevelIndex]);
		m_isSort = FALSE;
	}

	for (auto* pUI : m_vecSortUI)
	{
		CGameObject* pObj = dynamic_cast<CGameObject*>(pUI);
		if (nullptr == pObj)
			continue;

		m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::UI, pObj);
	}
}


void CUI_Manager::Request_SortUI()
{
	m_isSort = TRUE;
}

void CUI_Manager::Clear_Cache(uint32_t iLevelIndex)
{

}

void CUI_Manager::Sort_UI(vector<CGenericUI*>& Target)
{
	const auto& Base = m_vecGenericUICache[ENUM_TO_UINT(ELevelType::STATIC)];

	m_vecSortUI.clear();
	m_vecSortUI.reserve(Base.size() + Target.size());

	m_vecSortUI.insert(m_vecSortUI.end(), Base.begin(), Base.end());
	m_vecSortUI.insert(m_vecSortUI.end(), Target.begin(), Target.end());

	/* Z가 큰 순서대로 렌더 그룹에 넣기 */
	std::sort(m_vecSortUI.begin(), m_vecSortUI.end(), [](const CGenericUI* pUI1, const CGenericUI* pUI2)
		{
			return pUI1->Get_PosZ() > pUI2->Get_PosZ();
		});
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END