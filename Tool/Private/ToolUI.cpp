#include "pch.h"
#include "ToolUI.h"

#include "Engine_Utils.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"
#include "GameInstance.h"

#include "ToolCanvas.h"
#include "ToolLayer.h"
#include "ImGui_UIManager.h"

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
	m_iLayerIndex = pDesc->iLayerIndex;
	m_wstrTextureTag = Engine_Utils::ToWString(pDesc->strInitTextureTag);
	m_iTextureIndex = pDesc->iInitTextureIndex;

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

    return S_OK;
}

void CToolUI::Update_Priority(const _float fTimeDelta)
{
	Set_Size(m_fWidth, m_fHeight);
	SetUp_RectTransform_Position();
	SetUp_Visible();
	Super::Update_Priority(fTimeDelta);
}

void CToolUI::Update(const _float fTimeDelta)
{
	if(m_isVisible)
		Super::Update(fTimeDelta);
}

void CToolUI::Update_Late(const _float fTimeDelta)
{
	if (m_isVisible)
		Super::Update_Late(fTimeDelta);
}

void CToolUI::Ready_Before_Render(const _float fTimeDelta)
{

	if (m_isVisible)
	{
		Sync_Data();
		Super::Ready_Before_Render(fTimeDelta);

	}
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
	auto* pCanvas = CImGui_UIManager::GetInstance()->Safe_Access_Canvas(CImGui_UIManager::GetInstance()->Get_CurCanvasIndex());
	if (nullptr == pCanvas)
		return;

	Vec2 initPos = {};
	switch (m_eRectTransformType)
	{
	case Tool::ERectTransform::LT:initPos = pCanvas->Get_LT();break;
	case Tool::ERectTransform::CT:initPos = pCanvas->Get_CT();break;
	case Tool::ERectTransform::RT:initPos = pCanvas->Get_RT();break;
	case Tool::ERectTransform::LC:initPos = pCanvas->Get_LC();break;
	case Tool::ERectTransform::C:initPos = pCanvas->Get_C();break;
	case Tool::ERectTransform::RC:initPos = pCanvas->Get_RC();break;
	case Tool::ERectTransform::LB:initPos = pCanvas->Get_LB();break;
	case Tool::ERectTransform::CB:initPos = pCanvas->Get_CB();break;
	case Tool::ERectTransform::RB:initPos = pCanvas->Get_RB();break;
	default:initPos = pCanvas->Get_C();break;
	}

	Move_Position(initPos.x + m_fX, initPos.y + m_fY, m_fZ);
}

void CToolUI::SetUp_Visible()
{
	CToolLayer* pLayer = CImGui_UIManager::GetInstance()->Safe_Access_Layer(m_iLayerIndex);
	if (nullptr == pLayer)
		return;

	m_isVisible = pLayer->Get_isVisible();
}

void CToolUI::Sync_Data()
{
	m_tUIData.strTag = m_strName;
	m_tUIData.iLayerIndex = m_iLayerIndex;
	m_tUIData.iRectTransformType = static_cast<uint32_t>( m_eRectTransformType);
	m_tUIData.iUIType = static_cast<uint32_t>(m_eUIType);
	m_tUIData.fWidth = m_fWidth;
	m_tUIData.fHeight = m_fHeight;
	m_tUIData.fPosX = m_fX;
	m_tUIData.fPosY = m_fY;
	m_tUIData.fPosZ = m_fZ;
	m_tUIData.strTextureTag = Engine_Utils::ToString(m_wstrTextureTag);
	m_tUIData.iTextureIndex = m_iTextureIndex;
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
	Super::Free();
}

