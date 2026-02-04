#include "pch.h"
#include "ToolUI.h"
#include "Engine_Utils.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"

#include "ToolCanvas.h"
#include "ToolLayer.h"
#include "ImGui_UIManager.h"
#include "ImGui_ToolManager.h"
#include "UIAction_Tool.h"

#include "UIAction_Registry.h"
#include "GameInstance.h"
CToolUI::CToolUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext)
{
}

CToolUI::CToolUI(const CToolUI& rhs)
	:CUIObject(rhs)
{
}

HRESULT CToolUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CToolUI::Initialize(void* pArg)
{
	TOOLUI_DESC* pDesc = static_cast<TOOLUI_DESC*>(pArg);
	m_strName = pDesc->strName;

	m_strCanvasName = pDesc->strCanvasName;
	m_iCanvasIndex = pDesc->iCanvasIndex;
	m_strLayerName = pDesc->strLayerName;
	m_iLayerIndex = pDesc->iLayerIndex;

	m_wstrTextureTag = Engine_Utils::ToWString(pDesc->strInitTextureTag);
	m_iTextureIndex = pDesc->iInitTextureIndex;

	m_eRectTransformType = static_cast<ERectTransform>(pDesc->iRectTransformType);

	m_fHeight = pDesc->fHeight;
	m_fWidth = pDesc->fWidth;
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fZ = pDesc->fZ;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CToolUI::Awake(const _uint iCurrentLevelID)
{   
	if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	m_pActionForMe = CUIAction_Tool::Create(this);
	m_iInteractState = static_cast<uint32_t>(DTO::EUIEvent_Flag::NONE);
	if (nullptr == m_pActionForMe)
		return E_FAIL;

    return S_OK;
}

void CToolUI::Update_Priority(const _float fTimeDelta)
{
	Set_Size(m_fWidth, m_fHeight);
	SetUp_RectTransform_Position();
	SetUp_Visible();
	m_isHitTest = FALSE;
	Super::Update_Priority(fTimeDelta);
}

void CToolUI::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CToolUI::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CToolUI::Ready_Before_Render(const _float fTimeDelta)
{
	Acting_About_State();
	Sync_Data();
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CToolUI::Render()
{
	if (!m_isVisible)
		return S_OK;

	if (FAILED(Super::Render()))
		return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    Get_Component<CShader>()->Apply();
    Get_Component<CVIBuffer>()->Bind_Resource();
    Get_Component<CVIBuffer>()->Render();

	if (m_isHitTest)
	{
		D3D11_VIEWPORT vp = {};
		UINT numVP = 1;
		m_pDeviceContext->RSGetViewports(&numVP, &vp);
		m_pDeviceContext->IASetInputLayout(m_pInputLayout);
		m_pEffect->SetWorld(DirectX::XMMatrixIdentity());
		m_pEffect->SetView(DirectX::XMMatrixIdentity());
		m_pEffect->SetProjection(DirectX::XMMatrixOrthographicOffCenterLH(0.f, vp.Width, vp.Height, 0.f, 0.f, 1.f));
		m_pEffect->Apply(m_pDeviceContext);
		m_pBatch->Begin();
		const DirectX::XMFLOAT4 vColor = { 1.f, 0.f, 0.f, 1.f };

		// Top
		m_pBatch->DrawLine(
			VertexPositionColor{ { (float)m_tRenderRect.left-5.f,  (float)m_tRenderRect.top - 5.f,    m_fZ }, vColor },
			VertexPositionColor{ { (float)m_tRenderRect.right+5.f, (float)m_tRenderRect.top - 5.f,    m_fZ }, vColor });

		// Right
		m_pBatch->DrawLine(
			VertexPositionColor{ { (float)m_tRenderRect.right+5.f, (float)m_tRenderRect.top-5.f,    m_fZ }, vColor },
			VertexPositionColor{ { (float)m_tRenderRect.right+5.f, (float)m_tRenderRect.bottom+5.f, m_fZ }, vColor });

		// Bottom
		m_pBatch->DrawLine(
			VertexPositionColor{ { (float)m_tRenderRect.right+5.f, (float)m_tRenderRect.bottom+5.f, m_fZ }, vColor },
			VertexPositionColor{ { (float)m_tRenderRect.left-5.f,  (float)m_tRenderRect.bottom+5.f, m_fZ }, vColor });

		// Left
		m_pBatch->DrawLine(
			VertexPositionColor{ { (float)m_tRenderRect.left-5.f,  (float)m_tRenderRect.bottom+5.f, m_fZ }, vColor },
			VertexPositionColor{ { (float)m_tRenderRect.left-5.f,  (float)m_tRenderRect.top-5.f,    m_fZ }, vColor });
		m_pBatch->End();
	}

    return S_OK;
}

_bool CToolUI::Calc_HitEvent()
{
	if (::PtInRect(&m_tRenderRect, CImGui_ToolManager::GetInstance()->Get_CalculatedMousePos_Point()))
		return TRUE;
	return FALSE;
}

HRESULT CToolUI::Bind_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType, const json& params)
{
	const size_t index = ENUM_TO_SZET(EventType);
	if (index >= m_vecBindingActions.size())
		return E_FAIL;

	DTO::TUI_EventBindData Desc = {};
	Desc.strOwnerTag = m_strName;
	Desc.strActionKey = DTO::UIFunctypeToString(ActType);
	Desc.eEvent = EventType;
	Desc.Params = params;
	m_vecBindingActionData[index].push_back(Desc);
	auto Func = m_pGameInstance->Get_UIAction_Registry()->Build_Action(ActType, params);
	if (!Func)
		return E_FAIL;
	m_vecBindingActions[index].push_back(std::move(Func));
	return S_OK;
}

HRESULT CToolUI::ReBind_Action()
{
	for (uint32_t i = 0; i < m_vecBindingActionData.size(); ++i)
	{
		m_vecBindingActions[i].clear();
		for (auto& data : m_vecBindingActionData[i])
		{
			auto Func = m_pGameInstance->Get_UIAction_Registry()->Build_Action(DTO::StringToUIFunctype( data.strActionKey),data.Params);
			if (!Func)
				return E_FAIL;
			m_vecBindingActions[i].push_back(std::move(Func));
		}
	}
	return S_OK;
}

HRESULT CToolUI::Remove_Action(DTO::EUIEvent EventType, DTO::EUIAction ActType)
{
	const size_t EventIndex = ENUM_TO_SZET(EventType);
	if (EventIndex >= m_vecBindingActionData.size())
		return E_FAIL;

	_bool isRemoved = { FALSE };
	for (auto iter = m_vecBindingActionData[EventIndex].begin(); iter != m_vecBindingActionData[EventIndex].end(); iter++)
	{
		if (DTO::StringToUIFunctype(iter->strActionKey) == ActType)
		{
			m_vecBindingActionData[EventIndex].erase(iter);
			isRemoved = TRUE;
			break;
		}
	}
	if (!isRemoved)
	{
		MSG_BOX("CToolUI::Remove_Action, No Action with Match strActionKey");
		return E_FAIL;
	}
	m_vecBindingActions[EventIndex].clear();
		if (FAILED(ReBind_Action()))
			return E_FAIL;
	return S_OK;
}

HRESULT CToolUI::Ready_Components(TOOLUI_DESC* pDesc)
{ 
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::UI), m_wstrTextureTag, pDesc)))
        return E_FAIL;

    if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
        return E_FAIL;

    if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
        return E_FAIL;

	return S_OK;
}

