#include "pch.h"
#include "UI_Manager.h"
#include "FileUtils.h"
#include "Engine_Utils.h"
#include "Canvas.h"
#include "UILayer.h"
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

CUILayer* CUI_Manager::Find_UILayer(uint32_t iLevelIndex, const _string& strUILayerTag)
{
	auto iter = m_mapUILayerCache[iLevelIndex].find(strUILayerTag);
	if (iter == m_mapUILayerCache[iLevelIndex].end())
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

vector<CUILayer*>* CUI_Manager::Find_UILayer_Vector(uint32_t iLevelIndex, const _string& strCanvasTag)
{
	CCanvas* pCanvas = Find_Canvas(iLevelIndex, strCanvasTag);
	if (nullptr == pCanvas)
		return nullptr;
	return pCanvas->Get_UILayerVector();
}

vector<CGenericUI*>* CUI_Manager::Find_GenericUI_Vector(uint32_t iLevelIndex, const _string& strUILayerTag)
{
	CUILayer* pUILayer = Find_UILayer(iLevelIndex, strUILayerTag);
	if (nullptr == pUILayer)
		return nullptr;
	return pUILayer->Get_UIVector();
}

vector<CCanvas*>* CUI_Manager::Get_Level_All_Canvas(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecCanvasCache[iLevelIndex];
}

vector<CUILayer*>* CUI_Manager::Get_Level_All_UILayer(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecUILayerCache[iLevelIndex];
}

vector<CGenericUI*>* CUI_Manager::Get_Level_All_GenericUI(uint32_t iLevelIndex)
{
	if (iLevelIndex >= g_iLevelType_Count)
		return nullptr;
	return &m_vecGenericUICache[iLevelIndex];
}

void CUI_Manager::Free()
{
	Safe_Release(m_pGameInstance);
}

NS_END