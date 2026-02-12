#include "pch.h"
#include "ToolCanvas.h"
#include "ImGui_UIManager.h"
#include "ToolUI.h"
#include "Engine_Utils.h"
/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"
#include "DataDocument_UI.h"
#include "DebugDraw.h"
#include "GameInstance.h"

#define UIDATAFILE_PATH L"../../Resources/Data/UIData/"

CToolCanvas::CToolCanvas(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext),
	m_pUIManager(CImGui_UIManager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

CToolCanvas::CToolCanvas(const CToolCanvas& rhs)
	: CUIObject(rhs)
{
}

HRESULT CToolCanvas::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolCanvas::Initialize(void* pArg)
{
	m_pUIManager = { nullptr };
	m_tCanvasData = {};
	m_pCaptureUI = {nullptr};
	m_pHoveringUI = { nullptr };
	m_ArrReleasedUI = {nullptr};


	TOOLCANVAS_DESC* pDesc = static_cast<TOOLCANVAS_DESC*>(pArg);
	m_strTag = pDesc->strTag;
	m_iClientLevelIndex = pDesc->iClientLevelIndex;
	m_tCanvasData.iEditorSizeX = pDesc->iEditorSizeX;
	m_tCanvasData.iEditorSizeY = pDesc->iEditorSizeY;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolCanvas::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderInput = { nullptr };
	size_t iShaderInputLenght = {};
	m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLenght);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderInput, iShaderInputLenght, &m_pInputLayout)))
		return E_FAIL;
	return S_OK;
}

void CToolCanvas::Update_Priority(const _float fTimeDelta)
{
	Set_Size(m_fWidth, m_fHeight);
	Move_Position(m_fX, m_fY, m_fZ);
	Super::Update_Priority(fTimeDelta);
}

void CToolCanvas::Update(const _float fTimeDelta)
{
	Calc_HitUpdate();
	Super::Update(fTimeDelta);
}

