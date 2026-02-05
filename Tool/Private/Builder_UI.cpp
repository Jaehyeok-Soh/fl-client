#include "pch.h"
#include "Builder_UI.h"

#include "ToolCanvas.h"
#include "ToolLayer.h"
#include "ToolUI.h"

#include "ImGui_UIManager.h"
#include "UIAction_Registry.h"
#include "GameInstance.h"

CBuilder_UI::CBuilder_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	:Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_UI::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_UI::Build(const CDataDocumentBase& document)
{
	if (document.Get_Category() != DTO::ECategory::UI)
		return E_FAIL;

	const auto& doc = static_cast<const CDataDocument_UI&>(document);
	// For. Canvas
	{
		/* 문서에 저장된 IObjectDataBase -> 데이터를 가진 클래스의 부모 */
		const vector<Engine::IObjectDataBase*> vecDtoList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::CANVAS));
		for (const auto& pDtoBase : vecDtoList)
		{
			/* 데이터를 보유한 클래스 다운캐스팅 */
			const auto* pDto = static_cast<const Engine::CUI_Canvas_DTO*>(pDtoBase);
			/* 데이터를 보유한 클래스에서 데이터를 추출 -> 오브젝트 매니저 레이어에 추가까지 */
			if (FAILED(Create_CanvasDTO(pDto->Get_Data())))
				return E_FAIL;
		}
	}
	// For. Layer
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::LAYER));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_Layer_DTO*>(pObjectData);
			if (FAILED(Create_LayerDTO(pDto->Get_Data())))
				return E_FAIL;
		}
	}
	// For. GenericUI
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::GENERICUI));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_GenericUI_DTO*>(pObjectData);
			if (FAILED(Create_GenericUIDTO(pDto->Get_Data())))
				return E_FAIL;
		}
	}
	// For. Event
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::EVENT));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_EventBindData_DTO*>(pObjectData);
			if (FAILED(Create_EventBindDataDTO(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	CImGui_UIManager::GetInstance()->Move_CanvasCache(m_pCanvasCache);
	CImGui_UIManager::GetInstance()->Move_LayerCache(m_pLayerCache);
	CImGui_UIManager::GetInstance()->Move_UICache(m_pUICache);
	return S_OK;
}

HRESULT CBuilder_UI::Create_CanvasDTO(const DTO::TUI_CanvasData& data)
{
	if (data.eType != DTO::EUIType::CANVAS)
		return E_FAIL;

	/* 데이터를 이용해서 Object 만들기 */
	CToolCanvas::TOOLCANVAS_DESC Desc = {};
	Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::UI);

	Desc.strTag = data.strTag;
	Desc.fWidth = data.fWidth;
	Desc.fHeight = data.fHeight;
	Desc.fX = data.fPosX;
	Desc.fY = data.fPosY;
	Desc.fZ = data.fPosZ;

	_wstring wstrLayerTag = Engine_Utils::ToWString(Desc.strTag) + L"_Layer";
	CGameObject* pResult = m_pGameInstance->Add_GameObject(Desc.iLevelIndex, g_wszPrototypeTagCanvas, Desc.iLevelIndex, wstrLayerTag, &Desc);
	 if (nullptr == pResult)
		return E_FAIL;

	 CToolCanvas* pCanvas = dynamic_cast<CToolCanvas*>(pResult);
	 if (nullptr == pCanvas)
		 return E_FAIL;

	 if (FAILED(CImGui_UIManager::GetInstance()->Safe_Add_Canvas(pCanvas)))
		 return E_FAIL;

	 m_pCanvasCache.emplace(data.strTag, pCanvas);
	return S_OK;
}

HRESULT CBuilder_UI::Create_LayerDTO(const DTO::TUI_LayerData& data)
{
	if (data.eType != DTO::EUIType::LAYER)
		return E_FAIL;

	CToolLayer::TOOLLAYER_DESC Desc = {};
	Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::UI);
	Desc.strTag = data.strTag;
	Desc.isInitVisible = TRUE;
	Desc.strCanvasName = data.strCanvasName;

	_wstring wstrLayerTag = Engine_Utils::ToWString(Desc.strCanvasName) + L"_Layer";
	CGameObject* pResult = m_pGameInstance->Add_GameObject(Desc.iLevelIndex, g_wszPrototypeTagLayer, Desc.iLevelIndex, wstrLayerTag, &Desc);
	if (pResult == nullptr)
		return E_FAIL;
	 
	auto* pLayer = dynamic_cast<CToolLayer*>(pResult);
	if (nullptr == pLayer)
		return E_FAIL;

	auto iter = m_pCanvasCache.find(Desc.strCanvasName);
	if (iter == m_pCanvasCache.end())
		return E_FAIL;

	if (FAILED(iter->second->Safe_Add_Layer(pLayer)))
		return E_FAIL;
	
	m_pLayerCache.emplace(Desc.strTag, pLayer);
	
	return S_OK;
}

HRESULT CBuilder_UI::Create_GenericUIDTO(const DTO::TUI_GenericUIData& data)
{
	if (data.eType != DTO::EUIType::GENERICUI)
		return E_FAIL;

	/* 데이터를 이용해서 Object 만들기 */
	CToolUI::TOOLUI_DESC Desc = {};
	Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::UI);

	Desc.strName = data.strTag;
	Desc.iRectTransformType = data.iRectTransformType;
	Desc.fWidth = data.fWidth;
	Desc.fHeight = data.fHeight; 
	Desc.fX = data.fPosX;
	Desc.fY = data.fPosY;
	Desc.fZ = data.fPosZ;
	Desc.strInitTextureTag = data.strTextureTag;
	Desc.iInitTextureIndex = data.iTextureIndex;
	Desc.strCanvasName = data.strCanvasName;
	Desc.strLayerName = data.strLayerName;

	auto iterCanvas = m_pCanvasCache.find(Desc.strCanvasName);
	if (iterCanvas != m_pCanvasCache.end())
		Desc.pCacheCanvas = iterCanvas->second;

	auto iterLayer = m_pLayerCache.find(Desc.strLayerName);
	if (iterLayer != m_pLayerCache.end())
		Desc.pCacheLayer = iterLayer->second;

	_wstring wstrLayerTag = Engine_Utils::ToWString(data.strCanvasName) + L"_Layer";
	CGameObject* pResult = m_pGameInstance->Add_GameObject(Desc.iLevelIndex, g_wszPrototypeTagUI,Desc.iLevelIndex, wstrLayerTag, &Desc);
	if (pResult == nullptr)
		return E_FAIL;

	auto* pUI = dynamic_cast<CToolUI*>(pResult);
	if (nullptr == pUI)
		return E_FAIL;

	auto iter = m_pLayerCache.find(data.strLayerName);
	if (iter == m_pLayerCache.end())
		return E_FAIL;

	if (FAILED(iter->second->Safe_Add_UI(pUI)))
		return E_FAIL;

	m_pUICache.emplace(data.strTag, pUI);

	return S_OK;
}

HRESULT CBuilder_UI::Create_EventBindDataDTO(const DTO::TUI_EventBindData& data)
{
	if (data.eType != DTO::EUIType::EVENT)
		return E_FAIL;

	auto iter = m_pUICache.find(data.strOwnerTag);
	if (iter == m_pUICache.end())
		return E_FAIL;

	if (FAILED(iter->second->Bind_Action(data)))
		return E_FAIL;

	return S_OK;
}

CBuilder_UI* CBuilder_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_UI* pInstance = new CBuilder_UI(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_UI::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_UI::Free()
{
	Super::Free();
}
