#include "pch.h"
#include "ImGui_UIManager.h"
#include "Tool_Defines.h"
#include "Engine_Utils.h"
#include "ToolCanvas.h"
#include "ToolLayer.h"
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
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumCanvasData = static_cast<int32_t>(m_vecCanvas.size());
	if (iNewCanvasIndex >= iNumCanvasData)
		return;

	m_iCurCanvasIndex = iNewCanvasIndex;

	auto* pLayerVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_LayerObject_Vector_Ptr();
	if (nullptr == pLayerVec)
	{
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumLayerData = static_cast<int32_t>(pLayerVec->size());

	if (m_iCurLayerIndex < 0)
		m_iCurLayerIndex = 0;
	if (m_iCurLayerIndex >= iNumLayerData)
		m_iCurLayerIndex = iNumLayerData - 1;

	auto* pUIvec = (*pLayerVec)[m_iCurLayerIndex]->Safe_Access_UIObject_Vector_Ptr();
	if (nullptr == pUIvec)
	{
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumUIData = static_cast<int32_t>(pUIvec->size());

	if (m_iCurUIIndex < 0)
		m_iCurUIIndex = 0;
	if (m_iCurUIIndex >= iNumUIData)
		m_iCurUIIndex = iNumUIData - 1;
}

void CImGui_UIManager::Safe_Change_Layer(int32_t iNewLayerIndex)
{
	if (-1 == m_iCurCanvasIndex)
		return;

	auto* pLayerVec = m_vecCanvas[m_iCurCanvasIndex]->Safe_Access_LayerObject_Vector_Ptr();
	if (nullptr == pLayerVec || iNewLayerIndex < 0)
	{
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumLayerData = static_cast<int32_t>(pLayerVec->size());
	if (iNewLayerIndex >= iNumLayerData)
		return;

	m_iCurLayerIndex = iNewLayerIndex;

	auto* pUIVec = (*pLayerVec)[m_iCurLayerIndex]->Safe_Access_UIObject_Vector_Ptr();
	if (nullptr == pUIVec)
	{
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumUIData = static_cast<int32_t>(pUIVec->size());
	
	if (m_iCurUIIndex >= iNumUIData)
		m_iCurUIIndex = iNumUIData - 1;
}

void CImGui_UIManager::Safe_Change_UI(int32_t iNewUIIndex)
{
	if (-1 == m_iCurCanvasIndex)
		return;

	if (-1 == m_iCurLayerIndex)
		return;

	auto* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
	{
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	auto* pUIVec = pLayer->Safe_Access_UIObject_Vector_Ptr();
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

int32_t CImGui_UIManager::Get_NumLayer()
{
	CToolCanvas* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return 0;

	if (nullptr == pCanvas->Safe_Access_LayerObject_Vector_Ptr())
		return 0;

	return static_cast<int32_t>(pCanvas->Safe_Access_LayerObject_Vector_Ptr()->size());
}

int32_t CImGui_UIManager::Get_NumUI()
{
	CToolLayer* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return 0;

	if (nullptr == pLayer->Safe_Access_UIObject_Vector_Ptr())
		return 0;

	return static_cast<int32_t>(pLayer->Safe_Access_UIObject_Vector_Ptr()->size());
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

vector<CToolLayer*>* CImGui_UIManager::Safe_Access_LayerVector()
{
	/* 현재 Canvas에 접근 */
	CToolCanvas* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return nullptr;

	/* 현재 Canvas가 가진 Layer Vector에 접근 */
	vector<CToolLayer*>* pLayerVec = pCanvas->Safe_Access_LayerObject_Vector_Ptr();
	if (nullptr == pLayerVec)
		return nullptr;

	return pLayerVec;
}

CToolLayer* CImGui_UIManager::Safe_Access_Layer(int32_t index)
{
	/* 현재 Canvas에 접근 */
	CToolCanvas* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return nullptr;

	/* index번째 Layer Vector에 접근, [Vector.empty / index >= Vector.size / index < 0] -> nullptr */
	CToolLayer* pLayer = pCanvas->Safe_Access_LayerObject_Ptr(index);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer;
}

vector<CToolUI*>* CImGui_UIManager::Safe_Access_UIVector()
{
	/* 현재 Layer에 접근 */
	CToolLayer* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return nullptr;

	/* 현재 Layer가 가진 UI Vector에 접근 */
	vector<CToolUI*>* pUIVector = pLayer->Safe_Access_UIObject_Vector_Ptr();
	if (nullptr == pUIVector)
		return nullptr;

	return pUIVector;
}

CToolUI* CImGui_UIManager::Safe_Access_UI(int32_t index)
{
	/* 현재 Layer에 접근 */
	CToolLayer* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return nullptr;

	/* index번째 UI Vector에 접근, [Vector.empty / index >= Vector.size / index < 0] -> nullptr */
	CToolUI* pUI = pLayer->Safe_Access_UIObject_Ptr(index);
	if (nullptr == pUI)
		return nullptr;

	return pUI;
}

void CImGui_UIManager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

