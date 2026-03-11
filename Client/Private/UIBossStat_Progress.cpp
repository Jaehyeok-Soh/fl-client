#include "pch.h"
#include "UIBossStat_Progress.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "StatCom_Boss.h"
#include "Client_EventDefine.h"
#include "WorldUI_Component.h"	
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIBossStat_Progress::CUIBossStat_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIProgress_Bar(pDevice, pDeviceContext)
{
}

CUIBossStat_Progress::CUIBossStat_Progress(const CUIBossStat_Progress& rhs)
	:CUIProgress_Bar(rhs)
{
}

HRESULT CUIBossStat_Progress::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Progress::Initialize(void* pArg)
{
	BOSS_STAT_PROGRESS_DESC* pDesc = static_cast<BOSS_STAT_PROGRESS_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIBossStat_Progress::Awake(const _uint iCurrentLevelID)
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

void CUIBossStat_Progress::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIBossStat_Progress::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// CurRatio °»½Å
	Convert_Stat_To_Ratio();
}

void CUIBossStat_Progress::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
	if (m_eSubClassType == DTO::EUISubClassType::BOSS_STAT_HP_PROGRESS)
	{
		if (m_fProgress_Ratio == 1.f)
		{
			m_vColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
			m_vGradiantColorTint = Vec4{ 1.f, 1.f, 1.f, 1.f };
		}
		else
		{
			m_vColorTint = m_vOriginColorTint;
			m_vGradiantColorTint = m_vOriginColorTint;
		}
	}
}

void CUIBossStat_Progress::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIBossStat_Progress::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Progress::Ready_Components(BOSS_STAT_PROGRESS_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Progress::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIBossStat_Progress::Attach_Personal_Info()
{
	m_pGameInstance->Subscribe<XIBILA_BOSS_UI_ON>([this]()
		{
			this->Set_Visible();
		});
	m_pGameInstance->Subscribe<XIBILA_BOSS_UI_OFF>([this]()
		{
			this->Set_Invisible();
		});

	return S_OK;
}

void CUIBossStat_Progress::Tick_By_Type(const _float fTimeDelta)
{
	if (DTO::EUISubClassType::BOSS_STAT_ARMOR_PROGRESS == m_eSubClassType)
	{
		_bool is = false;//m_pTargetStat->Is_Groggy();
		
		if (is)
		{
			if (!m_isBossGroggyTrigger)
			{
				m_pGameInstance->Broadcast<BOSS_GROGGY>();
			}
		}
		else
		{
			if (m_isBossGroggyTrigger)
			{
				m_isBossGroggyTrigger = false;
			}
		}
	}
}

void CUIBossStat_Progress::Initialize_Visible_Event()
{
	m_isActive		= false;
	m_isFin_Event	= false;

	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::BOSS_STAT_BEGIN:
		break;
	case DTO::EUISubClassType::BOSS_STAT_HP_PROGRESS:
	case DTO::EUISubClassType::BOSS_STAT_ARMOR_PROGRESS:
		Ready_LerpChange(1.5f, 0.f, 1.f, 0.3f, 0.f);
		break;
	case DTO::EUISubClassType::BOSS_STAT_END:
		break;
	case DTO::EUISubClassType::END:
	default:
		break;
	}
}

void CUIBossStat_Progress::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
}

_bool CUIBossStat_Progress::Tick_Visible_Event(const _float fTimeDelta)
{
	return Tick_LerpChange(&m_fProgress_Ratio ,fTimeDelta);
}

_bool CUIBossStat_Progress::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	return true;
}

HRESULT CUIBossStat_Progress::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	if (auto* pBossNamePlate = std::get_if<UI_BOSS_NAMEPLATE_PREFAB_DATA>(&pDesc->Data))
	{
		m_pTargetStat = static_cast<CStatCom_Boss*>(pBossNamePlate->pTarget->Get_Component<CMyStat>());
		if (nullptr == m_pTargetStat)
			return E_FAIL;
	}

	m_isSpawned = true;
	m_fProgress_Ratio = 0.f;
	m_vOriginColorTint = m_vColorTint;
	return S_OK;
}

HRESULT CUIBossStat_Progress::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible			= false;
	m_isPreVisible		= false;
	m_isVisibleTrigger	= false;
	return S_OK;
}

HRESULT CUIBossStat_Progress::Convert_Stat_To_Ratio()
{
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::BOSS_STAT_BEGIN:
		break;
	case DTO::EUISubClassType::BOSS_STAT_HP_PROGRESS:
		m_fCurRatio = m_pTargetStat->Get_HealthRatio();
		break;
	case DTO::EUISubClassType::BOSS_STAT_ARMOR_PROGRESS:
		m_fCurRatio = m_pTargetStat->Get_CurrentGRoggyRatio();
		break;
	case DTO::EUISubClassType::BOSS_STAT_END:
		break;
	}
	return S_OK;
}

CUIBossStat_Progress* CUIBossStat_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIBossStat_Progress* pInstance = new CUIBossStat_Progress(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIBossStat_Progress::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIBossStat_Progress::Clone(void* pArg)
{
	CUIBossStat_Progress* pInstance = new CUIBossStat_Progress(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIBossStat_Progress::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIBossStat_Progress::Free()
{
	Super::Free();
}
