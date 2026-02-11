#include "pch.h"
#include "Builder_UI.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UIProgress_Bar.h"
#include "UIJust_Image.h"
#include "UIText.h"
#include "UITrigger.h"

#include "UISkill_BG.h"
#include "UIMini_Map.h"

#include"UI_Manager.h"
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

	D3D11_VIEWPORT vp{};
	UINT numVP = 1;
	m_pDeviceContext->RSGetViewports(&numVP, &vp);
	m_vViewportSIze.x = vp.Width;
	m_vViewportSIze.y = vp.Height;

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

	// For. TextData
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::UI_TEXT));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_Text_DTO*>(pObjectData);
			if (FAILED(Create_TextDTO(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	// For. Trigger
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::TRIGGER));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_Trigger_DTO*>(pObjectData);
			if (FAILED(Create_TriggerDTO(pDto->Get_Data())))
				return E_FAIL;
		}
	}

	// For. DImage
	{
		const vector<Engine::IObjectDataBase*> vecDataList = doc.Get_ListByType(ENUM_TO_UINT(DTO::EUIType::DYNAMIC_IMAGE));
		for (const auto& pObjectData : vecDataList)
		{
			const auto* pDto = static_cast<const Engine::CUI_DImage_DTO*>(pObjectData);
			if (FAILED(Create_DImageDTO(pDto->Get_Data())))
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

	if (FAILED(CUI_Manager::GetInstance()->Swap_MapCanvasCache(m_iLevelID, std::move(m_MapCanvasCache))))
		return E_FAIL;
	if (FAILED(CUI_Manager::GetInstance()->Swap_MapGenericUICache(m_iLevelID, std::move(m_pMapUICache))))
		return E_FAIL;

	m_MapTextDataCache.clear();
	m_MapTriggerDataCache.clear();

	CUI_Manager::GetInstance()->Request_SortUI();
	return S_OK;
}

HRESULT CBuilder_UI::Create_CanvasDTO(const DTO::TUI_CanvasData& data)
{
	if (data.eType != DTO::EUIType::CANVAS)
		return E_FAIL;

	CCanvas::CANVAS_DESC Desc = {};
	Desc.iLevelIndex = data.iLevelIndex;
	Desc.strName = data.strTag;
	m_vAspect.x = (_float)g_iWinSizeX / (_float)data.iEditorSizeX;
	m_vAspect.y = (_float)g_iWinSizeY / (_float)data.iEditorSizeY;
	Desc.fX = data.fPosX * m_vAspect.x;
	Desc.fY = data.fPosY * m_vAspect.y;
	Desc.fZ = data.fPosZ;
	Desc.fWidth = m_vViewportSIze.x;
	Desc.fHeight = m_vViewportSIze.y;

	const _wstring wstrLayerTag = Engine_Utils::ToWString(data.strTag) + L"_Layer";
	CGameObject* pResult = m_pGameInstance->Add_GameObject(m_iLevelID, L"Prototype_UI_Canvas", m_iLevelID, wstrLayerTag, &Desc);
	if (pResult == nullptr)
		return E_FAIL;

	auto* pCanvas = dynamic_cast<CCanvas*>(pResult);
	if (nullptr == pCanvas)
		return E_FAIL;

	m_MapCanvasCache.emplace(data.strTag, pCanvas);

	if (FAILED(CUI_Manager::GetInstance()->Add_VecCanvasCache(m_iLevelID, pCanvas)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuilder_UI::Create_GenericUIDTO(const DTO::TUI_GenericUIData& data)
{
	if (data.eType != DTO::EUIType::GENERICUI)
		return E_FAIL;

	auto iter = m_MapCanvasCache.find(data.strCanvasName);
	if (iter == m_MapCanvasCache.end())
		return E_FAIL;

	if (FAILED(Register_Class(data.eClassType, data, iter->second)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuilder_UI::Create_TextDTO(const DTO::TUI_TextData& data)
{
	if (data.eType != DTO::EUIType::UI_TEXT)
		return E_FAIL;

	m_MapTextDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UI::Create_TriggerDTO(const DTO::TUI_TriggerData& data)
{
	if (data.eType != DTO::EUIType::TRIGGER)
		return E_FAIL;

	m_MapTriggerDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UI::Create_DImageDTO(const DTO::TUI_DImageData& data)
{
	if (data.eType != DTO::EUIType::DYNAMIC_IMAGE)
		return E_FAIL;

	m_MapDImageDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UI::Register_Class(DTO::EUIClassType eClassType, const DTO::TUI_GenericUIData& data, CCanvas* pCanvas)
{
	if (nullptr == pCanvas)
		return E_FAIL;

	CGenericUI::GENERIC_UI_DESC DefaultDesc = Make_DefaultInfo(data, pCanvas);
	const _wstring wstrLayerTag = Engine_Utils::ToWString(pCanvas->Get_Name()) + L"_Layer";
	const _wstring wstrProtoTag = L"Prototype_UI_" + Engine_Utils::ToWString(DTO::UIClassTypeToString(eClassType));

	CGameObject* pResult = nullptr;

	if (eClassType == DTO::EUIClassType::PROGRESS_BAR)
	{
		CUIProgress_Bar::PROGRESS_BAR_DESC ProgressDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(ProgressDesc) = DefaultDesc;
		ProgressDesc.eOwner = data.eSubClassType;
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &ProgressDesc);
	}
	else if (eClassType == DTO::EUIClassType::UI_TEXT)
	{
		CUIText::UI_TEXT_DESC TextDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(TextDesc) = DefaultDesc;
		TextDesc.eOwner = data.eSubClassType;
		auto iter = m_MapTextDataCache.find(data.strTag);
		if (iter == m_MapTextDataCache.end())
			return E_FAIL;
		TextDesc.wstrText = Engine_Utils::ToWString( iter->second.strText);
		TextDesc.vFontColor = iter->second.vFontColor;
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &TextDesc);
	}
	else if (eClassType == DTO::EUIClassType::JUST_IMAGE)
	{
		CUIJust_Image::JUST_IMAGE_DESC JustImageDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(JustImageDesc) = DefaultDesc;
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &JustImageDesc);
	}
	else if (eClassType == DTO::EUIClassType::TRIGGER)
	{
		CUITrigger::UI_TRIGGER_DESC TriggerDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(TriggerDesc) = DefaultDesc;
		TriggerDesc.eOwner = data.eSubClassType;
		auto iter = m_MapTriggerDataCache.find(data.strTag);
		if (iter == m_MapTriggerDataCache.end())
			return E_FAIL;
		TriggerDesc.tTriggerData = std::move(iter->second);
		m_MapTriggerDataCache.erase(iter);	

		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &TriggerDesc);
	}
	else if (eClassType == DTO::EUIClassType::DYNAMIC_IMAGE)
	{
		auto iter = m_MapDImageDataCache.find(data.strTag);
		if (iter == m_MapDImageDataCache.end())
			return E_FAIL;

		const auto Type = iter->second.eDISubClassType;

		// 플레이어 스킬류일때 
		const bool isPlayerSkill = (Type >= DTO::EUIDImageSubClassType::PLAYER_E &&	Type <= DTO::EUIDImageSubClassType::PLAYER_SKILL_END);
		const bool isMiniMap = (Type >= DTO::EUIDImageSubClassType::MINIMAP_PLAYER_ICON &&	Type <= DTO::EUIDImageSubClassType::MINIMAP_END);
		if (isPlayerSkill)
		{
			CUISkill_BG::SKILL_BG_DESC SkillBGDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(SkillBGDesc) = DefaultDesc;
			SkillBGDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(m_iLevelID, L"Prototype_UI_SkillBG", m_iLevelID, wstrLayerTag, &SkillBGDesc);
		}
		
		else if (isMiniMap)
		{
			CUIMini_Map::MINIMAP_DESC SkillBGDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(SkillBGDesc) = DefaultDesc;
			SkillBGDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(m_iLevelID, L"Prototype_UI_MiniMap", m_iLevelID, wstrLayerTag, &SkillBGDesc);
		}
	}
	else
	{
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &DefaultDesc);
	}

	if (pResult == nullptr)
		return E_FAIL;

	auto* pUI = dynamic_cast<CGenericUI*>(pResult);
	if (nullptr == pUI)
		return E_FAIL;

	pCanvas->Get_UIVector()->push_back(pUI);
	m_pMapUICache.emplace(data.strTag, pUI);

	if (FAILED(CUI_Manager::GetInstance()->Add_VecGenericUICache(m_iLevelID, pUI)))
		return E_FAIL;

	return S_OK;
}

CGenericUI::GENERIC_UI_DESC CBuilder_UI::Make_DefaultInfo(const DTO::TUI_GenericUIData& data, CCanvas* pCanvas)
{
	CGenericUI::GENERIC_UI_DESC Desc = {};
	Desc.strName				= data.strTag;
	Desc.iLevelIndex			= m_iLevelID;
	Desc.iRectTransformType		= data.iRectTransformType;
	Desc.fWidth					= data.fWidth * m_vAspect.x;
	Desc.fHeight				= data.fHeight * m_vAspect.y;
	Desc.fX						= data.fPosX * m_vAspect.x;
	Desc.fY						= data.fPosY * m_vAspect.y;
	Desc.fZ						= data.fPosZ;
	Desc.wstrTextureTag			= Engine_Utils::ToWString(data.strTextureTag);
	Desc.isAlpha				= TRUE;
	Desc.isInitVisible			= data.isVisible;
	Desc.pCanvasCache			= pCanvas;
	Desc.iComponentFlag			= data.iComponentFlag;
	Desc.isUseColorTint			= data.isUseColorTint;
	Desc.vColorTint				= data.vColorTint;
	Desc.iShaderPass			= data.iShaderPass;
	Desc.fDelay					= data.fDelay;
	Desc.iFillDir				= data.iFillDir;
	Desc.fAlpha					= data.fAlphaRatio;

	return Desc;
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
