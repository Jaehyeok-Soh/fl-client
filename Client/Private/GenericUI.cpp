#include "pch.h"
#include "GenericUI.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CGenericUI::CGenericUI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIObject(pDevice, pDeviceContext),
	m_pUIManager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

CGenericUI::CGenericUI(const CGenericUI& rhs)
	:CUIObject(rhs),
	m_pUIManager(CUI_Manager::GetInstance())
{
	Safe_AddRef(m_pUIManager);
}

HRESULT CGenericUI::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Initialize(void* pArg)
{
	GENERIC_UI_DESC* pDesc = static_cast<GENERIC_UI_DESC*>(pArg);
	m_strName				= pDesc->strName;
	m_iLevelID				= pDesc->iLevelIndex;
	m_eRectTransformType	= static_cast<ERectTransform>(pDesc->iRectTransformType);
	m_wstrTextureTag		= pDesc->wstrTextureTag;
	m_iTextureIndex			= pDesc->iTextureIndex;
	m_iComponentFlag		= pDesc->iComponentFlag;
	m_pParentCanvasCache	= pDesc->pCanvasCache;
	m_isUseColorTint		= pDesc->isUseColorTint;
	m_vColorTint			= pDesc->vColorTint;
	m_iShaderPass			= pDesc->iShaderPass;
	m_iFillDir				= pDesc->iFillDir;
	m_fDelay				= pDesc->fDelay;
	m_fAlpha_Ratio			= pDesc->fAlpha;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	Get_Component<CShader>()->Set_Pass(m_iShaderPass);
	if (FAILED(Get_Component<CTexture>()->Add_DefaultTexture(m_wstrTextureTag, 0)))
		return E_FAIL;
	return S_OK;
}

HRESULT CGenericUI::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_iInteractState = static_cast<uint32_t>(EUIEvent_Flag::NONE);
	return S_OK;
}

void CGenericUI::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGenericUI::Update(const _float fTimeDelta)
{
	m_vRenderPos = Vec3{ m_vRectPos.x + m_vMoveOffset.x + m_fX, m_vRectPos.y + m_vMoveOffset.y + m_fY, m_fZ };
	Move_Position(m_vRenderPos.x, m_vRenderPos.y, m_vRenderPos.z);

	m_tRenderRect.left		= static_cast<LONG>(m_vRenderPos.x - (m_fWidth * 0.5f));
	m_tRenderRect.right		= static_cast<LONG>(m_vRenderPos.x + (m_fWidth * 0.5f));
	m_tRenderRect.top		= static_cast<LONG>(m_vRenderPos.y - (m_fHeight * 0.5f));
	m_tRenderRect.bottom	= static_cast<LONG>(m_vRenderPos.y + (m_fHeight * 0.5f));

	Super::Update(fTimeDelta);
}

void CGenericUI::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CGenericUI::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CGenericUI::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

_bool CGenericUI::Calc_HitEvent()
{
	if (::PtInRect(&m_tRenderRect, m_pGameInstance->Get_MousePos()))
		return TRUE;
	return FALSE;
}

void CGenericUI::Acting_By_InteractState()
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

HRESULT CGenericUI::Ready_Components(GENERIC_UI_DESC* pDesc)
{
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Empty", pDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGenericUI::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	pShader->Set_Pass(m_iShaderPass);

	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_iFlip")->SetRawValue(&m_iFlip, 0, sizeof(int32_t))))
		return E_FAIL;

	const int32_t isColor = m_isUseColorTint ? 1 : 0;
	if (FAILED(pShader->Get_Variable("g_iColor")->SetRawValue(&isColor, 0, sizeof(int32_t))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_vColorTint")->SetRawValue(&m_vColorTint, 0, sizeof(Vec4))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fAlphaRatio")->SetRawValue(&m_fAlpha_Ratio, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_iFillDir")->SetRawValue(&m_iFillDir, 0, sizeof(int32_t))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fProgressRatio")->SetRawValue(&m_fProgress_Ratio, 0, sizeof(_float))))
		return E_FAIL;
	/*if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::DEFAULT))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;
	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::DEFAULT_ALPHA))
	{

	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::COLOR))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;

	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::FADE))
	{
		if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
			return E_FAIL;


	}
	else if (m_iShaderPass == ENUM_TO_UINT(EUIShaderPass::PROGRESS))
	{


		if (FAILED(pShader->Get_Variable("g_vColorTint")->SetRawValue(&m_vColorTint, 0, sizeof(Vec4))))
			return E_FAIL;



	}*/
	return S_OK;
}

void CGenericUI::Free()
{
	Safe_Release(m_pUIManager);
	Super::Free();
}
