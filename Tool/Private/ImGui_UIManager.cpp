#include "pch.h"
#include "ImGui_UIManager.h"
#include "Tool_Defines.h"
#include "Engine_Utils.h"
#include "ToolUI.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_UIManager)

CImGui_UIManager::CImGui_UIManager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

void CImGui_UIManager::Safe_Add_CanvasData(const CANVAS_DATA& tData)
{
	m_vecCanvasData.push_back(tData);
	Safe_Change_Canvas(static_cast<uint32_t>(m_vecCanvasData.size() - 1));
}

void CImGui_UIManager::Safe_Add_LayerData(const LAYER_DATA& tData)
{
	CANVAS_DATA* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return;
	
	pCanvas->vecLayers.push_back(tData);
	Safe_Change_Layer(static_cast<uint32_t>(pCanvas->vecLayers.size() - 1));
}

void CImGui_UIManager::Safe_Add_UIData(const GENERIC_UI_DATA& tData)
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return;

	if (FAILED(Safe_Add_UI(pLayer, tData)))
		return;

	pLayer->vecUIData.push_back(tData);
	Safe_Change_UI(static_cast<uint32_t>(pLayer->vecUIData.size() - 1));
}

HRESULT CImGui_UIManager::Safe_Add_UI(LAYER_DATA* pLayer, const GENERIC_UI_DATA& tData)
{
	if ("" == pLayer->strTag)
		return E_FAIL;

	uint32_t iLevelIndex = static_cast<uint32_t>(ELevelType::UI);
	CToolUI::TOOLUI_DESC Desc = {};
	Desc.isAlpha	= TRUE;
	Desc.fWidth		= 64.f;
	//Desc.fWidth		= tData.fWidth;
	Desc.fHeight	= 64.f;
	//Desc.fHeight	= tData.fHeight;
	Desc.fX			= tData.fPosX;
	Desc.fY			= tData.fPosY;
	Desc.fZ			= tData.fPosZ;
	Desc.wstrTextureTag = L"Prototype_Component_Button_Test_Texture";

	CGameObject* pGO = CGameInstance::GetInstance()->Add_GameObject(iLevelIndex, L"Prototype_UI_Test_Button",iLevelIndex,Engine_Utils::ToWString(pLayer->strTag), &Desc);
	if (nullptr == pGO)
		return E_FAIL;

	pLayer->vecUIObjects.push_back(reinterpret_cast<CToolUI*>(pGO));
	return S_OK;
}

void CImGui_UIManager::Safe_Change_Canvas(int32_t iNewCanvasIndex)
{
	if (m_vecCanvasData.empty() || iNewCanvasIndex < 0)
	{
		m_iCurCanvasIndex = -1;
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	/* ÁÖÀÇ */
	int32_t iNumCanvasData = static_cast<int32_t>(m_vecCanvasData.size());
	if (iNewCanvasIndex >= iNumCanvasData)
		return;

	m_iCurCanvasIndex = iNewCanvasIndex;
	if (m_vecCanvasData[m_iCurCanvasIndex].vecLayers.empty())
	{
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumLayerData = static_cast<int32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers.size());
	if (m_iCurLayerIndex >= iNumLayerData)
		m_iCurLayerIndex = iNumLayerData - 1;

	if (m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.empty())
	{
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumUIData = static_cast<int32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.size());
	if (m_iCurUIIndex >= iNumUIData)
		m_iCurUIIndex = iNumUIData - 1;
}

void CImGui_UIManager::Safe_Change_Layer(int32_t iNewLayerIndex)
{
	if (-1 == m_iCurCanvasIndex)
		return;

	if (m_vecCanvasData[m_iCurCanvasIndex].vecLayers.empty() || iNewLayerIndex < 0)
	{
		m_iCurLayerIndex = -1;
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumLayerData = static_cast<int32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers.size());
	if (iNewLayerIndex >= iNumLayerData)
		return;

	m_iCurLayerIndex = iNewLayerIndex;

	if (m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.empty())
	{
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumUIData = static_cast<int32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.size());
	
	if (m_iCurUIIndex >= iNumUIData)
		m_iCurUIIndex = iNumUIData - 1;
}

void CImGui_UIManager::Safe_Change_UI(int32_t iNewUIIndex)
{
	if (-1 == m_iCurCanvasIndex)
		return;

	if (-1 == m_iCurLayerIndex)
		return;

	if (m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.empty() || iNewUIIndex < 0)
	{
		m_iCurUIIndex = -1;
		return;
	}

	int32_t iNumUIData = static_cast<int32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.size());
	if (iNewUIIndex >= iNumUIData)
		return;

	m_iCurUIIndex = iNewUIIndex;
}

