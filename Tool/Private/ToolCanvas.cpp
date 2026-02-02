#include "pch.h"
#include "ToolCanvas.h"
#include "Tool_Defines.h"
#include "ImGui_UIManager.h"
#include "ToolLayer.h"
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
	:CUIObject(rhs)
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
	/* 누른 순간 */
	if (MOUSE_LBUTTON_DOWN)
	{
		m_pCaptureUI = Calc_TopUI();
		if (nullptr != m_pCaptureUI)
			Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESS_ENTER);
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
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESS_ENTER);
					m_isPreUIPressing = TRUE;
				}
				else
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESSING);
				}
			}
			else
			{
				if (m_isPreUIPressing)
				{
					Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::PRESS_EXIT);
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
			if (m_pCaptureUI->Calc_HitEvent())
			{
				Engine_Utils::Add_Flag(m_pCaptureUI->Get_InteractState_Ref(), CUIObject::EInteractState::CLICKED);
			}
		}
		m_pCaptureUI = nullptr;
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
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_EXIT);
				m_pHoveringUI = nullptr;
			}
		}
		/* 마우스랑 겹치는 UI가 있다 */
		else
		{
			if (nullptr == m_pHoveringUI)
			{
				m_pHoveringUI = pUI;
				Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_ENTER);
			}
			else
			{
				if (m_pHoveringUI != pUI)
				{
					Engine_Utils::Add_Flag(m_pHoveringUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_EXIT);
					Engine_Utils::Add_Flag(pUI->Get_InteractState_Ref(), CUIObject::EInteractState::HOVERING_ENTER);
					m_pHoveringUI = pUI;
				}
			}
		}
	}
}

CToolUI* CToolCanvas::Calc_TopUI()
{
	CToolUI* pTopUI = { nullptr };

	for (CToolLayer* pLayer : m_vecToolLayers)
	{
		if (!pLayer->Get_isVisible())
			continue;

		auto* vecUI = pLayer->Safe_Access_UIObject_Vector_Ptr();
		if (nullptr == vecUI)
			continue;

		for (CToolUI* pUI : *vecUI)
		{
			/* UI안에 마우스가 있으면 TRUE */
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
	}
	return pTopUI;
}

void CToolCanvas::Sync_Data()
{
	m_tCanvasData.strTag = m_strTag;

	m_tCanvasData.iLevelIndex = m_iClientLevelIndex;
	m_tCanvasData.fWidth = m_fWidth;
	m_tCanvasData.fHeight = m_fHeight;
	m_tCanvasData.fPosX = m_fX;
	m_tCanvasData.fPosY = m_fY;
	m_tCanvasData.fPosZ = m_fZ;
	m_tCanvasData.iEditorSizeX = g_iWinSizeX;
	m_tCanvasData.iEditorSizeY = g_iWinSizeY;
}

HRESULT CToolCanvas::Safe_Add_Layer(CToolLayer* pLayer)
{
	if (nullptr == pLayer)
		return E_FAIL;

	m_vecToolLayers.push_back(pLayer);
	return S_OK;
}

vector<CToolLayer*>* CToolCanvas::Safe_Access_LayerObject_Vector_Ptr()
{
	if (m_vecToolLayers.empty())
		return nullptr;
	return &m_vecToolLayers;
}

CToolLayer* CToolCanvas::Safe_Access_LayerObject_Ptr(int32_t index)
{
	if (m_vecToolLayers.empty())
		return nullptr;

	int32_t NumLayer = static_cast<int32_t>(m_vecToolLayers.size());
	if (index >= NumLayer || index < 0)
		return nullptr;

	return m_vecToolLayers[index];
}

CToolLayer* CToolCanvas::Safe_Access_CurLayerObject_Ptr()
{
	if (m_vecToolLayers.empty())
		return nullptr;

	int32_t NumLayer = static_cast<int32_t>(m_vecToolLayers.size());
	if (m_pUIManager->Get_CurCanvasIndex() >= NumLayer || m_pUIManager->Get_CurCanvasIndex() < 0)
		return nullptr;

	return m_vecToolLayers[m_pUIManager->Get_CurCanvasIndex()];
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

	/* Canvas에 저장된 Layer 저장 */
	if (m_vecToolLayers.empty())
		return FALSE;

	for (auto* pLayer : m_vecToolLayers)
	{
		if (FAILED(pDoc->Try_Add(pLayer->Get_Data())))
			return FALSE;


		/* Layer에 저장된 UI 저장 */
		auto* pUIVec = pLayer->Safe_Access_UIObject_Vector_Ptr();
		if (nullptr == pUIVec)
			continue;
		for (auto* pUI : *pUIVec)
		{
			if (FAILED(pDoc->Try_Add(pUI->Get_Data())))
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
	for (auto* p : m_vecToolLayers)
	{
		if (nullptr == p)
			continue;
		Safe_Release(p);
	}
	m_vecToolLayers.clear();
	Super::Free();
}

