#include "pch.h"
#include "ToolUI.h"

/* Components */
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "Texture.h"

#include "ToolCanvas.h"
#include "ImGui_UIManager.h"
#include "ImGui_ToolManager.h"
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
	m_eClassType			= pDesc->eClassType;
	m_strName				= pDesc->strName;
	m_strCanvasName			= pDesc->strCanvasName;
	m_iCanvasIndex			= pDesc->iCanvasIndex;
	m_wstrTextureTag		= Engine_Utils::ToWString(pDesc->strInitTextureTag);
	m_wstrNoiseTextureTag		= Engine_Utils::ToWString(pDesc->strNoiseTextureTag);
	m_wstrAlphaMaskTextureTag	= Engine_Utils::ToWString(pDesc->strAlphaMaskTextureTag);
	m_wstrGlowTextureTag		= Engine_Utils::ToWString(pDesc->strGlowTextureTag);
	m_eRectTransformType	= static_cast<ERectTransform>(pDesc->iRectTransformType);
	m_pCacheCanvas			= pDesc->pCacheCanvas;
	m_isUseColorTint		= pDesc->isUseColorTint;
	m_vColorTint			= pDesc->vColorTint;
	m_vGradiantColorTint	= pDesc->vGradiantColorTint;
	m_iShaderPass			= pDesc->iShaderPass;
	m_iFillDir				= pDesc->iFillDir;
	m_fDelay				= pDesc->fDelay;
	m_eSubClassType			= pDesc->eSubClassType;
	m_iFlip					= pDesc->iFlip;
	m_fTestAlpha			= pDesc->fAlpha;

	// Local Values
	m_iIndex = pDesc->iIndex;
	if (m_eClassType == DTO::EUIClassType::UI_TEXT)
	{
		m_tUITextData			= pDesc->tTextData;
		m_wstrText_TextData		= Engine_Utils::ToWString(m_tUITextData.strText);
		m_eTextSubClassType		= m_tUITextData.eTextSubClassType;
		m_eFontShaderType		= m_tUITextData.eShaderType;
		m_vFontColor_TextData	= m_tUITextData.vFontColor;
		m_fScale_TextData		= m_tUITextData.fScale;
		m_ePivot_TextData		= m_tUITextData.ePivot;
		m_fRotate_TextData		= m_tUITextData.fRotate;
		m_strFontName_TextData	= m_tUITextData.strFontTag;
	}
	if (m_eClassType == DTO::EUIClassType::DYNAMIC_IMAGE)
	{
		m_tDImageData			= pDesc->tDImageData;
		m_eDImageSubClassType	= m_tDImageData.eDISubClassType;
		m_iParam0				= m_tDImageData.iParams0;
	}

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrTextureTag, DEFAULT)))
		return E_FAIL;
		
	if (m_wstrNoiseTextureTag != L"")
	{
		if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrNoiseTextureTag, NOISE)))
			return E_FAIL;
	}
	if (m_wstrAlphaMaskTextureTag != L"")
	{
		if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrAlphaMaskTextureTag, ALPHA_MASK)))
			return E_FAIL;
	}
    return S_OK;
}

HRESULT CToolUI::Awake(const _uint iCurrentLevelID)
{   
	if (FAILED(Super::Awake(iCurrentLevelID)))
        return E_FAIL;

	m_pBatch	= new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);
	m_pEffect	= new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);
	m_iInteractState = static_cast<uint32_t>(EUIInteract_Flag::NONE);
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
	Acting_About_State();
	Sync_Data();
}

void CToolUI::Ready_Before_Render(const _float fTimeDelta)
{
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
	return false;

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

	return S_OK;
}

HRESULT CToolUI::Bind_ShaderResources()
{
    CShader* pShader = Get_Component<CShader>();
	pShader->Set_Pass(m_iShaderPass);

    if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
        return E_FAIL;
 	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
		return E_FAIL;

	if (FAILED(pShader->Get_Variable("g_iFlip")->SetRawValue(&m_iFlip, 0, sizeof(int32_t))))
		return E_FAIL;

	const int32_t isColor = m_isUseColorTint ? 1 : 0;
	if (FAILED(pShader->Get_Variable("g_iColor")->SetRawValue(&isColor, 0, sizeof(int32_t))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_vColorTint")->SetRawValue(&m_vColorTint, 0, sizeof(Vec4))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_vGradiateColorTint")->SetRawValue(&m_vGradiantColorTint, 0, sizeof(Vec4))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fAlphaRatio")->SetRawValue(&m_fTestAlpha, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_iFillDir")->SetRawValue(&m_iFillDir, 0, sizeof(int32_t))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fProgressRatio")->SetRawValue(&m_fTestProgress, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fBrightness")->SetRawValue(&m_fBrightness, 0, sizeof(_float))))
		return E_FAIL;


	// Glow 
	if (FAILED(pShader->Get_Variable("g_vNoiseUVScale")->SetRawValue(&m_vNoiseUVScale, 0, sizeof(Vec2))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_vNoiseUVScroll")->SetRawValue(&m_vNoiseUVScroll, 0, sizeof(Vec2))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fTime")->SetRawValue(&m_fTime, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fGlowDistort")->SetRawValue(&m_fGlowDistort, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fGlowPulseSpeed")->SetRawValue(&m_fGlowPulseSpeed, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fGlowIntensity")->SetRawValue(&m_fGlowIntensity, 0, sizeof(_float))))
		return E_FAIL;

	if(m_eClassType == DTO::EUIClassType::UI_TEXT)
	{
		m_tFontDesc.eFontShaderType = m_eFontShaderType;
		m_tFontDesc.strFontTag		= Engine_Utils::ToWString(m_strFontName_TextData);
		m_tFontDesc.strText			= m_wstrText_TextData;
		Vec2 fontPos				= Vec2{ m_vRenderPos.x, m_vRenderPos.y };
		m_tFontDesc.vPosition		= fontPos;
		m_tFontDesc.vColor			= m_vFontColor_TextData;
		m_tFontDesc.ePivot			= m_ePivot_TextData;
		m_tFontDesc.fRotate			= m_fRotate_TextData;
		m_tFontDesc.fScale			= m_fScale_TextData;
		if (FAILED(m_pGameInstance->Request_DrawFont(m_tFontDesc)))
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
	if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::INVOKED))
	{
	}

	if (m_iInteractState == EUIInteract_Flag::NONE)
	{
	}
	else
	{
		if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::PRESS_ENTER))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::PRESS_EXIT))
		{
			CImGui_UIManager::GetInstance()->Safe_Change_UI(m_iIndex);
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_ENTER))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVER_EXIT))
		{
		}

		if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::PRESSING))
		{
		}
		else if (Engine_Utils::Has_Flag(m_iInteractState, EUIInteract_Flag::HOVERING))
		{
		}
	}
}