void CToolCanvas::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CToolCanvas::Ready_Before_Render(const _float fTimeDelta)
{
	Sync_Data();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CToolCanvas::Render()
{
	if (nullptr == m_pBatch || nullptr == m_pEffect || nullptr == m_pInputLayout)
		return E_FAIL;

	D3D11_VIEWPORT vp = {};
	UINT numVP = 1;
	m_pDeviceContext->RSGetViewports(&numVP, &vp);
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->SetWorld(DirectX::XMMatrixIdentity());
	m_pEffect->SetView(DirectX::XMMatrixIdentity());
	m_pEffect->SetProjection(DirectX::XMMatrixOrthographicOffCenterLH(0.f, vp.Width,vp.Height, 0.f,	0.f, 1.f));
	m_pEffect->Apply(m_pDeviceContext);
	m_pBatch->Begin();
	const DirectX::XMFLOAT4 vColor = { 1.f, 1.f, 1.f, 1.f };

	// Top
	m_pBatch->DrawLine(
		VertexPositionColor{ { (float)m_tRect.left,  (float)m_tRect.top,    m_fZ }, vColor },
		VertexPositionColor{ { (float)m_tRect.right, (float)m_tRect.top,    m_fZ }, vColor }
	);

	// Right
	m_pBatch->DrawLine(
		VertexPositionColor{ { (float)m_tRect.right, (float)m_tRect.top,    m_fZ }, vColor },
		VertexPositionColor{ { (float)m_tRect.right, (float)m_tRect.bottom, m_fZ }, vColor }
	);

	// Bottom
	m_pBatch->DrawLine(
		VertexPositionColor{ { (float)m_tRect.right, (float)m_tRect.bottom, m_fZ }, vColor },
		VertexPositionColor{ { (float)m_tRect.left,  (float)m_tRect.bottom, m_fZ }, vColor }
	);

	// Left
	m_pBatch->DrawLine(
		VertexPositionColor{ { (float)m_tRect.left,  (float)m_tRect.bottom, m_fZ }, vColor },
		VertexPositionColor{ { (float)m_tRect.left,  (float)m_tRect.top,    m_fZ }, vColor }
	);
	m_pBatch->End();
	return S_OK;
}


HRESULT CToolCanvas::Ready_Components(TOOLCANVAS_DESC* pDesc)
{
	return S_OK;
}

HRESULT CToolCanvas::Bind_ShaderResources()
{
	return S_OK;
}

void CToolCanvas::Calc_HitUpdate()
{
	if (!m_ArrReleasedUI.empty())
	{
		for (auto*& pUI : m_ArrReleasedUI)
		{
			if (nullptr != pUI)
			{
				pUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
				pUI = nullptr;
			}
		}
	}

	/* Trigger 이벤트 소비 */
	if (nullptr != m_pCaptureUI)
	{
		Engine_Utils::RemoveSoft_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_ENTER);
		Engine_Utils::RemoveSoft_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_EXIT);
		m_pCaptureUI->Get_InteractState_Ref() =  EUIEvent_Flag::NONE;
	}
	if (nullptr != m_pHoveringUI)
	{
		Engine_Utils::RemoveSoft_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_ENTER);
		Engine_Utils::RemoveSoft_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_EXIT);
		m_pHoveringUI->Get_InteractState_Ref() =  EUIEvent_Flag::NONE;
	}

	/* 누른 순간 */
	if (MOUSE_LBUTTON_DOWN)
	{
		/* 눌린곳에 있는 UI중 가장 위에 있는 애 */
		m_pCaptureUI = Calc_TopUI();
		if (nullptr != m_pCaptureUI)
		{
			Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_ENTER);
			m_isPreUIPressing = TRUE;
		}
	}
	/* 누르고 있을 때 */
	else if (MOUSE_LBUTTON_HOLD)
	{
		if (nullptr != m_pCaptureUI)
		{
			if (m_pCaptureUI->Calc_HitEvent())
			{
				if (!m_isPreUIPressing)
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_ENTER);
					m_isPreUIPressing = TRUE;
				}
				else
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESSING);
				}
			}
			else
			{
				if (m_isPreUIPressing)
				{
					m_pCaptureUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
					m_isPreUIPressing = FALSE;
				}
			}
		}
	}
	/* 땐 순간 */
	else if (MOUSE_LBUTTON_UP)
	{
		if (nullptr != m_pCaptureUI)
		{
			/* 땠을 때 동일한 UI면 */
			if (m_pCaptureUI->Calc_HitEvent())
			{
				Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), EUIEvent_Flag::PRESS_EXIT);
				const uint32_t CaptureUI = 0u;
				m_ArrReleasedUI[CaptureUI] = m_pCaptureUI;
				m_pCaptureUI = nullptr;
			}
			else
			{
				m_pCaptureUI->Get_InteractState_Ref() = EUIEvent_Flag::NONE;
				m_pCaptureUI = nullptr;
			}
			m_isPreUIPressing = FALSE;
		}
	}
	/* 안 누르고 있을 때*/
	else
	{
		CToolUI* pUI = Calc_TopUI();
		/* 마우스랑 겹치는 UI가 없다 */
		if (nullptr == pUI)
		{
			/* 호버링중인 UI가 있다 */
			if (nullptr != m_pHoveringUI)
			{
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_EXIT);
				const uint32_t HoverUI = 1u;
				m_ArrReleasedUI[HoverUI] = m_pHoveringUI;
				m_pHoveringUI = nullptr;
			}
		}
		/* 마우스랑 겹치는 UI가 있다 */
		else
		{
			if (nullptr == m_pHoveringUI)
			{
				m_pHoveringUI = pUI;
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_ENTER);
			}
			else
			{
				if (m_pHoveringUI != pUI)
				{
					Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_EXIT);
					Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVER_ENTER);
					const uint32_t HoverUI = 1u;
					m_ArrReleasedUI[HoverUI] = m_pHoveringUI;
					m_pHoveringUI = pUI;
				}
				else
				{
					Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), EUIEvent_Flag::HOVERING);
				}
			}
		}
	}
}

