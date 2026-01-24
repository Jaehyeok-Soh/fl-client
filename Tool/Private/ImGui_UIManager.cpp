#include "pch.h"
#include "ImGui_UIManager.h"
#include "Tool_Defines.h"
#include "GameInstance.h"
#include "Engine_Utils.h"
#include "ToolUI.h"

IMPLEMENT_SINGLETON(CImGui_UIManager)

CImGui_UIManager::CImGui_UIManager()
{
}

void CImGui_UIManager::Add_CanvasData(const CANVAS_DATA& tData)
{
	m_vecCanvasData.push_back(tData);
	m_iCurCanvasIndex = static_cast<uint32_t>(m_vecCanvasData.size() - 1);
}

void CImGui_UIManager::Add_LayerData(const LAYER_DATA& tData)
{
	if (m_iCurCanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Add_LayerData, No Canvas");
		return;
	}
	m_vecCanvasData[m_iCurCanvasIndex].vecLayers.push_back(tData);
	m_iCurLayerIndex = static_cast<uint32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers.size() - 1);
}

void CImGui_UIManager::Add_UIData(const GENERIC_UI_DATA& tData)
{
	if (m_iCurCanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Add_UIData, No Canvas");
		return;
	}

	if (m_iCurLayerIndex >= Get_NumLayer(m_iCurCanvasIndex))
	{
		MSG_BOX("CImGui_UIManager::Add_UIData, No Layer");
		return;
	}
	m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.push_back(tData);
	Add_UI(tData);
	m_iCurUIIndex = static_cast<uint32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.size() - 1);
}

void CImGui_UIManager::Add_UI(const GENERIC_UI_DATA& tData)
{
	CToolUI::TOOLUI_DESC Desc = {};
	Desc.bAlpha = TRUE;
	Desc.fSizeX = 1.f;
	Desc.fSizeY = 1.f;
	Desc.fX = 100.f;
	Desc.fY = 100.f;
	Desc.wstrTextureTag = L"Prototype_Component_Button_Test_Texture";

	CGameObject* pGO = CGameInstance::GetInstance()->Add_GameObject(
		static_cast<uint32_t>(ELevelType::UI), L"Prototype_UI_Test_Button",
		static_cast<uint32_t>(ELevelType::UI), Engine_Utils::ToWString(m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].strTag), &Desc);

	pGO->Awake(static_cast<uint32_t>(ELevelType::UI));
	m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIObjects.push_back(reinterpret_cast<CToolUI*>(pGO));
}

void CImGui_UIManager::Change_Canvas(uint32_t iNewCanvasIndex)
{
	uint32_t iNumCanvas = Get_NumCanvas();

	if (iNumCanvas == 0)
	{
		m_iCurCanvasIndex = 0;
		m_iCurLayerIndex = 0;
		m_iCurUIIndex = 0;
		return;
	}

	if (iNewCanvasIndex > iNumCanvas)
		return;
	
	m_iCurLayerIndex = 0;
	m_iCurUIIndex = 0;
	m_iCurCanvasIndex = iNewCanvasIndex;
}

void CImGui_UIManager::Change_Layers(uint32_t iNewLayerIndex)
{
	uint32_t iNumCanvas = Get_NumCanvas();

	if (iNumCanvas == 0)
	{
		m_iCurCanvasIndex = 0;
		m_iCurLayerIndex = 0;
		m_iCurUIIndex = 0;
		return;
	}

	uint32_t iNumLayer = Get_NumLayer(m_iCurCanvasIndex);

	if (iNumLayer == 0)
	{
		m_iCurLayerIndex = 0;
		m_iCurUIIndex = 0;
		return;
	}

	if (iNewLayerIndex > iNumLayer)
		return;

	m_iCurUIIndex = 0;
	m_iCurLayerIndex = iNewLayerIndex;
}

void CImGui_UIManager::Change_UIData(uint32_t iNewUIIndex)
{
	uint32_t iNumCanvas = Get_NumCanvas();
	if (iNumCanvas == 0)
	{
		m_iCurCanvasIndex = 0;
		m_iCurLayerIndex = 0;
		m_iCurUIIndex = 0;
		return;
	}

	uint32_t iNumLayer = Get_NumLayer(m_iCurCanvasIndex);
	if (iNumLayer == 0)
	{
		m_iCurLayerIndex = 0;
		m_iCurUIIndex = 0;
		return;
	}

	uint32_t iNumUI = Get_NumUI(m_iCurCanvasIndex, m_iCurLayerIndex);
	if (iNumUI == 0)
	{
		m_iCurUIIndex = 0;
		return;
	}
	if (iNewUIIndex > iNumUI)
		return;
	
	m_iCurUIIndex = iNewUIIndex;
}

