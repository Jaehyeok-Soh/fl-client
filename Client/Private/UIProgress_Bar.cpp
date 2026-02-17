#include "pch.h"
#include "UIProgress_Bar.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "StatComponent.h"
#include "GameInstance.h"

CUIProgress_Bar::CUIProgress_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGenericUI(pDevice, pDeviceContext)
{
}

CUIProgress_Bar::CUIProgress_Bar(const CUIProgress_Bar& rhs)
	:CGenericUI(rhs)
{
}

HRESULT CUIProgress_Bar::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIProgress_Bar::Initialize(void* pArg)
{
	PROGRESS_BAR_DESC* pDesc = static_cast<PROGRESS_BAR_DESC*>(pArg);
	m_pTargetStat = pDesc->pTargetStat;
	m_eSubClassType = pDesc->eOwner;
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIProgress_Bar::Attach_Personal_Info()
{
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::NONE_OWNER:
		return S_OK;
	case DTO::EUISubClassType::PLAYER_HP:
	{
		m_pTargetStat;
		m_vOriginColor = m_vColorTint;
		m_vOriginGradiantColor = m_vGradiantColorTint;
		return S_OK;
	}
	case DTO::EUISubClassType::PLAYER_ARMOR:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUISubClassType::PLAYER_ENERGY:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUIProgress_Bar::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	return S_OK;
}

void CUIProgress_Bar::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);

	if (m_pGameInstance->KeyButton_Down(DIK_1))
		m_fCurRatio = 0.29f;
	if (m_pGameInstance->KeyButton_Down(DIK_2))
		m_fCurRatio = 0.5f;
	if (m_pGameInstance->KeyButton_Down(DIK_3))
		m_fCurRatio = 0.9f;


	if (m_eSubClassType == DTO::EUISubClassType::PLAYER_HP)
	{
		if (m_fProgress_Ratio < 0.3f)
		{
			if (!m_isStartLowHp)
			{
				m_isStartLowHp = TRUE;
				m_isEndLowHp = FALSE;
				m_fTickTimeAcc = 1.f;
			}
		}
		else
		{
			if (!m_isEndLowHp)
			{
				m_isStartLowHp = FALSE;
				m_isEndLowHp = TRUE;
				m_vColorTint = m_vOriginColor;
				m_vGradiantColorTint = m_vOriginGradiantColor;
			}
		}

		if (m_isStartLowHp)
		{
			Low_HP(fTimeDelta);
		}
	}
}

void CUIProgress_Bar::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// if (!m_pTargetStat)
	// 	return;
	// m_fCurRatio = m_pTargetStat->Get_HealthRatio();

	_float fEpsilon = 0.0001f;
	if (fabs( m_fCurRatio - m_fPreRatio) > fEpsilon)
	{
		m_isChangeRatio = TRUE;
		m_fStartRatio = m_fProgress_Ratio;
		m_fTargetRatio = m_fCurRatio;
		m_fTimeAcc = 0.f;
		m_fDelayTimeAcc = 0.f;
	}
	m_fPreRatio = m_fCurRatio;

}

void CUIProgress_Bar::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (m_isChangeRatio)
	{
		m_fDelayTimeAcc += fTimeDelta;
		if (m_fDelayTimeAcc <= m_fDelay)
			return;

		m_fTimeAcc += fTimeDelta;
		_float t = m_fTimeAcc / m_fDuration;

		if (t >= 1.f)
		{
			m_fProgress_Ratio = m_fTargetRatio;
			m_isChangeRatio = FALSE;
		}
		else
		{
			m_fProgress_Ratio = m_fStartRatio + (m_fTargetRatio - m_fStartRatio) * t;
		}
	}
}

void CUIProgress_Bar::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
	Acting_By_InteractState();
}

HRESULT CUIProgress_Bar::Render()
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

void CUIProgress_Bar::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (eEvent == ETriggerEventType::PRESS_ENTER)
	{
		if (m_isVisible)
			Set_Invisible();
		else
			Set_Visible();
	}
}

void CUIProgress_Bar::Initialize_Visible_Event()
{
	m_fTimeAcc = 0.f;
	m_fAlpha_Ratio = 0.f;
}

void CUIProgress_Bar::Initialize_InVisible_Event()
{
	m_fTimeAcc = 0.f;
}

_bool CUIProgress_Bar::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fAlpha_Ratio += fTimeDelta * 2.f;
	if (m_fAlpha_Ratio >= 1.f)
	{
		m_fAlpha_Ratio = 1.f;
		return true;
	}
	return false;
}

_bool CUIProgress_Bar::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUIProgress_Bar::Ready_Components(PROGRESS_BAR_DESC* pDesc)
{
	return S_OK;
}

HRESULT CUIProgress_Bar::Bind_ShaderResources()
{
	Super::Bind_ShaderResources();

	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	return S_OK;
}

void CUIProgress_Bar::Low_HP(const _float fTimeDelta)
{
	m_vColorTint = Vec4{ 1.f, 0.f, 0.f, 1.f };
	m_vGradiantColorTint = Vec4{ 1.f, 0.f, 0.f, 1.f };

	if (m_fTickTimeAcc >= 1.f)
		m_isHPPaulse = FALSE;
	else if (m_fTickTimeAcc < 0.3f)
		m_isHPPaulse = TRUE;

	if (m_isHPPaulse)
	{
		m_fTickTimeAcc += fTimeDelta;
	}
	else
	{
		m_fTickTimeAcc -= fTimeDelta;
	}

	m_vColorTint.x *= m_fTickTimeAcc;
}

CUIProgress_Bar* CUIProgress_Bar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIProgress_Bar* pInstance = new CUIProgress_Bar(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIProgress_Bar::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIProgress_Bar::Clone(void* pArg)
{
	CUIProgress_Bar* pInstance = new CUIProgress_Bar(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIProgress_Bar::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIProgress_Bar::Free()
{
	Super::Free();
}
