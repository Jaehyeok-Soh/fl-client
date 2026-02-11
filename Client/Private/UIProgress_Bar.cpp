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
	switch (m_eOwnerType)
	{
	case DTO::EUIOwnerType::NONE_OWNER:
		return S_OK;
	case DTO::EUIOwnerType::PLAYER_HP:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUIOwnerType::PLAYER_ARMOR:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUIOwnerType::PLAYER_ENERGY:
	{
		m_pTargetStat;
		return S_OK;
	}
	case DTO::EUIOwnerType::END:
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
	if (m_pGameInstance->KeyButton_Down(DIK_1))
		m_fCurRatio = 0.1f;
	if (m_pGameInstance->KeyButton_Down(DIK_2))
		m_fCurRatio = 0.5f;
	if (m_pGameInstance->KeyButton_Down(DIK_3))
		m_fCurRatio = 0.9f;

	Super::Update_Priority(fTimeDelta);
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
	Acting_By_InteractState();
	Super::Ready_Before_Render(fTimeDelta);
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

HRESULT CUIProgress_Bar::Ready_Components(PROGRESS_BAR_DESC* pDesc)
{
	Super::Ready_Components(pDesc);
	return S_OK;
}

HRESULT CUIProgress_Bar::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	Super::Bind_ShaderResources();

	return S_OK;
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