HRESULT CToolUI::Excute_Action(DTO::EUIEvent EventType)
{
	if (nullptr == m_pActionForMe)
		return E_FAIL;

	size_t index = ENUM_TO_SZET(EventType);
	if (index >= m_vecBindingActions.size())
		return E_FAIL;

	for (auto& fn : m_vecBindingActions[index])
		fn(m_pActionForMe);
	return S_OK;
}

HRESULT CToolUI::Bind_ShaderResources()
{
    CShader* pShader = Get_Component<CShader>();

    if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
        return E_FAIL;

    if (FAILED(Get_Component<CTexture>()->Bind_ShaderResource(pShader, m_iTextureIndex)))
        return E_FAIL;

    return S_OK;
}

void CToolUI::SetUp_RectTransform_Position()
{
	auto* pCanvas = CImGui_UIManager::GetInstance()->Safe_Access_Canvas(m_iCanvasIndex);
	if (nullptr == pCanvas)
		return;

	Vec2 initPos = {};
	switch (m_eRectTransformType)
	{
	case Tool::ERectTransform::LT:initPos = pCanvas->Get_LT();break;
	case Tool::ERectTransform::CT:initPos = pCanvas->Get_CT();break;
	case Tool::ERectTransform::RT:initPos = pCanvas->Get_RT();break;
	case Tool::ERectTransform::LC:initPos = pCanvas->Get_LC();break;
	case Tool::ERectTransform::C: initPos = pCanvas->Get_C();break;
	case Tool::ERectTransform::RC:initPos = pCanvas->Get_RC();break;
	case Tool::ERectTransform::LB:initPos = pCanvas->Get_LB();break;
	case Tool::ERectTransform::CB:initPos = pCanvas->Get_CB();break;
	case Tool::ERectTransform::RB:initPos = pCanvas->Get_RB();break;
	default:initPos = pCanvas->Get_C();break;
	}

	Move_Position(initPos.x + m_fX, initPos.y + m_fY, m_fZ);
	m_vRenderPos = Vec3{ initPos.x + m_fX, initPos.y + m_fY, m_fZ };

	m_tRenderRect.left		= static_cast<LONG>(initPos.x + m_fX - (m_fWidth * 0.5f));
	m_tRenderRect.right		= static_cast<LONG>(initPos.x + m_fX + (m_fWidth * 0.5f));
	m_tRenderRect.top		= static_cast<LONG>(initPos.y + m_fY - (m_fHeight * 0.5f));
	m_tRenderRect.bottom	= static_cast<LONG>(initPos.y + m_fY + (m_fHeight * 0.5f));
}

