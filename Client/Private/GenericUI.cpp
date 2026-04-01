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
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;

	GENERIC_UI_DESC* pDesc = static_cast<GENERIC_UI_DESC*>(pArg);
	m_strName					= pDesc->strName;
	m_iLevelID					= pDesc->iLevelIndex;
	m_eRectTransformType		= static_cast<ERectTransform>(pDesc->iRectTransformType);

	if (pDesc->wstrTextureTag != L"")
	{
		m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::DEFAULT)].push_back(pDesc->wstrTextureTag);
	}
	if (pDesc->wstrNoiseTextureTag != L"")
	{
		m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::NOISE)].push_back(pDesc->wstrNoiseTextureTag);
	}
	if (pDesc->wstrAlphaMaskTextureTag != L"")
	{
		m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::ALPHA_MASK)].push_back(pDesc->wstrAlphaMaskTextureTag);
	}
	if (pDesc->wstrGlowTextureTag != L"")
	{
		m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::GLOW)].push_back(pDesc->wstrGlowTextureTag);
	}

	m_pParentCanvasCache		= pDesc->pCanvasCache;
	m_isUseColorTint			= pDesc->isUseColorTint;
	m_vColorTint				= pDesc->vColorTint;
	m_vGradiantColorTint		= pDesc->vGradiantColorTint;
	m_iShaderPass				= pDesc->iShaderPass;
	m_iFillDir					= pDesc->iFillDir;
	m_fDelay					= pDesc->fDelay;
	m_fAlpha_Ratio				= pDesc->fAlpha;
	m_iFlip						= pDesc->iFlip;


	Get_Component<CShader>()->Set_Pass(m_iShaderPass);

	auto* pTexture = Get_Component<CTexture>();
	// Default Texture Binding
	if (FAILED(pTexture->Add_DefaultTexture(m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::DEFAULT)].back(), ENUM_TO_UINT(EUITextureSlot::DEFAULT))))
		return E_FAIL;

	// Noise Texture Binding
	if (!m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::NOISE)].empty())
	{
		if (FAILED(pTexture->Add_DefaultTexture(m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::NOISE)].back(), ENUM_TO_UINT(EUITextureSlot::NOISE))))
			return E_FAIL;
	}
	// Alpha Mask Texture Binding
	if (!m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::ALPHA_MASK)].empty())
	{
		if (FAILED(pTexture->Add_DefaultTexture(m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::ALPHA_MASK)].back(), ENUM_TO_UINT(EUITextureSlot::ALPHA_MASK))))
			return E_FAIL;
	}
	// Glow Texture Binding
	if (!m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::GLOW)].empty())
	{
		if (FAILED(pTexture->Add_DefaultTexture(m_ArrTextures[ENUM_TO_UINT(EUITextureSlot::GLOW)].back(), ENUM_TO_UINT(EUITextureSlot::GLOW))))
			return E_FAIL;
	}

	m_vMoveOffsetBase = m_vMoveOffset;

	return S_OK;
}

HRESULT CGenericUI::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	m_fBrightness = 1.f;
	m_iInteractState = static_cast<uint32_t>(EUIInteract_Flag::NONE);

	m_vChangeColor_OriginColor = m_vColorTint;
	m_vChangeColor_OriginGradColor = m_vGradiantColorTint;

	Bind_Events();

	return S_OK;
}

void CGenericUI::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CGenericUI::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CGenericUI::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CGenericUI::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	if (nullptr != m_pWorldUIComp)
	{
		Set_Position(Vec3{ m_pWorldUIComp->Get_TargetScreenPos().x + m_vMoveOffset.x , m_pWorldUIComp->Get_TargetScreenPos().y + m_vMoveOffset.y, m_fZ }) ;
		Move_Size(m_fWidth * m_pWorldUIComp->Get_ScaleOffset(), m_fHeight * m_pWorldUIComp->Get_ScaleOffset());
	}
	else
	{
		m_vRenderPos = Vec3{ m_vRectPos.x + m_vMoveOffset.x + m_fX, m_vRectPos.y + m_vMoveOffset.y + m_fY, m_fZ };
		Move_Position(m_vRenderPos.x, m_vRenderPos.y, m_vRenderPos.z);
		Move_Size(m_fWidth * m_fScale, m_fHeight * m_fScale);

		m_tRenderRect.left = static_cast<LONG>(m_vRenderPos.x - (m_fWidth * 0.5f));
		m_tRenderRect.right = static_cast<LONG>(m_vRenderPos.x + (m_fWidth * 0.5f));
		m_tRenderRect.top = static_cast<LONG>(m_vRenderPos.y - (m_fHeight * 0.5f));
		m_tRenderRect.bottom = static_cast<LONG>(m_vRenderPos.y + (m_fHeight * 0.5f));
	}

	m_pGameInstance->Push_RenderObject(RENDER_CATEGORY::UI, this);
}