CToolUI* CToolCanvas::Calc_TopUI()
{
	CToolUI* pTopUI = { nullptr };

	for (CToolUI* pUI : m_vecToolUI)
	{
		if (pUI->Calc_HitEvent())
		{
			if (nullptr == pTopUI)
				pTopUI = pUI;
			else
			{
				if (pTopUI->Get_PosZ() < pUI->Get_PosZ())
					pTopUI = pUI;
			}
		}
	}
	return pTopUI;
}

HRESULT CToolCanvas::Safe_Add_UI(CToolUI* pUI)
{
	if (nullptr == pUI)
		return E_FAIL;
	m_vecToolUI.push_back(pUI);
	return S_OK;
}

vector<CToolUI*>* CToolCanvas::Safe_Access_UI_Vector()
{
	if (m_vecToolUI.empty())
		return nullptr;

	return &m_vecToolUI;
}

void CToolCanvas::Sync_Data()
{
	m_tCanvasData.strTag = m_strTag;

	m_tCanvasData.iLevelIndex	= m_iClientLevelIndex;
	m_tCanvasData.fWidth		= m_fWidth;
	m_tCanvasData.fHeight		= m_fHeight;
	m_tCanvasData.fPosX			= m_fX;
	m_tCanvasData.fPosY			= m_fY;
	m_tCanvasData.fPosZ			= m_fZ;
	m_tCanvasData.iEditorSizeX	= g_iWinSizeX;
	m_tCanvasData.iEditorSizeY	= g_iWinSizeY;
}

_bool CToolCanvas::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	ELevelType eLevelType = ELevelType::UI;
	_uint iLevelID = ENUM_TO_UINT(eLevelType);
	_wstring wstrFilePath = UIDATAFILE_PATH + Engine_Utils::ToWString(m_strTag) + L".json";
	CDataDocumentBase* pDocBase = m_pGameInstance->Ensure_Document(iLevelID, eCategory, wstrFilePath);
	if (pDocBase == nullptr)
		return FALSE;

	CDataDocument_UI* pDoc = static_cast<CDataDocument_UI*>(pDocBase);
	if (FAILED(pDoc->Try_Add(m_tCanvasData)))
		return FALSE;

	if (m_vecToolUI.empty())
		return FALSE;

	for (auto* pUI : m_vecToolUI)
	{
		if (FAILED(pDoc->Try_Add(pUI->Get_Data())))
			return FALSE;

		if (pUI->Get_UIClassType() == DTO::EUIClassType::UI_TEXT)
		{
			if (FAILED(pDoc->Try_Add(pUI->Get_TextData())))
				return FALSE;
		}
		else if (pUI->Get_UIClassType() == DTO::EUIClassType::TRIGGER)
		{
			if (FAILED(pDoc->Try_Add(pUI->Get_TriggerData())))
				return FALSE;
		}
		else if (pUI->Get_UIClassType() == DTO::EUIClassType::BUTTON_TRIGGER)
		{
			if (FAILED(pDoc->Try_Add(pUI->Get_ButtonTriggerData())))
				return FALSE;
		}
		else if (pUI->Get_UIClassType() == DTO::EUIClassType::DYNAMIC_IMAGE)
		{
			if (FAILED(pDoc->Try_Add(pUI->Get_DImageData())))
				return FALSE;
		}
	}
	return TRUE;
}

CToolCanvas* CToolCanvas::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CToolCanvas* pInstance = new CToolCanvas(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CToolCanvas::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CToolCanvas::Clone(void* pArg)
{
	CToolCanvas* pInstance = new CToolCanvas(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CToolCanvas::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolCanvas::Free()
{
	Safe_Release(m_pUIManager);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);
	Super::Free();
}