void CToolUI::SetUp_Visible()
{
	CToolLayer* pLayer = CImGui_UIManager::GetInstance()->Safe_Access_Layer(m_iLayerIndex);
	if (nullptr == pLayer)
		return;

	if(!m_isVisible)
	m_isVisible = pLayer->Get_isVisible();
}

void CToolUI::Acting_About_State()
{
	if(m_iInteractState == DTO::EUIEvent_Flag::NONE)
		Excute_Action(DTO::EUIEvent::NONE);
	else
	{
		if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESS_ENTER))
		{
			Excute_Action(DTO::EUIEvent::PRESS_ENTER);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESS_EXIT))
		{
			Excute_Action(DTO::EUIEvent::PRESS_EXIT);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVER_ENTER))
		{
			Excute_Action(DTO::EUIEvent::HOVER_ENTER);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVER_EXIT))
		{
			Excute_Action(DTO::EUIEvent::HOVER_EXIT);
		}

		if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::PRESSING))
		{
			Excute_Action(DTO::EUIEvent::PRESSING);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, DTO::EUIEvent_Flag::HOVERING))
		{
			Excute_Action(DTO::EUIEvent::HOVERING);
		}
	}
}

void CToolUI::Sync_Data()
{
	m_tUIData.strTag = m_strName;
	m_tUIData.strCanvasName = m_strCanvasName;
	m_tUIData.strLayerName = m_strLayerName;
	m_tUIData.iRectTransformType = static_cast<uint32_t>( m_eRectTransformType);
	m_tUIData.fWidth = m_fWidth;
	m_tUIData.fHeight = m_fHeight;
	m_tUIData.fPosX = m_fX;
	m_tUIData.fPosY = m_fY;
	m_tUIData.fPosZ = m_fZ;
	m_tUIData.strTextureTag = Engine_Utils::ToString(m_wstrTextureTag);
	m_tUIData.iTextureIndex = m_iTextureIndex;
}

vector<DTO::TUI_EventBindData>* CToolUI::Safe_Access_EventData(DTO::EUIEvent EventType)
{
	size_t index = ENUM_TO_SZET(EventType);

	if (index >= m_vecBindingActionData.size() || m_vecBindingActionData.empty())
		return nullptr;

	return &m_vecBindingActionData[index];
}

array<vector<DTO::TUI_EventBindData>, ENUM_TO_UINT(DTO::EUIEvent::END)>* CToolUI::Safe_Access_AllEventData()
{
	return &m_vecBindingActionData;
}

CToolUI* CToolUI::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CToolUI* pInstance = new CToolUI(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CToolUI::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CToolUI::Clone(void* pArg)
{
	CToolUI* pInstance = new CToolUI(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CToolUI::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolUI::Free()
{
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);
	Safe_Release(m_pActionForMe);
	Super::Free();
}

