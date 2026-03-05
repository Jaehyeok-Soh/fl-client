#include "pch.h"
#include "Builder_UI.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UIJust_Image.h"

// 프로그레스 클래스
#include "UIPlayerStat_Progress.h"
#include "UILoading_Progress.h"
#include "UIMonsterStat_Progress.h"
#include "UIPlayerAmmo_Progress.h"
// 텍스트 클래스
#include "UIMenu_Text.h"
#include "UIPlayerStat_Text.h"
#include "UILoading_Text.h"
#include "UIMonsterStat_Text.h"
#include "UICombo_Text.h"
#include "UIBossAction_Text.h"
// 다이나믹 이미지 클래스
#include "UIMenu_Image.h"
#include "UIHover_Image.h"
#include "UIMini_Map.h"
#include "UISkill_BG.h"
#include "UIMenu_OutLine.h"
#include "UILoading_Image.h"
#include "UINameplate_BG.h"
#include "UIAimDot_Image.h"	
#include "UILevelChange_Image.h"
#include "UICombo_Image.h"
#include "UIBossAction_Image.h"

// 트리거 클래스
#include "UICommon_Trigger.h"
#include "UIMenu_Trigger.h"
#include "UIMenu_Exit_Trigger.h"

#include "WorldUI_Component.h"

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

	if (FAILED(CUI_Manager::GetInstance()->Merge_MapCanvasCache(m_iLevelID, std::move(m_MapCanvasCache))))
		return E_FAIL;
	if (FAILED(CUI_Manager::GetInstance()->Merge_MapGenericUICache(m_iLevelID, std::move(m_pMapUICache))))
		return E_FAIL;

	m_MapTextDataCache.clear();
	m_MapTriggerDataCache.clear();

	CUI_Manager::GetInstance()->Add_TriggerUI(std::move(m_vecTriggerUIs));
	return S_OK;
}

