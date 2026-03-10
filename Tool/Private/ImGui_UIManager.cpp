#include "pch.h"
#include "ImGui_UIManager.h"
#include "Tool_Defines.h"
#include "ToolCanvas.h"
#include "ToolUI.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_UIManager)

CImGui_UIManager::CImGui_UIManager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_UIManager::Safe_Add_Canvas(CToolCanvas* pCanvas)
{
	if (nullptr == pCanvas)
		return E_FAIL;

	m_vecCanvas.push_back(pCanvas);
	return S_OK;
}

void CImGui_UIManager::Safe_Change_Canvas(int32_t iNewCanvasIndex)
{
	if (m_vecCanvas.empty() || iNewCanvasIndex < 0)
	{
		m_iCurCanvasIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumCanvasData = static_cast<int32_t>(m_vecCanvas.size());
	if (iNewCanvasIndex >= iNumCanvasData)
		return;

	m_iCurCanvasIndex = iNewCanvasIndex;

	auto* pUIVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_UI_Vector();
	if (nullptr == pUIVec)
		return;

	int32_t iNumUIData = static_cast<int32_t>(pUIVec->size());

	if (m_iCurUIIndex < 0)
		m_iCurUIIndex = 0;
	if (m_iCurUIIndex >= iNumUIData)
		m_iCurUIIndex = iNumUIData - 1;
}

void CImGui_UIManager::Safe_Change_UI(int32_t iNewUIIndex)
{
	if (-1 == m_iCurCanvasIndex)
		return;

	auto* pUIVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_UI_Vector();
	if (nullptr == pUIVec || iNewUIIndex < 0)
	{
		m_iCurUIIndex = -1;
		return;
	}
	int32_t iNumUIData = static_cast<int32_t>(pUIVec->size());
	if (iNewUIIndex >= iNumUIData)
		return;

	m_iCurUIIndex = iNewUIIndex;
}

int32_t CImGui_UIManager::Get_NumCanvas()
{
	if (m_vecCanvas.empty())
		return 0;

	return static_cast<int32_t>(m_vecCanvas.size());
}

int32_t CImGui_UIManager::Get_NumUI()
{
	if (m_vecCanvas.empty())
		return 0;

	auto* pUIVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_UI_Vector();
	if (nullptr == pUIVec)
		return 0;

	return static_cast<int32_t>(pUIVec->size());
}

CToolCanvas* CImGui_UIManager::Safe_Access_Canvas(int32_t index)
{
	if (m_vecCanvas.empty())
		return nullptr;

	int32_t NumCanvas = static_cast<int32_t>(m_vecCanvas.size());
	if (index >= NumCanvas || index < 0)
		return nullptr;

	return m_vecCanvas[index];
}

vector<CToolUI*>* CImGui_UIManager::Safe_Access_UIVector()
{
	if (m_iCurCanvasIndex >= m_vecCanvas.size())
		return nullptr;

	auto* pUIVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_UI_Vector();
	return pUIVec;
}

CToolUI* CImGui_UIManager::Safe_Access_UI(int32_t index)
{
	if (m_iCurCanvasIndex >= m_vecCanvas.size())
		return nullptr;

	auto* pUIVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_UI_Vector();
	if (nullptr == pUIVec)
		return nullptr;
	
	int32_t iNumUI = static_cast<int32_t> (pUIVec->size());
	if ( index >= iNumUI)
		return nullptr;

	return (*pUIVec)[index];
}

HRESULT CImGui_UIManager::Safe_Add_CanvasCache(const _string& strTag, CToolCanvas* pCache)
{
	auto iter = m_MapCanvasCache.find(strTag);
	if (iter != m_MapCanvasCache.end())
		return E_FAIL;
	m_MapCanvasCache.emplace(strTag, pCache);
	return S_OK;
}

HRESULT CImGui_UIManager::Safe_Add_UICache(const _string& strTag, CToolUI* pCache)
{
	auto iter = m_MapUICache.find(strTag);
	if (iter != m_MapUICache.end())
		return E_FAIL;
	m_MapUICache.emplace(strTag, pCache);
	return S_OK;
}
CToolCanvas* CImGui_UIManager::Find_Canvas(const _string& strTag)
{
	auto iter = m_MapCanvasCache.find(strTag);
	if (iter == m_MapCanvasCache.end())
		return nullptr;
	return iter->second;
}

CToolUI* CImGui_UIManager::Find_UI(const _string& strTag)
{
	auto iter = m_MapUICache.find(strTag);
	if (iter == m_MapUICache.end())
		return nullptr;
	return iter->second;
}

void CImGui_UIManager::Clear()
{
	m_iCurCanvasIndex = {};
	m_iCurUIIndex = {};
	vector<_string> VecLayerTag;
	for (const auto* pCanvas : m_vecCanvas)
	{
		VecLayerTag.push_back(pCanvas->Get_Tag() + "_Layer");
	}

	m_MapCanvasCache.clear();
	m_MapUICache.clear();
	m_vecCanvas.clear();
	m_vecSortUI.clear();
	
	for (_string strLayerTag : VecLayerTag)
	{
		m_pGameInstance->Clear_Layer(ENUM_TO_UINT(ELevelType::UI), Engine_Utils::ToWString(strLayerTag));
	}
}

void CImGui_UIManager::Add_RenderGroup()
{
	if (m_isSort)
	{
		Sort_UI();
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

void CImGui_UIManager::Request_SortUI()
{
	m_isSort = TRUE;
}

void CImGui_UIManager::Sort_UI()
{
	m_vecSortUI.clear();
	for (auto* pCanvas : m_vecCanvas)
	{
		auto* pUIVec = pCanvas->Safe_Access_UI_Vector();
		if (nullptr == pUIVec)
			continue;
		for (auto* pUI : *pUIVec)
		{
			m_vecSortUI.push_back(pUI);
		}
	}

	/* Z가 큰 순서대로 렌더 그룹에 넣기 */
	std::sort(m_vecSortUI.begin(), m_vecSortUI.end(), [](const CToolUI* pUI1, const CToolUI* pUI2)
		{
			return pUI1->Get_PosZ() > pUI2->Get_PosZ();
		});
}

void CImGui_UIManager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