void CToolUI::Sync_Data()
{
	// UI Object Values
	m_tUIData.fWidth					= m_fWidth ;
	m_tUIData.fHeight					= m_fHeight ;
	m_tUIData.fPosX						= m_fX ;
	m_tUIData.fPosY						= m_fY ;
	m_tUIData.fPosZ						= m_fZ;
	m_tUIData.isVisible					= m_isVisible;
	m_tUIData.isInteract				= m_isInteract;
	m_tUIData.isActivate				= m_isActive;
	m_tUIData.fScale					= m_fScale;
	m_tUIData.fRotate					= m_fRotate;

	// Tool UI Values
	m_tUIData.strTag					= m_strName;
	m_tUIData.strCanvasName				= m_strCanvasName;
	m_tUIData.iRectTransformType		= static_cast<uint32_t>( m_eRectTransformType);
	m_tUIData.strTextureTag				= Engine_Utils::ToString(m_wstrTextureTag);
	m_tUIData.strNoiseTextureTag		= Engine_Utils::ToString(m_wstrNoiseTextureTag);
	m_tUIData.strAlphaMaskTextureTag	= Engine_Utils::ToString(m_wstrAlphaMaskTextureTag);
	m_tUIData.strGlowTextureTag			= Engine_Utils::ToString(m_wstrGlowTextureTag);
	m_tUIData.eClassType				= m_eClassType;
	m_tUIData.iComponentFlag			= m_iComponentFlag;
	m_tUIData.eSubClassType 			= m_eSubClassType;
	m_tUIData.isUseColorTint 			= m_isUseColorTint;
	m_tUIData.vColorTint 				= m_vColorTint;
	m_tUIData.vGradiantColorTint		= m_vGradiantColorTint;
	m_tUIData.iShaderPass				= m_iShaderPass;
	m_tUIData.fDelay					= m_fDelay;
	m_tUIData.iFlip						= m_iFlip;
	m_tUIData.fAlphaRatio				= m_fTestAlpha;
	m_tUIData.iFillDir					= m_iFillDir;

	if (m_eClassType == DTO::EUIClassType::UI_TEXT)
	{
		Sync_TextData();
	}
	else if (m_eClassType == DTO::EUIClassType::DYNAMIC_IMAGE)
	{
		Sync_DImageData();
	}
}

void CToolUI::Sync_TextData()
{
	m_tUITextData.strTag		= m_strName + "_TextData";
	m_tUITextData.strOwnerName	= m_strName;
	m_tUITextData.eTextSubClassType = m_eTextSubClassType;
	m_tUITextData.eShaderType	= m_eFontShaderType;
	m_tUITextData.strText		= Engine_Utils::ToString(m_wstrText_TextData);
	m_tUITextData.vFontColor	= m_vFontColor_TextData;
	m_tUITextData.fRotate		= m_fRotate_TextData;
	m_tUITextData.ePivot		= m_ePivot_TextData;
	m_tUITextData.strFontTag	= m_strFontName_TextData;
	m_tUITextData.fScale		= m_fScale_TextData;
	m_tUITextData.iParam0		= m_iTextParam0;
}

void CToolUI::Sync_DImageData()
{
	m_tDImageData.strTag			= m_strName + "_DImageData";
	m_tDImageData.strOwnerName		= m_strName;
	m_tDImageData.eDISubClassType	= m_eDImageSubClassType;
	m_tDImageData.iParams0			= m_iParam0;
}


_bool CToolUI::Add_Tag(vector<_string>& vec, const _string& str)
{
	if (str == "")
		return false;
	if (std::find(vec.begin(), vec.end(), str) != vec.end())
		return false;
	vec.push_back(str);
	return true;
}

_bool CToolUI::Remove_Tag(vector<_string>& vec, const _string& str)
{
	if (str == "")
		return false;

	auto it = std::find(vec.begin(), vec.end(), str);
	if (it == vec.end())
		return false;
	vec.erase(it);
	return true;
}

HRESULT CToolUI::Request_Change_Texture()
{
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrTextureTag, DEFAULT)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolUI::Request_Change_NoiseTexture()
{
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrNoiseTextureTag, NOISE)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolUI::Request_Change_AlphaMaskTexture()
{
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrAlphaMaskTextureTag, ALPHA_MASK)))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolUI::Request_Change_GlowTexture()
{
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrGlowTextureTag, GLOW)))
		return E_FAIL;

	return S_OK;
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