HRESULT CBuilder_UI::Create_CanvasDTO(const DTO::TUI_CanvasData& data)
{
	if (data.eType != DTO::EUIType::CANVAS)
		return E_FAIL;

	CCanvas::CANVAS_DESC Desc = {};
	Desc.iLevelIndex	= data.iLevelIndex;
	Desc.strName		= data.strTag;
	m_vAspect.x			= (_float)g_iWinSizeX / (_float)data.iEditorSizeX;
	m_vAspect.y			= (_float)g_iWinSizeY / (_float)data.iEditorSizeY;
	Desc.fX				= data.fPosX * m_vAspect.x;
	Desc.fY				= data.fPosY * m_vAspect.y;
	Desc.fZ				= data.fPosZ;
	Desc.fWidth			= m_vViewportSIze.x;
	Desc.fHeight		= m_vViewportSIze.y;
	m_ePrefabtype = static_cast<Client::EUIPrefabType>(data.iPrefabType);

	CGameObject* pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_Canvas", m_iLevelID, g_wszUILayer, &Desc);
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
	_wstring wstrProtoTag = L"Prototype_UI_" + Engine_Utils::ToWString(DTO::UIClassTypeToString(eClassType));
	CGameObject* pResult = nullptr;

	////////////////////////////////////////
	// PROGRESS_BAR //
	if (eClassType == DTO::EUIClassType::PROGRESS_BAR)
	{
		const auto Type = data.eSubClassType;
		const _bool isPlayerStat	= Type >= DTO::EUISubClassType::PLAYER_STAT_BEGIN && Type <= DTO::EUISubClassType::PLAYER_STAT_END;
		const _bool isLoading		= Type == DTO::EUISubClassType::LOADING_PROGRESS;
		const _bool isMonsterStat	= Type >= DTO::EUISubClassType::MONSTER_STAT_BEGIN && Type <= DTO::EUISubClassType::MONSTER_STAT_END;
		const _bool isPlayerAmmo	= Type == DTO::EUISubClassType::PLAYER_AMMO_PROGRESS;

		if (isPlayerStat)
		{
			CUIPlayerStat_Progress::PLAYER_STAT_PROGRESS_DESC  PlayerStatProgressDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(PlayerStatProgressDesc) = DefaultDesc;
			PlayerStatProgressDesc.eOwner = data.eSubClassType;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_PlayerStatProgress", m_iLevelID, g_wszUILayer, &PlayerStatProgressDesc);
		}
		else if (isLoading)
		{
			CUILoading_Progress::LOADING_PROGRESS_DESC  LoadingProgressDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(LoadingProgressDesc) = DefaultDesc;
			LoadingProgressDesc.eOwner = data.eSubClassType;
			LoadingProgressDesc.pLoadingRatio = CUI_Manager::GetInstance()->Get_LoadingRatio();
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LoadingProgress", m_iLevelID, g_wszUILayer, &LoadingProgressDesc);
		}
		else if (isPlayerAmmo)
		{
			CUIPlayerAmmo_Progress::PLAYER_AMMO_PROGRESS_DESC  PlayerAmmoDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(PlayerAmmoDesc) = DefaultDesc;
			PlayerAmmoDesc.eOwner = data.eSubClassType;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_PlayerAmmoProgress", m_iLevelID, g_wszUILayer, &PlayerAmmoDesc);
		}
		else
		{
			_wstring wstr = Engine_Utils::ToWString(data.strTag) + L" <- 얘가 문제";
			MSG_BOXW(wstr.c_str());
			return E_FAIL;
		}
	}

	////////////////////////////////////////
	// UI_TEXT //
	else if (eClassType == DTO::EUIClassType::UI_TEXT)
	{
		CUIText::UI_TEXT_DESC TextDesc = {};
		auto iter = m_MapTextDataCache.find(data.strTag);
		if (iter == m_MapTextDataCache.end())
			return E_FAIL;

		const auto Type					= iter->second.eTextSubClassType;
		const _bool isPlayerStat		= (Type >= DTO::EUITextSubClassType::PLAYER_STAT_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::PLAYER_STAT_TEXT_END);
		const _bool isMenu				= (Type >= DTO::EUITextSubClassType::MENU_TEXT_BEGIN	&& Type <= DTO::EUITextSubClassType::MENU_TEXT_END);
		const _bool isLoading			= (Type >= DTO::EUITextSubClassType::LOADING_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::LOADING_TEXT_END);
		const _bool isMonsterNameplate	= (Type >= DTO::EUITextSubClassType::MONSTER_STAT_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::MONSTER_STAT_TEXT_END);
		const _bool isCombo				= (Type >= DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::BATTLE_COMBO_TEXT_END);
		const _bool isBossCivila		= (Type >= DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_BEGIN && Type <= DTO::EUITextSubClassType::BOSS_CIVILA_ACTION_END);

		static_cast<CGenericUI::GENERIC_UI_DESC&>(TextDesc) = DefaultDesc;
		TextDesc.eTextSubClass	= Type;
		TextDesc.eShaderType	= iter->second.eShaderType;
		TextDesc.wstrFontTag	= Engine_Utils::ToWString(iter->second.strFontTag);
		TextDesc.wstrText		= Engine_Utils::ToWString(iter->second.strText);
		TextDesc.vFontColor		= iter->second.vFontColor;
		TextDesc.ePivot			= iter->second.ePivot;
		TextDesc.fRotate		= iter->second.fRotate;
		TextDesc.fScale			= iter->second.fScale * m_vAspect.x;

		if (isPlayerStat)
		{
			CUIPlayerStat_Text::PLAYER_STAT_DESC PlayerStatTextDesc= {};
			static_cast<CUIText::UI_TEXT_DESC&>(PlayerStatTextDesc) = TextDesc;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_PlayerStatText", m_iLevelID, g_wszUILayer, &PlayerStatTextDesc);

		}
		else if (isMenu)
		{
			CUIMenu_Text::MENU_TEXT_DESC MenuTextDesc = {};
			static_cast<CUIText::UI_TEXT_DESC&>(MenuTextDesc) = TextDesc;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_MenuText", m_iLevelID, g_wszUILayer, &MenuTextDesc);
		}
		else if (isLoading)
		{
			CUILoading_Text::LOADING_TEXT_DESC LoadingTextDesc = {};
			static_cast<CUIText::UI_TEXT_DESC&>(LoadingTextDesc) = TextDesc;
			LoadingTextDesc.pLoadingRatio = CUI_Manager::GetInstance()->Get_LoadingRatio();
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LoadingText", m_iLevelID, g_wszUILayer, &LoadingTextDesc);
		}
		else if (isCombo)
		{
			CUICombo_Text::COMBO_TEXT_DESC ComboTextDesc = {};
			static_cast<CUIText::UI_TEXT_DESC&>(ComboTextDesc) = TextDesc;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_ComboText", m_iLevelID, g_wszUILayer, &ComboTextDesc);
		}
		else if (isBossCivila)
		{
			CUIBossAction_Text::BOSS_ACTION_TEXT_DESC BossActionTextDesc= {};
			static_cast<CUIText::UI_TEXT_DESC&>(BossActionTextDesc) = TextDesc;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_BossActionText", m_iLevelID, g_wszUILayer, &BossActionTextDesc);
		}
		else
		{
			_wstring wstr = Engine_Utils::ToWString(data.strTag) + L" <- 얘가 문제";
			MSG_BOXW(wstr.c_str());
			return E_FAIL;
		}
	}

	////////////////////////////////////////
	// JUST_IMAGE //
	else if (eClassType == DTO::EUIClassType::JUST_IMAGE)
	{
		CUIJust_Image::JUST_IMAGE_DESC JustImageDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(JustImageDesc) = DefaultDesc;
		pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), wstrProtoTag, m_iLevelID, g_wszUILayer, &JustImageDesc);
	}

	////////////////////////////////////////
	// TRIGGER //
	else if (eClassType == DTO::EUIClassType::TRIGGER)
	{
		auto iter = m_MapTriggerDataCache.find(data.strTag);
		if (iter == m_MapTriggerDataCache.end())
			return E_FAIL;
		const auto Type = iter->second.eTriggerSubClassType;
		const _bool isMenu = (Type == DTO::EUITriggerSubClassType::MENU_TAB_TRIGGER);
		const _bool isMenuExit = (Type == DTO::EUITriggerSubClassType::MENU_TAB_EXIT_TRIGGER);

		if (isMenu)
		{
			CUIMenu_Trigger::UI_MENU_TRIGGER_DESC MenuTriggerDesc= {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(MenuTriggerDesc) = DefaultDesc;
			MenuTriggerDesc.eTriggerSubClass = Type;
			MenuTriggerDesc.tTriggerData = std::move(iter->second);
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_UIMenuTrigger", m_iLevelID, g_wszUILayer, &MenuTriggerDesc);

		}
		else if (isMenuExit)
		{
			CUIMenu_Exit_Trigger::UI_MENU_EXIT_TRIGGER_DESC MenuExitTriggerDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(MenuExitTriggerDesc) = DefaultDesc;
			MenuExitTriggerDesc.eTriggerSubClass = Type;
			MenuExitTriggerDesc.tTriggerData = std::move(iter->second);
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_UIMenuExitTrigger", m_iLevelID, g_wszUILayer, &MenuExitTriggerDesc);
		}
		else
		{
			CUICommon_Trigger::UI_COMMON_TRIGGER_DESC CommonTriggerDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(CommonTriggerDesc) = DefaultDesc;
			CommonTriggerDesc.eTriggerSubClass = Type;
			CommonTriggerDesc.tTriggerData = std::move(iter->second);
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_UICommonTrigger", m_iLevelID, g_wszUILayer, &CommonTriggerDesc);
		}

		if (nullptr == pResult)
		{
			_wstring wstr = Engine_Utils::ToWString(data.strTag) + L" <- 얘가 문제";
			MSG_BOXW(wstr.c_str());
			return E_FAIL;
		}
		auto* pUI = dynamic_cast<CGenericUI*>(pResult);
		if (nullptr == pUI)
			return E_FAIL;
		auto* pTriggerUI = dynamic_cast<CUITrigger*>(pUI);
		if (nullptr == pTriggerUI)
			return E_FAIL;
		m_vecTriggerUIs.push_back(pTriggerUI);
	}

	////////////////////////////////////////
	// DYNAMIC_IMAGE //
	else if (eClassType == DTO::EUIClassType::DYNAMIC_IMAGE)
	{
		auto iter = m_MapDImageDataCache.find(data.strTag);
		if (iter == m_MapDImageDataCache.end())
			return E_FAIL;
		if ("MiniMap_Player_Icon" == data.strTag)
			int a = 0;
		const auto Type = iter->second.eDISubClassType;

		const _bool isPlayerSkill		= (Type >= DTO::EUIDImageSubClassType::PLAYER_SKILL_BEGIN	&& Type <= DTO::EUIDImageSubClassType::PLAYER_SKILL_END);
		const _bool isMiniMap			= (Type >= DTO::EUIDImageSubClassType::MINIMAP_BEGIN		&& Type <= DTO::EUIDImageSubClassType::MINIMAP_END);
		const _bool isHoverIcon			= (Type >= DTO::EUIDImageSubClassType::HOVER_POPUP_BEGIN	&& Type <= DTO::EUIDImageSubClassType::HOVER_POPUP_END);
		const _bool isMenu				= (Type >= DTO::EUIDImageSubClassType::MENU_BEGIN			&& Type <= DTO::EUIDImageSubClassType::MENU_ICON_BG);
		const _bool isOutLine			= (Type >= DTO::EUIDImageSubClassType::MENU_ICON_OUTLINE	&& Type <= DTO::EUIDImageSubClassType::MENU_END);
		const _bool isLoading			= (Type >= DTO::EUIDImageSubClassType::LOADING_BEGIN		&& Type <= DTO::EUIDImageSubClassType::LOADING_END);
		const _bool isMonsterNameplate	= (Type == DTO::EUIDImageSubClassType::MONSTER_NAMEPLATE_BG);
		const _bool isAimDot			= (Type >= DTO::EUIDImageSubClassType::BATTLE_UI_BEGIN		&& Type <= DTO::EUIDImageSubClassType::BATTLE_UI_END);
		const _bool isLevelChange		= (Type >= DTO::EUIDImageSubClassType::LEVEL_CHAGE_1		&& Type <= DTO::EUIDImageSubClassType::LEVEL_CHAGE_5);
		const _bool isCombo				= (Type >= DTO::EUIDImageSubClassType::BATTLE_COMBO_BEGIN	&& Type <= DTO::EUIDImageSubClassType::BATTLE_COMBO_END);
		const _bool isBossCivila		= (Type >= DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_BEGIN && Type <= DTO::EUIDImageSubClassType::BOSS_CIVILA_ACTION_END);

		if (isPlayerSkill)
		{
			CUISkill_BG::SKILL_BG_DESC SkillBGDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(SkillBGDesc) = DefaultDesc;
			SkillBGDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_SkillBG", m_iLevelID, g_wszUILayer, &SkillBGDesc);
		}
		else if (isMiniMap)
		{
			CUIMini_Map::MINIMAP_DESC SkillBGDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(SkillBGDesc) = DefaultDesc;
			SkillBGDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_MiniMap", m_iLevelID, g_wszUILayer, &SkillBGDesc);
		}
		else if (isHoverIcon)
		{
			CUIHover_Image::HOVER_IMAGE_DESC HoverImageDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(HoverImageDesc) = DefaultDesc;
			HoverImageDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_HoverImage", m_iLevelID, g_wszUILayer, &HoverImageDesc);
		}
		else if (isMenu)
		{
			CUIMenu_Image::MENU_IMAGE_DESC MenuImageDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(MenuImageDesc) = DefaultDesc;
			MenuImageDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_MenuImage", m_iLevelID, g_wszUILayer, &MenuImageDesc);
		}
		else if (isOutLine)
		{
			CUIMenu_OutLine::MENU_OUTLINE_DESC MenuOutlineDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(MenuOutlineDesc) = DefaultDesc;
			MenuOutlineDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_MenuOutline", m_iLevelID, g_wszUILayer, &MenuOutlineDesc);
		}
		else if (isLoading)
		{
			CUILoading_Image::LOADING_IMAGE_DESC LoadingImageDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(LoadingImageDesc) = DefaultDesc;
			LoadingImageDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LoadingImage", m_iLevelID, g_wszUILayer, &LoadingImageDesc);
		}
		else if (isAimDot)
		{
			CUIAimDot_Image::AIMDOT_IMAGE_DESC AimDotImageDesc= {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(AimDotImageDesc) = DefaultDesc;
			AimDotImageDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_AimDotImage", m_iLevelID, g_wszUILayer, &AimDotImageDesc);
		}
		else if (isLevelChange)
		{
			CUILevelChange_Image::LEVEL_CHANGE_DESC LevelChangeDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(LevelChangeDesc) = DefaultDesc;
			LevelChangeDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_LevelChangeImage", m_iLevelID, g_wszUILayer, &LevelChangeDesc);
		}
		else if (isCombo)
		{
			CUICombo_Image::COMBO_IMAGE_DESC ComboImageDesc= {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(ComboImageDesc) = DefaultDesc;
			ComboImageDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_ComboImage", m_iLevelID, g_wszUILayer, &ComboImageDesc);
		}
		else if (isBossCivila)
		{
			CUIBossAction_Image::BOSS_ACTION_IMAGE_DESC BossActionImageDesc= {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(BossActionImageDesc) = DefaultDesc;
			BossActionImageDesc.eSubClassType = Type;
			pResult = m_pGameInstance->Add_GameObject(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_UI_BossActionImage", m_iLevelID, g_wszUILayer, &BossActionImageDesc);
		}
		else
		{
			_wstring wstr = Engine_Utils::ToWString(data.strTag) + L" <- 얘가 문제";
			MSG_BOXW(wstr.c_str());
			return E_FAIL;
		}
	}

	if (pResult == nullptr)
	{
		_wstring wstr = Engine_Utils::ToWString(data.strTag) + L" <- 얘가 문제";
		MSG_BOXW(wstr.c_str());
		return E_FAIL;
	}

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
	Desc.fHeight				= data.fHeight * m_vAspect.x;
	Desc.fX						= data.fPosX * m_vAspect.x;
	Desc.fY						= data.fPosY * m_vAspect.y;
	Desc.fZ						= data.fPosZ;
	Desc.fScale					= data.fScale;
	Desc.fRotate				= data.fRotate;
	Desc.wstrTextureTag			= Engine_Utils::ToWString(data.strTextureTag);
	Desc.wstrNoiseTextureTag	= Engine_Utils::ToWString(data.strNoiseTextureTag);
	Desc.wstrAlphaMaskTextureTag= Engine_Utils::ToWString(data.strAlphaMaskTextureTag);
	Desc.wstrGlowTextureTag		= Engine_Utils::ToWString(data.strGlowTextureTag);
	Desc.isAlpha				= TRUE;
	Desc.isInitVisible			= data.isVisible;
	Desc.isInitInteract			= data.isInteract;
	Desc.isInitActivate			= data.isActivate;
	Desc.isUseColorTint			= data.isUseColorTint;
	Desc.pCanvasCache			= pCanvas;
	Desc.iComponentFlag			= data.iComponentFlag;
	Desc.isUseColorTint			= data.isUseColorTint;
	Desc.vColorTint				= data.vColorTint;
	Desc.vGradiantColorTint		= data.vGradiantColorTint;
	Desc.iShaderPass			= data.iShaderPass;
	Desc.fDelay					= data.fDelay;
	Desc.iFillDir				= data.iFillDir;
	Desc.fAlpha					= data.fAlphaRatio;
	Desc.iFlip					= data.iFlip;

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
