#include "pch.h"
#include "Builder_UI.h"

#include "Canvas.h"
#include "GenericUI.h"
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

	CGenericUI::GENERIC_UI_DESC Desc = {};
	Desc.iRectTransformType		= data.iRectTransformType;
	Desc.fWidth					= data.fWidth * m_vAspect.x;
	Desc.fHeight				= data.fHeight * m_vAspect.y;
	Desc.fX						= data.fPosX * m_vAspect.x;
	Desc.fY						= data.fPosY * m_vAspect.y;
	Desc.fZ						= data.fPosZ;
	Desc.wstrTextureTag			= L"Prototype_Component_UI_Texture";
	Desc.iTextureIndex			= data.iTextureIndex;
	Desc.isAlpha				= TRUE;

	auto iter = m_MapCanvasCache.find(data.strCanvasName);
	if (iter == m_MapCanvasCache.end())
		return E_FAIL;

	const _wstring wstrLayerTag = Engine_Utils::ToWString(iter->second->Get_Name()) + L"_Layer";
	CGameObject* pResult = m_pGameInstance->Add_GameObject(m_iLevelID, L"Prototype_UI_GenericUI", m_iLevelID, wstrLayerTag, &Desc);
	if (pResult == nullptr)
		return E_FAIL;
	
	auto* pUI = dynamic_cast<CGenericUI*>(pResult);
	if (nullptr == pUI)
		return E_FAIL;

	iter->second->Get_UIVector()->push_back(pUI);
	m_pMapUICache.emplace(data.strTag, pUI);

	if (FAILED(CUI_Manager::GetInstance()->Add_VecGenericUICache(m_iLevelID, pUI)))
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
