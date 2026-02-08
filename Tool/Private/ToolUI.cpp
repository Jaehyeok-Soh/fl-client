#include "pch.h"
#include "ToolUI.h"
#include "Engine_Utils.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"

#include "ToolCanvas.h"
#include "ImGui_UIManager.h"
#include "ImGui_ToolManager.h"

#include "UIButton_Component.h"
#include "UIProgress_Component.h"
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
	
	m_strName				= pDesc->strName;
	m_strCanvasName			= pDesc->strCanvasName;
	m_iCanvasIndex			= pDesc->iCanvasIndex;
	m_wstrTextureTag		= Engine_Utils::ToWString(pDesc->strInitTextureTag);
	m_eRectTransformType	= static_cast<ERectTransform>(pDesc->iRectTransformType);
	m_pCacheCanvas			= pDesc->pCacheCanvas;
	m_isUseColorTint		= pDesc->isUseColorTint;
	m_vColorTint			= pDesc->vColorTint;
	m_iShaderPass			= pDesc->iShaderPass;
	m_iFillDir				= pDesc->iFillDir;

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

	m_pBatch	= new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect	= new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);
	m_iInteractState = static_cast<uint32_t>(EUIEvent_Flag::NONE);
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
	if (m_isDisable)
		return;

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

	m_isHitTest = FALSE;

    return S_OK;
}

_bool CToolUI::Calc_HitEvent()
{
	if (m_isDisable)
		return FALSE;

	if (::PtInRect(&m_tRenderRect, CImGui_ToolManager::GetInstance()->Get_CalculatedMousePos_Point()))
		return TRUE;

	return FALSE;
}

HRESULT CToolUI::Ready_Components(TOOLUI_DESC* pDesc)
{ 
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Empty", pDesc)))
        return E_FAIL;

    if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
        return E_FAIL;

	if (FAILED(Add_Script_Component(L"UIButton_Component", CUIButton_Component::Create())))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolUI::Bind_ShaderResources()
{
    CShader* pShader = Get_Component<CShader>();

    if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
        return E_FAIL;

    if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
        return E_FAIL;

	pShader->Set_Pass(m_iShaderPass);

	if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::DEFAULT))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;
	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::DEFAULT_ALPHA))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;
	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::COLOR))
	{
		if (FAILED(pShader->Get_Variable("g_vColorTint")->SetRawValue(&m_vColorTint, 0, sizeof(Vec4))))
			return E_FAIL;
	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::FADE))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;

		if (FAILED(pShader->Get_Variable("g_fAlphaRatio")->SetRawValue(&m_fTestAlpha, 0, sizeof(_float))))
			return E_FAIL;
	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::PROGRESS))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;

		if (FAILED(pShader->Get_Variable("g_isColor")->SetRawValue(&m_isUseColorTint, 0, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(pShader->Get_Variable("g_vColorTint")->SetRawValue(&m_vColorTint, 0, sizeof(Vec4))))
			return E_FAIL;

		if (FAILED(pShader->Get_Variable("g_fProgressRatio")->SetRawValue(&m_fTestProgress, 0, sizeof(_float))))
			return E_FAIL;

		if (FAILED(pShader->Get_Variable("g_iFillDir")->SetRawValue(&m_iFillDir, 0, sizeof(uint32_t))))
			return E_FAIL;
	}

    return S_OK;
}

void CToolUI::SetUp_RectTransform_Position()
{
	if (nullptr == m_pCacheCanvas)
		return;

	Vec2 initPos = Calc_RectTransformPosition();

	m_vRenderPos = Vec3{ initPos.x + m_vMoveOffset.x + m_fX, initPos.y + m_vMoveOffset.y + m_fY, m_fZ };
	Move_Position(m_vRenderPos.x , m_vRenderPos.y , m_vRenderPos.z);

	m_tRenderRect.left		= static_cast<LONG>(initPos.x + m_vMoveOffset.x + m_fX - (m_fWidth * 0.5f));
	m_tRenderRect.right		= static_cast<LONG>(initPos.x + m_vMoveOffset.x + m_fX + (m_fWidth * 0.5f));
	m_tRenderRect.top		= static_cast<LONG>(initPos.y + m_vMoveOffset.y + m_fY - (m_fHeight * 0.5f));
	m_tRenderRect.bottom	= static_cast<LONG>(initPos.y + m_vMoveOffset.y + m_fY + (m_fHeight * 0.5f));
}

Vec2 CToolUI::Calc_RectTransformPosition()
{
	switch (m_eRectTransformType)
	{
	case Tool::ERectTransform::LT:return m_pCacheCanvas->Get_LT();
	case Tool::ERectTransform::CT:return m_pCacheCanvas->Get_CT();
	case Tool::ERectTransform::RT:return m_pCacheCanvas->Get_RT();
	case Tool::ERectTransform::LC:return m_pCacheCanvas->Get_LC();
	case Tool::ERectTransform::C: return m_pCacheCanvas->Get_C();
	case Tool::ERectTransform::RC:return m_pCacheCanvas->Get_RC();
	case Tool::ERectTransform::LB:return m_pCacheCanvas->Get_LB();
	case Tool::ERectTransform::CB:return m_pCacheCanvas->Get_CB();
	case Tool::ERectTransform::RB:return m_pCacheCanvas->Get_RB();
	default:return m_pCacheCanvas->Get_C();
	}
}

void CToolUI::SetUp_Visible()
{
}

void CToolUI::Acting_About_State()
{
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::INVOKED))
	{
	}

	if (m_iInteractState == EUIEvent_Flag::NONE)
	{

	}
	else
	{
		if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_ENTER))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESS_EXIT))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVER_ENTER))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVER_EXIT))
		{
		}

		if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::PRESSING))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIEvent_Flag::HOVERING))
		{
		}
	}
}

void CToolUI::Sync_Data()
{
	// UI Object Values
	m_tUIData.fWidth				= m_fWidth;
	m_tUIData.fHeight				= m_fHeight;
	m_tUIData.fPosX					= m_fX;
	m_tUIData.fPosY					= m_fY;
	m_tUIData.fPosZ					= m_fZ;
	m_tUIData.isVisible				= m_isVisible;

	// Tool UI Values
	m_tUIData.strTag				= m_strName;
	m_tUIData.strCanvasName			= m_strCanvasName;
	m_tUIData.iRectTransformType	= static_cast<uint32_t>( m_eRectTransformType);
	m_tUIData.strTextureTag			= Engine_Utils::ToString(m_wstrTextureTag);
	m_tUIData.eClassType			= m_eClassType;
	m_tUIData.iComponentFlag		= m_iComponentFlag;
	m_tUIData.eOwnerType 			= m_eOwnerType;
	m_tUIData.isUseColorTint 		= m_isUseColorTint;
	m_tUIData.vColorTint 			= m_vColorTint;
	m_tUIData.iShaderPass			= m_iShaderPass;
}

HRESULT CToolUI::Request_Change_Texture()
{
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrTextureTag, 0)))
		return E_FAIL;
}

void CToolUI::Request_Chnage_ShaderPass(uint32_t pass)
{
	Get_Component<CShader>()->Set_Pass(pass);
}

CToolUI* CToolUI::Create(EToolObjectType eType, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CToolUI* pInstance = new CToolUI(pDevice, pContext);
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
	Super::Free();
}