int32_t CImGui_UIManager::Get_NumCanvas()
{
	if (m_vecCanvasData.empty())
		return 0;

	return static_cast<int32_t>(m_vecCanvasData.size());
}

int32_t CImGui_UIManager::Get_NumLayer()
{
	CANVAS_DATA* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return 0;

	if (pCanvas->vecLayers.empty())
		return 0;

	return static_cast<int32_t>(pCanvas->vecLayers.size());
}

int32_t CImGui_UIManager::Get_NumUI()
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return 0;

	if (pLayer->vecUIData.empty())
		return 0;

	return static_cast<int32_t>(pLayer->vecUIData.size());
}

void CImGui_UIManager::Safe_Remove_CanvasData()
{
	CANVAS_DATA* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return;

	vector<_wstring> vecLayerTags;
	vecLayerTags.reserve(pCanvas->vecLayers.size());

	for (const LAYER_DATA& Layer : pCanvas->vecLayers)
	{
		if (!Layer.strTag.empty())
			vecLayerTags.push_back(Engine_Utils::ToWString(Layer.strTag));
	}

	for (const _wstring& wstrTag : vecLayerTags)
	{
		m_pGameInstance->Clear_Layer(static_cast<uint32_t>(ELevelType::UI), wstrTag);
	}

	pCanvas->Clear_Data();
	m_vecCanvasData.erase(m_vecCanvasData.begin() + m_iCurCanvasIndex);

	if (Get_NumCanvas() <= 0)
		Safe_Change_Canvas(-1);
	else
		Safe_Change_Canvas(Get_NumCanvas() - 1);
}

void CImGui_UIManager::Safe_Remove_LayerData()
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return;
	auto p = &m_vecCanvasData[m_iCurCanvasIndex].vecLayers;

	_wstring wstrLayerTag = Engine_Utils::ToWString(pLayer->strTag);
	pLayer->Clear_Data();
	m_pGameInstance->Clear_Layer(static_cast<uint32_t>(ELevelType::UI), wstrLayerTag);
	
	p->erase(p->begin() +  m_iCurLayerIndex);
	if(Get_NumLayer() <= 0)
		Safe_Change_Layer(-1);
	else
		Safe_Change_Layer(Get_NumLayer() - 1);
}


HRESULT CImGui_UIManager::Remake_UIObjects()
{
	CANVAS_DATA* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return S_OK;

	if (pCanvas->vecLayers.empty())
		return S_OK;

	uint32_t iLevelIndex = static_cast<uint32_t>(ELevelType::UI);

	for (LAYER_DATA& Layer : pCanvas->vecLayers)
	{
		if (Layer.strTag.empty())
			continue;

		Layer.vecUIObjects.clear();

		for (const GENERIC_UI_DATA& UIData : Layer.vecUIData)
		{
			CToolUI::TOOLUI_DESC Desc = {};
			Desc.fX = UIData.fPosX;
			Desc.fY = UIData.fPosY;
			Desc.fZ = UIData.fPosZ;
			Desc.fHeight = UIData.fHeight;
			Desc.fWidth = UIData.fWidth;
			Desc.isAlpha = TRUE;
			Desc.wstrTextureTag = L"Prototype_Component_Button_Test_Texture";

			CGameObject* pResult = m_pGameInstance->Add_GameObject(
				iLevelIndex, L"Prototype_UI_Test_Button",
				iLevelIndex, Engine_Utils::ToWString(Layer.strTag), &Desc);

			if (nullptr == pResult)
				return E_FAIL;

			Layer.vecUIObjects.push_back(reinterpret_cast<CToolUI*>(pResult));
		}
	}
	return S_OK;
}