HRESULT CGenericUI::Render()
{
	if (FAILED(Super::Render()))
		return E_FAIL;

	Get_Component<CShader>()->Apply();
	Get_Component<CVIBuffer>()->Bind_Resource();
	Get_Component<CVIBuffer>()->Render();
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

HRESULT CGenericUI::Ready_Components(GENERIC_UI_DESC* pDesc)
{
	if (FAILED(Add_Component<CVIBuffer_Rect_Tex>(0, L"Prototype_Component_VIBuffer_Rect_Tex", pDesc)))
		return E_FAIL;

	if ((pDesc->iComponentFlag & DTO::EComponentTypeFlag::WORLDUI_COMPONENT) != 0)
	{
		CWorldUI_Component::WOLRD_UI_COMP_DESC Desc = {};
		Desc.pTargetObject = nullptr;
		D3D11_VIEWPORT vp = {};
		_uint n = 1;
		m_pDeviceContext->RSGetViewports(&n, &vp);
		Desc.fVPWidth		= vp.Width;
		Desc.fVPHegiht		= vp.Height;
		Desc.fVPTopLeftX	= vp.TopLeftX;
		Desc.fVPTopLeftY	= vp.TopLeftY;
		Desc.fInitOffset	= Vec2{ m_fX, m_fY };

		if (FAILED(Add_Script_Component(L"WorldUIComponent", L"Prototype_ScriptComponent_WorldUI", &Desc)))
			return E_FAIL;

		auto* pScriptComp = Get_Script_Component(L"WorldUIComponent");
		if (nullptr == pScriptComp)
			return E_FAIL;

		CWorldUI_Component* pWorldUIComp = static_cast<CWorldUI_Component*>(pScriptComp);
		if (nullptr == pWorldUIComp)
			return E_FAIL;

		m_pWorldUIComp = pWorldUIComp;
	}
	return S_OK;
}

HRESULT CGenericUI::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();

	if (FAILED(Get_Component<CTexture>()->Bind_ShaderResourceBuffer(pShader)))
		return E_FAIL;

	const int32_t isColor = m_isUseColorTint ? 1 : 0;

	if (FAILED(pShader->Get_Variable("g_iFlip")->SetRawValue(&m_iFlip, 0, sizeof(int32_t))))
		return E_FAIL;
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
	if (FAILED(pShader->Get_Variable("g_fAllUIControllAlpha")->SetRawValue(&m_fAll_Controll_Alpha, 0, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CGenericUI::Ready_Lerp_Movement(const Vec2& vStartOffset, const Vec2& vTargetOffset, const _float fDuration, const _float fEaseValue, const _float fDelay, _bool isEaseOut)
{
	m_fLerpMove_TimeAcc = 0.f;
	m_fLerpMove_DelayTimeAcc = 0.f;

	m_vMoveOffset = vStartOffset;
	m_vLerpMove_StartOffset	= vStartOffset;
	m_vLerpMove_TargetOffset = vTargetOffset;
	m_fLerpMove_Duration = fDuration;
	m_fLerpMove_EaseValue = fEaseValue;
	m_fLerpMove_Delay = fDelay;
	m_isLerpMove_EaseOut = isEaseOut;
}

void CGenericUI::Ready_Fade(const _float fDuration, const _float fStartAlpha, const _float fTargetAlpha, const _float fDelay)
{
	m_fAlpha_Ratio = fStartAlpha;

	m_fFade_TimeAcc = 0.f;
	m_fFade_DelayTimeAcc = 0.f;

	m_fFade_StartAlphaRatio = fStartAlpha;
	m_fFade_TargetAlphaRatio = fTargetAlpha;
	
	m_fFade_Duration = fDuration;
	m_fFade_Delay = fDelay;
	m_fFade_EaseValue = 0.f;
}

void CGenericUI::Ready_LerpChange(const _float fDuration, const _float fStartValue, const _float fTargetValue, const _float fEaseValue, const _float fDelay, _bool isEaseOut)
{
	m_fLerpChange_TimeAcc = 0.f;
	m_fLerpChange_DelayTimeAcc = 0.f;

	m_fLerpChange_Delay = fDelay;
	m_fLerpChange_Duration = fDuration;
	m_fLerpChange_StartValue = fStartValue;
	m_fLerpChange_TargetValue = fTargetValue;
	m_fLerpChange_EaseValue = fEaseValue;
	m_isLerpChange_EaseOut = isEaseOut;
}
void CGenericUI::Ready_ChageColor(const _float fDuration, const Vec4& vStartColor, const Vec4& vStartGradColor, const Vec4& vTargetColor, const Vec4& vTargetGradColor, const _float fEaseValue, const _float fDelay, _bool isEaseOut)
{
	m_fChangeColor_TimeAcc = 0.f;
	m_fChangeColor_DelayTimeAcc = 0.f;

	m_fChangeColor_Delay = fDelay;
	m_fChangeColor_Duration = fDuration;
	m_vChangeColor_StartColor = vStartColor;
	m_vChangeColor_StartGradColor = vStartGradColor;
	m_vChangeColor_TargetColor = vTargetColor;
	m_vChangeColor_TargetGradColor = vTargetGradColor;
	m_fChangeColor_EaseValue = fEaseValue;
	m_isChangeColor_EaseOut = isEaseOut;
}

_bool CGenericUI::Tick_Lerp_Movement(const _float fTimeDelta)
{
	m_fLerpMove_DelayTimeAcc += fTimeDelta;
	if (m_fLerpMove_DelayTimeAcc < m_fLerpMove_Delay)
		return false;

	m_fLerpMove_TimeAcc += fTimeDelta;

	_float t = m_fLerpMove_TimeAcc / m_fLerpMove_Duration;

	if (t >= 1.f)
	{
		m_vMoveOffset = m_vMoveOffsetBase + m_vLerpMove_TargetOffset;
		return true;
	}

	_float eased = t;
	
	if (m_fLerpMove_EaseValue > 0.f)
	{
		if(m_isLerpMove_EaseOut)
			eased = 1.f - powf(1.f - t, m_fLerpMove_EaseValue);
		else
			eased = powf(t, m_fLerpMove_EaseValue);
	}

	m_vMoveOffset = m_vMoveOffsetBase + (m_vLerpMove_StartOffset + (m_vLerpMove_TargetOffset - m_vLerpMove_StartOffset) * eased);
	return false;
}

_bool CGenericUI::Tick_Fade(const _float fTimeDelta)
{
	m_fFade_DelayTimeAcc += fTimeDelta;
	if (m_fFade_DelayTimeAcc < m_fFade_Delay)
		return false;

	m_fFade_TimeAcc += fTimeDelta;

	_float t = m_fFade_TimeAcc / m_fFade_Duration;

	if (t >= 1.f)
	{
		m_fAlpha_Ratio = m_fFade_TargetAlphaRatio;
		return true;
	}

	_float eased = t;
	if (m_fFade_EaseValue > 0.f)
		eased = powf(t, m_fFade_EaseValue);

	m_fAlpha_Ratio = m_fFade_StartAlphaRatio + (m_fFade_TargetAlphaRatio - m_fFade_StartAlphaRatio) * t;
	
	return false;
}

_bool CGenericUI::Tick_LerpChange(_float* p, const _float fTimeDelta)
{
	m_fLerpChange_DelayTimeAcc += fTimeDelta;
	if (m_fLerpChange_DelayTimeAcc < m_fLerpChange_Delay)
		return false;

	m_fLerpChange_TimeAcc += fTimeDelta;

	_float t = m_fLerpChange_TimeAcc / m_fLerpChange_Duration;

	if (t >= 1.f)
	{
		*p = m_fLerpChange_TargetValue;
		return true;
	}

	_float eased = t;

	if (m_fLerpChange_EaseValue > 0.f)
	{
		if (m_isLerpChange_EaseOut)
			eased = 1.f - powf(1.f - t, m_fLerpChange_EaseValue);
		else
			eased = powf(t, m_fLerpChange_EaseValue);
	}

	*p = m_fLerpChange_StartValue + (m_fLerpChange_TargetValue - m_fLerpChange_StartValue) * eased;
	return false;
}

_bool CGenericUI::Tick_ChageColor(const _float fTimeDelta)
{
	m_fChangeColor_DelayTimeAcc += fTimeDelta;
	if (m_fChangeColor_DelayTimeAcc < m_fChangeColor_Delay)
		return false;

	m_fChangeColor_TimeAcc += fTimeDelta;

	_float t = m_fChangeColor_TimeAcc / m_fChangeColor_Duration;

	if (t >= 1.f)
	{
		m_vColorTint = m_vChangeColor_TargetColor;
		m_vGradiantColorTint = m_vChangeColor_TargetGradColor;
		return true;
	}

	_float eased = t;

	if (m_fChangeColor_EaseValue > 0.f)
	{
		if (m_isChangeColor_EaseOut)
			eased = 1.f - powf(1.f - t, m_fChangeColor_EaseValue);
		else
			eased = powf(t, m_fChangeColor_EaseValue);
	}

	m_vColorTint = m_vChangeColor_StartColor + (m_vChangeColor_TargetColor - m_vChangeColor_StartColor) * eased;
	m_vGradiantColorTint = m_vChangeColor_StartGradColor + (m_vChangeColor_TargetGradColor - m_vChangeColor_StartGradColor) * eased;
	return false;
}

_bool CGenericUI::Tick_ChangeOriginColor(const _float fTimeDelta)
{
	m_fChangeColor_DelayTimeAcc += fTimeDelta;
	if (m_fChangeColor_DelayTimeAcc < m_fChangeColor_Delay)
		return false;

	m_fChangeColor_TimeAcc += fTimeDelta;

	_float t = m_fChangeColor_TimeAcc / m_fChangeColor_Duration;

	if (t >= 1.f)
	{
		m_vColorTint = m_vChangeColor_OriginColor;
		m_vGradiantColorTint = m_vChangeColor_OriginGradColor;
		return true;
	}

	_float eased = t;

	if (m_fChangeColor_EaseValue > 0.f)
	{
		if (m_isChangeColor_EaseOut)
			eased = 1.f - powf(1.f - t, m_fChangeColor_EaseValue);
		else
			eased = powf(t, m_fChangeColor_EaseValue);
	}

	m_vColorTint = m_vChangeColor_StartColor + (m_vChangeColor_OriginColor - m_vChangeColor_StartColor) * eased;
	m_vGradiantColorTint = m_vChangeColor_StartGradColor + (m_vChangeColor_OriginGradColor - m_vChangeColor_StartGradColor) * eased;
	return false;
}

void CGenericUI::Request_SetDead()
{
	m_isDeadRequest = true;
}

void CGenericUI::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DEFAULT_UI_VISIBLE>([this]()
			{		
				if (this->Get_Name() == "Quest_Title_Icon")
					int a = 0;

				if (m_isEventVisible)
				{
					Set_Active(true);
					this->Set_Visible();
					m_isEventVisible = false;
				m_fAll_Controll_Alpha = 1.f;
					
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DEFAULT_UI_INVISIBLE>([this]()
			{
				if (this->Get_Name() == "Quest_Title_Icon")
					int a = 0;

				if (this->m_isVisible)
				{
					this->Set_Invisible();
					m_isEventVisible = true;
				m_fAll_Controll_Alpha = 0.f;
				}
			}));
}

void CGenericUI::Free()
{
	for (auto Handle : m_vecEventHandles)
	{
		m_pUIManager->Get_UIEvents().Unsubscribe(Handle);
	}

	Safe_Release(m_pUIManager);
	Super::Free();
}
