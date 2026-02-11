#include "pch.h"
#include "Builder_UI.h"

#include "ToolCanvas.h"
#include "ToolUI.h"

#include "ImGui_UIManager.h"
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
	// For. Text
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::UI_TEXT));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_Text_DTO*>(pObjectData);
			if (FAILED(Create_TextDTO(pDto->Get_Data())))
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

	CImGui_UIManager::GetInstance()->Move_CanvasCache(m_pCanvasCache);
	CImGui_UIManager::GetInstance()->Move_UICache(m_pUICache);

	m_TextDataCache.clear();
	m_TriggerDataCache.clear();

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

HRESULT CBuilder_UI::Create_GenericUIDTO(const DTO::TUI_GenericUIData& data)
{
	if (data.eType != DTO::EUIType::GENERICUI)
		return E_FAIL;

	/* 데이터를 이용해서 Object 만들기 */
	CToolUI::TOOLUI_DESC Desc = {};
	Desc.iLevelIndex = static_cast<uint32_t>(ELevelType::UI);
	Desc.eClassType			= data.eClassType;
	Desc.strName			= data.strTag;
	Desc.iRectTransformType = data.iRectTransformType;
	Desc.fWidth				= data.fWidth;
	Desc.fHeight			= data.fHeight; 
	Desc.fX					= data.fPosX;
	Desc.fY					= data.fPosY;
	Desc.fZ					= data.fPosZ;
	Desc.strInitTextureTag	= data.strTextureTag;
	Desc.strCanvasName		= data.strCanvasName;
	Desc.isInitVisible		= data.isVisible;
	Desc.isUseColorTint		= data.isUseColorTint;
	Desc.vColorTint			= data.vColorTint;
	Desc.iShaderPass		= data.iShaderPass;
	Desc.iFillDir			= data.iFillDir;
	Desc.iFlip				= data.iFlip;
	Desc.fDelay				= data.fDelay;
	if (data.eClassType == DTO::EUIClassType::UI_TEXT)
	{
		auto iter = m_TextDataCache.find(data.strTag);
		if (iter == m_TextDataCache.end())
			return E_FAIL;

		Desc.tTextData = iter->second;
	}
	else if (data.eClassType == DTO::EUIClassType::TRIGGER)
	{
		auto iter = m_TriggerDataCache.find(data.strTag);
		if (iter == m_TriggerDataCache.end())
			return E_FAIL;

		Desc.tTriggerData = iter->second;
	}
	
	auto iterCanvas = m_pCanvasCache.find(Desc.strCanvasName);
	if (iterCanvas != m_pCanvasCache.end())
		Desc.pCacheCanvas = iterCanvas->second;

	_wstring wstrLayerTag = Engine_Utils::ToWString(data.strCanvasName) + L"_Layer";
	CGameObject* pResult = m_pGameInstance->Add_GameObject(Desc.iLevelIndex, g_wszPrototypeTagUI,Desc.iLevelIndex, wstrLayerTag, &Desc);
	if (pResult == nullptr)
		return E_FAIL;

	auto* pUI = dynamic_cast<CToolUI*>(pResult);
	if (nullptr == pUI)
		return E_FAIL;

	if (FAILED(iterCanvas->second->Safe_Add_UI(pUI)))
		return E_FAIL;

	m_pUICache.emplace(data.strTag, pUI);

	return S_OK;
}

HRESULT CBuilder_UI::Create_TextDTO(const DTO::TUI_TextData& data)
{
	if (data.eType != DTO::EUIType::UI_TEXT)
		return E_FAIL;

	m_TextDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UI::Create_TriggerDTO(const DTO::TUI_TriggerData& data)
{
	if (data.eType != DTO::EUIType::TRIGGER)
		return E_FAIL;

	m_TriggerDataCache.emplace(data.strOwnerName, data);
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