HRESULT CImGui_UIManager::Clear_UIObjects()
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return S_OK;

	if ("" == pLayer->strTag)
		return S_OK;

	uint32_t iLevelIndex = static_cast<uint32_t>(ELevelType::UI);

	m_pGameInstance->Clear_Layer(iLevelIndex, Engine_Utils::ToWString(pLayer->strTag));
	pLayer->vecUIObjects.clear();
	return S_OK;
}


vector<CANVAS_DATA>* CImGui_UIManager::Safe_Access_CanvasVector()
{
	if (m_vecCanvasData.empty())
		return nullptr;
	return &m_vecCanvasData;
}

CANVAS_DATA* CImGui_UIManager::Safe_Access_Canvas(int32_t index)
{
	if (m_vecCanvasData.empty())
		return nullptr;
	
	if (index < 0)
		return nullptr;

	int32_t NumCanvas = static_cast<int32_t>(m_vecCanvasData.size());

	if (index >= NumCanvas)
		return nullptr;

	return &m_vecCanvasData[index];
}

vector<LAYER_DATA>* CImGui_UIManager::Safe_Access_LayerVector()
{
	CANVAS_DATA* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return nullptr;

	if (pCanvas->vecLayers.empty())
		return nullptr;

	return &pCanvas->vecLayers;
}

LAYER_DATA* CImGui_UIManager::Safe_Access_Layer(int32_t index)
{
	CANVAS_DATA* pCanvas = Safe_Access_Canvas(m_iCurCanvasIndex);
	if (nullptr == pCanvas)
		return nullptr;

	if ((*pCanvas).vecLayers.empty())
		return nullptr;

	if (index < 0)
		return nullptr;

	int32_t iNumLayer = static_cast<int32_t>((*pCanvas).vecLayers.size());
	if (index >= iNumLayer)
		return nullptr;

	return &(pCanvas->vecLayers[index]);
}

vector<GENERIC_UI_DATA>* CImGui_UIManager::Safe_Access_UIVector()
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return nullptr;

	if (pLayer->vecUIData.empty())
		return nullptr;

	return &pLayer->vecUIData;
}

GENERIC_UI_DATA* CImGui_UIManager::Safe_Access_UIData(int32_t index)
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);
	if (nullptr == pLayer)
		return nullptr;

	if ((*pLayer).vecUIData.empty())
		return nullptr;

	if (index < 0)
		return nullptr;

	int32_t NumUI = static_cast<int32_t>((*pLayer).vecUIData.size());
	if (index >= NumUI)
		return nullptr;

	return &(pLayer->vecUIData[index]);
}

CToolUI* CImGui_UIManager::Safe_Access_UIObject()
{
	LAYER_DATA* pLayer = Safe_Access_Layer(m_iCurLayerIndex);

	if (nullptr == pLayer)
		return nullptr;

	if (pLayer->vecUIObjects.empty())
		return nullptr;

	int32_t NumUIObject = static_cast<int32_t>(pLayer->vecUIObjects.size());
	if (m_iCurUIIndex >= NumUIObject || m_iCurUIIndex < 0)
		return nullptr;

	return pLayer->vecUIObjects[m_iCurUIIndex];
}

void CImGui_UIManager::Free()
{
	Safe_Release(m_pGameInstance);
	Super::Free();
}

