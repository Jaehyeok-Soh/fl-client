#include "pch.h"
#include "UIBossStat_Text.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"

//=================
// Component
//=================
#include "MyStat.h"
#include "WorldUI_Component.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "GameInstance.h"

CUIBossStat_Text::CUIBossStat_Text(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIText(pDevice, pDeviceContext)
{
}

CUIBossStat_Text::CUIBossStat_Text(const CUIBossStat_Text& rhs)
	:CUIText(rhs)
{
}

HRESULT CUIBossStat_Text::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Text::Initialize(void* pArg)
{
	BOSS_STAT_TEXT_DESC* pDesc = static_cast<BOSS_STAT_TEXT_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Text::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	if (m_isSpawned)
	{
		Set_Invisible();
		m_isSpawned = false;
	}
	return S_OK;
}


void CUIBossStat_Text::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossStat_Text::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);
}

void CUIBossStat_Text::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIBossStat_Text::Ready_Before_Render(const _float fTimeDelta)
{
	if (FAILED(Convert_Stat_To_Text()))
		return;

	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIBossStat_Text::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Text::Ready_Components(BOSS_STAT_TEXT_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Text::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Text::Attach_Personal_Info()
{
	m_pGameInstance->Subscribe<BOSS_STAGING_EVENT_START>([this]() { this->Set_Invisible(); });
	m_pGameInstance->Subscribe<BOSS_STAGING_EVENT_END>([this]() { this->Set_Visible(); });

	return S_OK;
}

HRESULT CUIBossStat_Text::Convert_Stat_To_Text()
{
	switch (m_eTextSubClassType)
	{
	case DTO::EUITextSubClassType::BOSS_STAT_TEXT_LV:
		break;
	case DTO::EUITextSubClassType::BOSS_STAT_TEXT_NICKNAME:
		m_wstrText =Engine_Utils::ToWString( m_pTargetStat->Get_Owner()->Get_Name());
		break;

		break;
	}
	return S_OK;
}

void CUIBossStat_Text::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIBossStat_Text::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;

	Ready_Fade_Text(1.f, 0.f, 1.f, m_fDelay);
}

void CUIBossStat_Text::Initialize_InVisible_Event()
{
}

_bool CUIBossStat_Text::Tick_Visible_Event(const _float fTimeDelta)
{
	if (Tick_Fade_Text(fTimeDelta))
	{
		m_isActive = true;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

_bool CUIBossStat_Text::Tick_InVisible_Event(const _float fTimeDelta)
{
	return true;
}

HRESULT CUIBossStat_Text::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	m_pTargetStat = pDesc->pTarget->Get_Component<CMyStat>();

	if (nullptr == m_pTargetStat)
		return E_FAIL;
	/* ¸ó½ºÅÍ ½ºÅÈ ÄÄÆ÷³ÍÆ® ºÎÂø */
	m_isSpawned = true;
	return S_OK;
}

HRESULT CUIBossStat_Text::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

CUIBossStat_Text* CUIBossStat_Text::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossStat_Text* pInstance = new CUIBossStat_Text(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossStat_Text::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossStat_Text::Clone(void* pArg)
{
	CUIBossStat_Text* pInstance = new CUIBossStat_Text(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossStat_Text::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossStat_Text::Free()
{
	Super::Free();
}
