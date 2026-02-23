#include "pch.h"
#include "GenericUI.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "WorldUI_Component.h"
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
	m_wstrNoiseTextureTag	= pDesc->wstrNoiseTextureTag;
	m_wstrAlphaMaskTextureTag= pDesc->wstrAlphaMaskTextureTag;
	m_iTextureIndex			= pDesc->iTextureIndex;
	m_iComponentFlag		= pDesc->iComponentFlag;
	m_pParentCanvasCache	= pDesc->pCanvasCache;
	m_isUseColorTint		= pDesc->isUseColorTint;
	m_vColorTint			= pDesc->vColorTint;
	m_vGradiantColorTint	= pDesc->vGradiantColorTint;
	m_iShaderPass			= pDesc->iShaderPass;
	m_iFillDir				= pDesc->iFillDir;
	m_fDelay				= pDesc->fDelay;
	m_fAlpha_Ratio			= pDesc->fAlpha;
	m_iFlip					= pDesc->iFlip;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	Get_Component<CShader>()->Set_Pass(m_iShaderPass);
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

	m_vMoveOffsetBase = m_vMoveOffset;
	m_fBrightness = 1.f;
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
	if (nullptr != m_pWorldUIComp)
	{
		if (m_strName == "MonsterHP_BG")
			int a = 0;
		Set_Position(Vec3{ m_pWorldUIComp->Get_TargetScreenPos().x, m_pWorldUIComp->Get_TargetScreenPos().y, m_fZ }) ;
		Move_Size(m_fWidth * m_pWorldUIComp->Get_ScaleOffset(), m_fHeight * m_pWorldUIComp->Get_ScaleOffset());
	}
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
	if (!m_isInteract)
		return FALSE;
	if (::PtInRect(&m_tRenderRect, m_pGameInstance->Get_MousePos()))
		return TRUE;
	return FALSE;
}

void CGenericUI::Acting_By_InteractState()
{
}

void CGenericUI::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
}

HRESULT CGenericUI::Ready_Components(GENERIC_UI_DESC* pDesc)
{
	if (FAILED(Add_Component<CTexture>(ENUM_TO_UINT(ELevelType::STATIC), L"Prototype_Component_Texture_Empty", pDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CShader>(0, L"Prototype_Component_Shader_VtxPosTex", pDesc)))
		return E_FAIL;
	if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
		return E_FAIL;

	if ((pDesc->iComponentFlag & DTO::EComponentTypeFlag::WORLDUI_COMPONENT) != 0)
	{
		CWorldUI_Component::WOLRD_UI_COMP_DESC Desc = {};
		auto* p = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::LOGO), L"Monster");
		if (nullptr == p)
			return E_FAIL;
		//Desc.pTargetTransform = (pDesc->pTarget->Get_Component<CTransform>());
		Desc.pTargetObject = (p);

		D3D11_VIEWPORT vp = {};
		_uint n = 1;
		m_pDeviceContext->RSGetViewports(&n, &vp);
		Desc.fVPWidth		= vp.Width;
		Desc.fVPHegiht		= vp.Height;
		Desc.fVPTopLeftX	= vp.TopLeftX;
		Desc.fVPTopLeftY	= vp.TopLeftY;
		Desc.fInitOffset = Vec2{ m_fX, m_fY };

		if (FAILED(Add_Script_Component(L"WorldUIComponent", L"Prototype_ScriptComponent_WorldUI", &Desc)))
			return E_FAIL;

		auto* pScriptComp = Get_Script_Component(L"WorldUIComponent");
		if (nullptr == pScriptComp)
			return E_FAIL;
		auto* pWorldUIComp = static_cast<CWorldUI_Component*>(pScriptComp);
		if (nullptr == pWorldUIComp)
			return E_FAIL;
		m_pWorldUIComp = pWorldUIComp;
	}
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
	if (FAILED(pShader->Get_Variable("g_vGradiateColorTint")->SetRawValue(&m_vGradiantColorTint, 0, sizeof(Vec4))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fAlphaRatio")->SetRawValue(&m_fAlpha_Ratio, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_iFillDir")->SetRawValue(&m_iFillDir, 0, sizeof(int32_t))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fProgressRatio")->SetRawValue(&m_fProgress_Ratio, 0, sizeof(_float))))
		return E_FAIL;
	if (FAILED(pShader->Get_Variable("g_fBrightness")->SetRawValue(&m_fBrightness, 0, sizeof(_float))))
		return E_FAIL;
	
	return S_OK;
}

void CGenericUI::Ready_Lerp_Movement(const Vec2& vStartOffset, const Vec2& vTargetOffset, const _float fDuration, const _float fEaseValue, const _float fDelay)
{
	m_fTimeAcc		= 0.f;
	m_fDelayTimeAcc = 0.f;
	m_vStartOffset	= vStartOffset;
	m_vTargetOffset	= vTargetOffset;
	m_fDuration		= fDuration;
	m_fEaseValue	= fEaseValue;
	m_fLerpDelay	= fDelay;
}

_bool CGenericUI::Tick_Lerp_Movement(const _float fTimeDelta)
{
	m_fDelayTimeAcc += fTimeDelta;
	if (m_fDelayTimeAcc < m_fLerpDelay)
		return false;

	m_fTimeAcc += fTimeDelta;

	_float t = m_fTimeAcc / m_fDuration;
	if (t >= 1.f)
	{
		m_vMoveOffset = m_vMoveOffsetBase + m_vTargetOffset;
		return true;
	}

	_float eased = t;
	if (m_fEaseValue > 0.f)
		eased = powf(t, m_fEaseValue);

	m_vMoveOffset = m_vMoveOffsetBase + (m_vStartOffset + (m_vTargetOffset - m_vStartOffset) * eased);
	return false;
}

void CGenericUI::Ready_Fade(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fDelay)
{
	m_fAlpha_Ratio		= fStartAlpha;
	m_fFadeTimeAcc		= 0.f;
	m_fFadeDelayTimeAcc = 0.f;
	m_fFadeDelay		= fDelay;
	m_fFadeDuration		= fDuration;
	m_fStartAlphaRatio	= fStartAlpha;
	m_fTargetAlphaRatio = fTargetAlpha;
}

void CGenericUI::Ready_ExplosionFade(const _float fDuration, const _float fStartAlpha, const _float fExplosionAlpha, const _float fTargetAlpha, const _float fDelay)
{
	m_fAlpha_Ratio = fStartAlpha;
	m_fFadeTimeAcc = 0.f;
	m_fFadeDelayTimeAcc = 0.f;
	m_fFadeDelay = fDelay;
	m_fFadeDuration = fDuration;
	m_fStartAlphaRatio = fExplosionAlpha;
	m_fTargetAlphaRatio = fTargetAlpha;
}

_bool CGenericUI::Tick_Fade(const _float fTimeDelta)
{
	m_fFadeDelayTimeAcc += fTimeDelta;
	if (m_fFadeDelayTimeAcc < m_fFadeDelay)
		return false;

	m_fFadeTimeAcc += fTimeDelta;

	_float t = m_fFadeTimeAcc / m_fFadeDuration;
	if (t >= 1.f)
	{
		m_fAlpha_Ratio = m_fTargetAlphaRatio;
		return true;
	}

	_float eased = t;
	if (m_fEaseValue > 0.f)
		eased = powf(t, m_fEaseValue);

	_float f = m_fStartAlphaRatio + (m_fTargetAlphaRatio - m_fStartAlphaRatio) * t;
	m_fAlpha_Ratio = f;
	return false;
}

void CGenericUI::Free()
{
	Safe_Release(m_pUIManager);
	Super::Free();
}
