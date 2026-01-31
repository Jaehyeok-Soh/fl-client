#include "pch.h"

#include "ToolCanvas.h"
#include "Tool_Defines.h"
#include "GameInstance.h"
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
	Super::Update_Priority(fTimeDelta);
}

void CToolCanvas::Update(const _float fTimeDelta)
{
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

	SetUp_Rect();

	D3D11_VIEWPORT vp = {};
	UINT numVP = 1;
	m_pDeviceContext->RSGetViewports(&numVP, &vp);

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(DirectX::XMMatrixIdentity());
	m_pEffect->SetView(DirectX::XMMatrixIdentity());
	m_pEffect->SetProjection(DirectX::XMMatrixOrthographicOffCenterLH(
		0.f, vp.Width,
		vp.Height, 0.f,
		0.f, 1.f));

	m_pEffect->Apply(m_pDeviceContext);

	m_pBatch->Begin();

	const DirectX::XMFLOAT4 vColor = { 1.f, 1.f, 1.f, 1.f }; // 원하는 색으로 바꾸세요

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



void CToolCanvas::Calc_HitTest()
{
	vector<CToolUI*> vecUIs;

	for (CToolLayer* pLayer : m_vecToolLayers)
	{
		auto* vecUI = pLayer->Safe_Access_UIObject_Vector_Ptr();
		if (nullptr == vecUI)
			continue;

		for (CToolUI* pUI : *vecUI)
		{
			vecUIs.push_back(pUI);
		}
	}

	vector<CToolUI*> vecTriggerUIs;
	for (CToolUI* pUI : vecUIs)
	{
		if (pUI->Calc_HitEvent())
		{

		}
	}
}

void CToolCanvas::Sync_Data()
{
	m_tCanvasData.strTag = m_strTag;

	m_tCanvasData.fWidth = m_fWidth;
	m_tCanvasData.fHeight = m_fHeight;
	m_tCanvasData.fPosX = m_fX;
	m_tCanvasData.fPosY = m_fY;
	m_tCanvasData.fPosZ = m_fZ;
}

_bool CToolCanvas::Export_Data(DTO::ECategory eCategory, CDataDocumentBase* pDocument)
{
	//if (eCategory != DTO::ECategory::UI || pDocument == nullptr)
	//	return false;

	//if (pDocument->Get_Category() != DTO::ECategory::UI	)
	//	return false;

	//auto* pExampleDocument = static_cast<CDataDocument_UI*>(pDocument);

	//DTO::TExample_StaticModelData saveData;
	//// dto.vPosition = { 1.f, 1.f, 1.f };
	//// dto.vColor = { 1.f, 1.f, 1.f, 1.f };

	//pExampleDocument->Try_Add(saveData);
	return false;
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

