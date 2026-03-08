#include "pch.h"
#include "UILoading_Text.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"

CUILoading_Text::CUILoading_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUILoading_Text::CUILoading_Text(const CUILoading_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUILoading_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Initialize(void* pArg)
{
	LOADING_TEXT_DESC* pDesc = static_cast<LOADING_TEXT_DESC*>(pArg);
	m_pLoadingRatio = pDesc->pLoadingRatio;

	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;
	return S_OK;
}

void CUILoading_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUILoading_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
	// Ratio °ª °»½Å 
	if (m_eTextSubClassType == DTO::EUITextSubClassType::LOADING_TEXT_PERCENT && nullptr != m_pLoadingRatio)
	{
		_float fNewTarget = (*m_pLoadingRatio) * 100.f;

		if (fNewTarget < 0.f)	fNewTarget = 0.f;
		if (fNewTarget > 100.f) fNewTarget = 100.f;

		const _float fEps = 0.0001f;
		if (fabsf(fNewTarget - m_fTargetPercent) > fEps)
		{
			m_fStartPercent = m_fLerpPercent;
			m_fTargetPercent = fNewTarget;
			m_fPercentTimeAcc = 0.f;
		}
	}
}

void CUILoading_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	// Percent °è»ê
	if (m_eTextSubClassType == DTO::EUITextSubClassType::LOADING_TEXT_PERCENT)
	{
		Lerp_Percent(fTimeDelta);
	}
}

void CUILoading_Text::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);

	if (MOUSE_RBUTTON_DOWN)
	{
		m_iCurTextCursor++;
		if (m_iCurTextCursor >= m_vecText.size())
			m_iCurTextCursor = 0;
	}

	Convert_Value_To_Text();
}

HRESULT CUILoading_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Ready_Components(LOADING_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUILoading_Text::Attach_Personal_Info()
{
	m_iCurTextCursor = 0;
	m_vecText.reserve(10);

	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::LOADING_TEXT_TITLE:
		m_vecText.push_back(L"°­º´ÁØ");
		m_vecText.push_back(L"¼ÒÀçÇõ");
		m_vecText.push_back(L"ÃÖÁ¤¿ì");
		m_vecText.push_back(L"¹Î°ÇÈñ");
		m_vecText.push_back(L"±èÀººñ");
		m_vecText.push_back(L"¹é½Â¹Î");
		break;
	case DTO::EUITextSubClassType::LOADING_TEXT_CONTENTS:
		m_vecText.push_back(L"ÆÀÀå / ÇÁ·¹ÀÓ¿öÅ©");
		m_vecText.push_back(L"¼Ò¹æ°ü / ÇÇÁ÷½º");
		m_vecText.push_back(L"ÀÌÆåÆ®");
		m_vecText.push_back(L"¸Ê");
		m_vecText.push_back(L"ÇÃ·¹ÀÌ¾î");
		m_vecText.push_back(L"UI");
		break;
	}
	return S_OK;
}

void CUILoading_Text::Lerp_Percent(const _float fTimeDelta)
{
	if (m_fDuration <= 0.f)
	{
		m_fPercentTimeAcc = 0.f;
		m_fLerpPercent = m_fTargetPercent;
		m_fStartPercent = m_fTargetPercent;
		m_wstrText = Float_To_Wstring(m_fLerpPercent, 0);
		return;
	}

	m_fPercentTimeAcc += fTimeDelta;

	_float t = m_fPercentTimeAcc / m_fDuration;
	if (t >= 1.f)
	{
		t = 1.f;
		m_fPercentTimeAcc = 0.f;
		m_fLerpPercent = m_fTargetPercent;
		m_fStartPercent = m_fTargetPercent;
		m_wstrText = Float_To_Wstring(m_fLerpPercent, 0);
		return;
	}

	m_fLerpPercent = m_fStartPercent + (m_fTargetPercent - m_fStartPercent) * t;
	m_wstrText = Float_To_Wstring(m_fLerpPercent, 0);
}

HRESULT CUILoading_Text::Convert_Value_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::LOADING_TEXT_TITLE:
	case DTO::EUITextSubClassType::LOADING_TEXT_CONTENTS:
		if (m_vecText.empty())
			break;
	m_wstrText = m_vecText[m_iCurTextCursor];
	break;
	}

	return S_OK;
}

void CUILoading_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	m_vFontColor.w = 0.f;
	m_fLoadingTimeAcc = 0.f;
}

void CUILoading_Text::Initialize_InVisible_Event()
{
}

_bool CUILoading_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fLoadingTimeAcc += fTimeDelta;

	if (m_fLoadingTimeAcc < m_fDelay)
		return false;

	m_vFontColor.w += fTimeDelta;
	if (m_vFontColor.w > 1.f)
	{
		m_vFontColor.w = 1.f;
		m_isActive = true;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

_bool CUILoading_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

CUILoading_Text* CUILoading_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUILoading_Text* pInstance = new CUILoading_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUILoading_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUILoading_Text::Clone(void* pArg)
{
	CUILoading_Text* pInstance = new CUILoading_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUILoading_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUILoading_Text::Free()
{
	Super::Free();
}