uint32_t CImGui_UIManager::Get_NumCanvas()
{
	return static_cast<uint32_t>(m_vecCanvasData.size());
}

uint32_t CImGui_UIManager::Get_NumLayer(uint32_t iCanvasIndex)
{
	if (iCanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Get_NumLayer, CanvasIndex Out of Range");
		return 0;
	}

	return static_cast<uint32_t>(m_vecCanvasData[iCanvasIndex].vecLayers.size());
}

uint32_t CImGui_UIManager::Get_CurNumLayer()
{
	if (m_iCurCanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Get_NumLayer, CanvasIndex Out of Range");
		return 0;
	}

	return static_cast<uint32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers.size());
}

uint32_t CImGui_UIManager::Get_NumUI(uint32_t iCanvasIndex, uint32_t iLayerIndex)
{
	if (iCanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Get_NumUI, CanvasIndex Out of Range");
		return 0;
	}

	uint32_t iNumLayer = static_cast<uint32_t>(m_vecCanvasData[iCanvasIndex].vecLayers.size());
	if (iLayerIndex >= iNumLayer)
	{
		MSG_BOX("CImGui_UIManager::Get_NumUI, LayerIndex Out of Range");
		return 0;
	}

	return static_cast<uint32_t>(m_vecCanvasData[iCanvasIndex].vecLayers[iLayerIndex].vecUIData.size());
}

uint32_t CImGui_UIManager::Get_CurNumUI()
{
	if (m_iCurCanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Get_NumUI, CanvasIndex Out of Range");
		return 0;
	}

	uint32_t iNumLayer = static_cast<uint32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers.size());
	if (m_iCurLayerIndex >= iNumLayer)
	{
		MSG_BOX("CImGui_UIManager::Get_NumUI, LayerIndex Out of Range");
		return 0;
	}

	return static_cast<uint32_t>(m_vecCanvasData[m_iCurCanvasIndex].vecLayers[m_iCurLayerIndex].vecUIData.size());
}

void CImGui_UIManager::Remove_CanvasData()
{
	m_vecCanvasData.erase(m_vecCanvasData.begin() + m_iCurCanvasIndex);
	Change_Canvas(0);
}

void CImGui_UIManager::Remove_LayerData()
{
	Get_CurLayers_Ref().erase(Get_CurLayers_Ref().begin() + m_iCurLayerIndex);
	Change_Layers(0);
}

void CImGui_UIManager::Remove_UIData()
{
	Get_CurUIDatas_Ref().erase(Get_CurUIDatas_Ref().begin() + m_iCurUIIndex);
	Change_UIData(0);
}

CANVAS_DATA* CImGui_UIManager::Get_CanvasData_Ptr(uint32_t CanvasIndex)
{
	if (CanvasIndex >= Get_NumCanvas())
	{
		MSG_BOX("CImGui_UIManager::Get_CanvasData_Ptr, Index Out of Range");
		return nullptr;
	}
	return &m_vecCanvasData[CanvasIndex];
}

LAYER_DATA* CImGui_UIManager::Get_LayerData_Ptr(uint32_t LayerIndex)
{
	if (LayerIndex >= Get_NumLayer(m_iCurCanvasIndex))
	{
		MSG_BOX("CImGui_UIManager::Get_LayerData_Ptr, Index Out of Range");
		return nullptr;
	}
	return &Get_CurLayers_Ref()[LayerIndex];
}

GENERIC_UI_DATA* CImGui_UIManager::Get_UIData_Ptr(uint32_t UIIndex)
{
	if (UIIndex >= Get_NumUI(m_iCurCanvasIndex, m_iCurLayerIndex))
	{
		MSG_BOX("CImGui_UIManager::Get_UIData_Ptr, Index Out of Range");
		return nullptr;
	}
	return &Get_CurUIDatas_Ref()[UIIndex];
}

CToolUI* CImGui_UIManager::Get_UI_Ptr(uint32_t UIIndex)
{
	if (UIIndex >= Get_NumUI(m_iCurCanvasIndex, m_iCurLayerIndex))
	{
		MSG_BOX("CImGui_UIManager::Get_UIData_Ptr, Index Out of Range");
		return nullptr;
	}
	return Get_CurLayers_Ref()[m_iCurLayerIndex].vecUIObjects[UIIndex];
}

void CImGui_UIManager::Free()
{
	Super::Free();
}

