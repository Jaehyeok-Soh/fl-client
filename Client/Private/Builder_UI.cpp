#include "pch.h"
#include "Builder_UI.h"
#include "Canvas.h"
#include "GenericUI.h"
#include "UIPlayer_HP.h"
#include "UIMonster_HP.h"
#include "UI_Manager.h"
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

	return S_OK;
}

HRESULT CBuilder_UI::Create_CanvasDTO(const DTO::TUI_CanvasData& data)
{
	if (data.eType != DTO::EUIType::CANVAS)
		return E_FAIL;

	CCanvas::CANVAS_DESC Desc	= {};
	Desc.iLevelIndex			= data.iLevelIndex;
	Desc.strName				= data.strTag;
	m_vAspect.x					= (_float)g_iWinSizeX / (_float)data.iEditorSizeX;
	m_vAspect.y					= (_float)g_iWinSizeY / (_float)data.iEditorSizeY;
	Desc.fX						= data.fPosX * m_vAspect.x;
	Desc.fY						= data.fPosY * m_vAspect.y;
	Desc.fZ						= data.fPosZ;
	Desc.fWidth					= m_vViewportSIze.x;
	Desc.fHeight				= m_vViewportSIze.y;

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

HRESULT CBuilder_UI::Register_Class(DTO::EUIClassType eClassType, const DTO::TUI_GenericUIData& data, CCanvas* pCanvas)
{
	if (nullptr == pCanvas)
		return E_FAIL;

	CGenericUI::GENERIC_UI_DESC DefaultDesc = Make_DefaultInfo(data, pCanvas);
	const _wstring wstrLayerTag = Engine_Utils::ToWString(pCanvas->Get_Name()) + L"_Layer";
	const _wstring wstrProtoTag = L"Prototype_UI_" + Engine_Utils::ToWString(DTO::UIClassTypeToString(eClassType));

	CGameObject* pResult = nullptr;

	if (eClassType == DTO::EUIClassType::PLAYER_HP)
	{
		CUIPlayer_HP::PLAYER_HP_DESC PlayerHPDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(PlayerHPDesc) = DefaultDesc;
		/* 플레이어 스탯 컴포넌트 */
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &PlayerHPDesc);
	}
	else if (eClassType == DTO::EUIClassType::MONSTER_HP)
	{
		CUIMonster_HP::MONSTER_HP_DESC MonsterDesc = {};
		static_cast<CGenericUI::GENERIC_UI_DESC&>(MonsterDesc) = DefaultDesc;
		/* 몬스터 스탯 컴포넌트 */
		pResult = m_pGameInstance->Add_GameObject(m_iLevelID, wstrProtoTag, m_iLevelID, wstrLayerTag, &MonsterDesc);
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
	Desc.iRectTransformType = data.iRectTransformType;
	Desc.fWidth				= data.fWidth * m_vAspect.x;
	Desc.fHeight			= data.fHeight * m_vAspect.y;
	Desc.fX					= data.fPosX * m_vAspect.x;
	Desc.fY					= data.fPosY * m_vAspect.y;
	Desc.fZ					= data.fPosZ;
	Desc.wstrTextureTag		= Engine_Utils::ToWString(data.strTextureTag);
	Desc.isAlpha			= TRUE;
	Desc.isInitVisible		= data.isVisible;
	Desc.pCanvasCache		= pCanvas;
	Desc.iComponentFlag		= data.iComponentFlag;
	Desc.isUseColorTint		= data.isUseColorTint;
	Desc.vColorTint			= data.vColorTint;
	Desc.iShaderPass		= data.iShaderPass;

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
