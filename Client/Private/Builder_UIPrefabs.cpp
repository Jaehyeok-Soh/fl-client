#include "pch.h"
#include "Builder_UIPrefabs.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UIJust_Image.h"

// 프로그레스 클래스
#include "UIMonsterStat_Progress.h"
// 텍스트 클래스
#include "UIMonsterStat_Text.h"
// 다이나믹 이미지 클래스
#include "UINameplate_BG.h"
// 트리거 클래스

#include"UI_Manager.h"
#include "GameInstance.h"

CBuilder_UIPrefabs::CBuilder_UIPrefabs(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
	:Super(pDevice, pDeviceContext, iLevelID)
{
}

HRESULT CBuilder_UIPrefabs::Initialize()
{
	return S_OK;
}

HRESULT CBuilder_UIPrefabs::Build(const CDataDocumentBase& document)
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
	CUI_Manager::GetInstance()->Request_SortUI();
	return S_OK;
}

HRESULT CBuilder_UIPrefabs::Create_CanvasDTO(const DTO::TUI_CanvasData& data)
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


	switch (m_ePrefabtype)
	{
	case Client::EUIPrefabType::MONSTER_NAMEPLATE:
		break;
	case Client::EUIPrefabType::END:
		break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CBuilder_UIPrefabs::Create_GenericUIDTO(const DTO::TUI_GenericUIData& data)
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

HRESULT CBuilder_UIPrefabs::Create_TextDTO(const DTO::TUI_TextData& data)
{
	if (data.eType != DTO::EUIType::UI_TEXT)
		return E_FAIL;

	m_MapTextDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UIPrefabs::Create_TriggerDTO(const DTO::TUI_TriggerData& data)
{
	if (data.eType != DTO::EUIType::TRIGGER)
		return E_FAIL;

	m_MapTriggerDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UIPrefabs::Create_DImageDTO(const DTO::TUI_DImageData& data)
{
	if (data.eType != DTO::EUIType::DYNAMIC_IMAGE)
		return E_FAIL;

	m_MapDImageDataCache.emplace(data.strOwnerName, data);
	return S_OK;
}

HRESULT CBuilder_UIPrefabs::Register_Class(DTO::EUIClassType eClassType, const DTO::TUI_GenericUIData& data, CCanvas* pCanvas)
{
	if (nullptr == pCanvas)
		return E_FAIL;

	CGenericUI::GENERIC_UI_DESC DefaultDesc = Make_DefaultInfo(data, pCanvas);
	_wstring wstrProtoTag	= {};
	CGameObject* pResult	= nullptr;

	////////////////////////////////////////
	// PROGRESS_BAR //
	if (eClassType == DTO::EUIClassType::PROGRESS_BAR)
	{
		const auto Type = data.eSubClassType;
		const _bool isPlayerStat	= Type >= DTO::EUISubClassType::PLAYER_STAT_BEGIN && Type <= DTO::EUISubClassType::PLAYER_STAT_END;
		const _bool isLoading		= Type == DTO::EUISubClassType::LOADING_PROGRESS;
		const _bool isMonsterStat	= Type >= DTO::EUISubClassType::MONSTER_STAT_BEGIN && Type <= DTO::EUISubClassType::MONSTER_STAT_END;

		if (isMonsterStat)
		{
			CUIMonsterStat_Progress::MONSTER_STAT_PROGRESS_DESC  MonsterStatProgressDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(MonsterStatProgressDesc) = DefaultDesc;
			MonsterStatProgressDesc.eOwner = data.eSubClassType;
			MonsterStatProgressDesc.iComponentFlag = DTO::EComponentTypeFlag::WORLDUI_COMPONENT;
			wstrProtoTag = L"Prototype_UI_MonsterStatProgress";
			_wstring wstrPoolTag = L"Prefab_" + Engine_Utils::ToWString(MonsterStatProgressDesc.strName);
			if (FAILED(CUI_Manager::GetInstance()->Regist_Prefab(m_iLevelID, m_ePrefabtype, wstrProtoTag, wstrPoolTag, &MonsterStatProgressDesc)))
			{
				data.strTag;
				return E_FAIL;
			}
		}
		else
		{
			data.strTag;
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

		const auto Type = iter->second.eTextSubClassType;
		const _bool isPlayerStat		= (Type >= DTO::EUITextSubClassType::PLAYER_STAT_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::PLAYER_STAT_TEXT_END);
		const _bool isMenu				= (Type >= DTO::EUITextSubClassType::MENU_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::MENU_TEXT_END);
		const _bool isLoading			= (Type >= DTO::EUITextSubClassType::LOADING_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::LOADING_TEXT_END);
		const _bool isMonsterNameplate	= (Type >= DTO::EUITextSubClassType::MONSTER_STAT_TEXT_BEGIN && Type <= DTO::EUITextSubClassType::MONSTER_STAT_TEXT_END);

		static_cast<CGenericUI::GENERIC_UI_DESC&>(TextDesc) = DefaultDesc;
		TextDesc.eTextSubClass	= Type;
		TextDesc.eShaderType	= iter->second.eShaderType;
		TextDesc.wstrFontTag	= Engine_Utils::ToWString(iter->second.strFontTag);
		TextDesc.wstrText		= Engine_Utils::ToWString(iter->second.strText);
		TextDesc.vFontColor		= iter->second.vFontColor;
		TextDesc.ePivot			= iter->second.ePivot;
		TextDesc.fRotate		= iter->second.fRotate;
		TextDesc.fScale			= iter->second.fScale * m_vAspect.x;

		if (isMonsterNameplate)
		{
			CUIMonsterStat_Text::MONSTER_STAT_DESC MonsterStatDesc = {};
			static_cast<CUIText::UI_TEXT_DESC&>(MonsterStatDesc) = TextDesc;
			MonsterStatDesc.iComponentFlag = DTO::EComponentTypeFlag::WORLDUI_COMPONENT;
			wstrProtoTag = L"Prototype_UI_MonsterStatText";
			_wstring wstrPoolTag = L"Prefab_" + Engine_Utils::ToWString(MonsterStatDesc.strName);
			if (FAILED(CUI_Manager::GetInstance()->Regist_Prefab(m_iLevelID, m_ePrefabtype, wstrProtoTag, wstrPoolTag, &MonsterStatDesc)))
			{
				data.strTag;
				return E_FAIL;
			}
		}
		else
		{
			data.strTag;
			return E_FAIL;
		}
	}

	////////////////////////////////////////
	// JUST_IMAGE //
	else if (eClassType == DTO::EUIClassType::JUST_IMAGE)
	{
		CUIJust_Image::JUST_IMAGE_DESC JustImageDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(JustImageDesc) = DefaultDesc;
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, g_wszUILayer, &JustImageDesc);
		JustImageDesc.iComponentFlag = DTO::EComponentTypeFlag::WORLDUI_COMPONENT;			
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
	}

	////////////////////////////////////////
	// DYNAMIC_IMAGE //
	else if (eClassType == DTO::EUIClassType::DYNAMIC_IMAGE)
	{
		auto iter = m_MapDImageDataCache.find(data.strTag);
		if (iter == m_MapDImageDataCache.end())
			return E_FAIL;

		const auto Type = iter->second.eDISubClassType;

		const _bool isPlayerSkill		= (Type >= DTO::EUIDImageSubClassType::PLAYER_SKILL_BEGIN && Type <= DTO::EUIDImageSubClassType::PLAYER_SKILL_END);
		const _bool isMiniMap			= (Type >= DTO::EUIDImageSubClassType::MINIMAP_BEGIN && Type <= DTO::EUIDImageSubClassType::MINIMAP_END);
		const _bool isHoverIcon			= (Type >= DTO::EUIDImageSubClassType::HOVER_POPUP_BEGIN && Type <= DTO::EUIDImageSubClassType::HOVER_POPUP_END);
		const _bool isMenu				= (Type >= DTO::EUIDImageSubClassType::MENU_BEGIN && Type <= DTO::EUIDImageSubClassType::MENU_ICON_BG);
		const _bool isOutLine			= (Type >= DTO::EUIDImageSubClassType::MENU_ICON_OUTLINE && Type <= DTO::EUIDImageSubClassType::MENU_END);
		const _bool isLoading			= (Type >= DTO::EUIDImageSubClassType::LOADING_BEGIN && Type <= DTO::EUIDImageSubClassType::LOADING_END);
		const _bool isMonsterNameplate	= (Type == DTO::EUIDImageSubClassType::MONSTER_NAMEPLATE_BG);

		if (isMonsterNameplate)
		{
			CUINameplate_BG::NAMEPLATE_BG_DESC NameplateDesc = {};
			static_cast<CGenericUI::GENERIC_UI_DESC&>(NameplateDesc) = DefaultDesc;
			NameplateDesc.iComponentFlag = DTO::EComponentTypeFlag::WORLDUI_COMPONENT;
			wstrProtoTag = L"Prototype_UI_Nameplate_BG";
			_wstring wstrPoolTag = L"Prefab_" + Engine_Utils::ToWString(NameplateDesc.strName);
			if (FAILED(CUI_Manager::GetInstance()->Regist_Prefab(m_iLevelID, m_ePrefabtype, wstrProtoTag, wstrPoolTag, &NameplateDesc)))
			{
				data.strTag;
				return E_FAIL;
			}
		}
		else
		{
			data.strTag;
			return E_FAIL;
		}
	}
	////////////////////////////////////////
	// WORLD_UI //
	else if (eClassType == DTO::EUIClassType::WORLD_UI)
	{

	}

	if (pResult == nullptr)
	{
		data.strTag;
		return S_OK;
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

CGenericUI::GENERIC_UI_DESC CBuilder_UIPrefabs::Make_DefaultInfo(const DTO::TUI_GenericUIData& data, CCanvas* pCanvas)
{
	CGenericUI::GENERIC_UI_DESC Desc = {};
	Desc.strName					= data.strTag;
	Desc.iLevelIndex				= m_iLevelID;
	Desc.iRectTransformType			= data.iRectTransformType;
	Desc.fWidth						= data.fWidth * m_vAspect.x;
	Desc.fHeight					= data.fHeight * m_vAspect.x;
	Desc.fX							= data.fPosX * m_vAspect.x;
	Desc.fY							= data.fPosY * m_vAspect.y;
	Desc.fZ							= data.fPosZ;
	Desc.wstrTextureTag				= Engine_Utils::ToWString(data.strTextureTag);
	Desc.wstrNoiseTextureTag		= Engine_Utils::ToWString(data.strNoiseTextureTag);
	Desc.wstrAlphaMaskTextureTag = Engine_Utils::ToWString(data.strAlphaMaskTextureTag);
	Desc.isAlpha = TRUE;
	Desc.isInitVisible = data.isVisible;
	Desc.isInitInteract = data.isInteract;
	Desc.isInitActivate = data.isActivate;
	Desc.isUseColorTint = data.isUseColorTint;
	Desc.pCanvasCache = pCanvas;
	Desc.iComponentFlag = data.iComponentFlag;
	Desc.isUseColorTint = data.isUseColorTint;
	Desc.vColorTint = data.vColorTint;
	Desc.vGradiantColorTint = data.vGradiantColorTint;
	Desc.iShaderPass = data.iShaderPass;
	Desc.fDelay = data.fDelay;
	Desc.iFillDir = data.iFillDir;
	Desc.fAlpha = data.fAlphaRatio;
	Desc.iFlip = data.iFlip;

	return Desc;
}

CBuilder_UIPrefabs* CBuilder_UIPrefabs::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iLevelID)
{
	CBuilder_UIPrefabs* pInstance = new CBuilder_UIPrefabs(pDevice, pDeviceContext, iLevelID);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("CBuilder_UIPrefabs::Create, Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBuilder_UIPrefabs::Free()
{
	Super::Free();
}
