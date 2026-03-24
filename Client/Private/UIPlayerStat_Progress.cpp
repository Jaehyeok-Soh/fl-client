#include "pch.h"
#include "UIPlayerStat_Progress.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "StatCom_Player.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "MyStat.h"
#include "GameInstance.h"
#include <UI_Manager.h>

CUIPlayerStat_Progress::CUIPlayerStat_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIProgress_Bar(pDevice, pDeviceContext)
{
}

CUIPlayerStat_Progress::CUIPlayerStat_Progress(const CUIPlayerStat_Progress& rhs)
	:CUIProgress_Bar(rhs)
{
}

HRESULT CUIPlayerStat_Progress::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Progress::Initialize(void* pArg)
{
	PLAYER_STAT_PROGRESS_DESC* pDesc = static_cast<PLAYER_STAT_PROGRESS_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPlayerStat_Progress::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIPlayerStat_Progress::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
	if (m_eSubClassType == DTO::EUISubClassType::PLAYER_HP)
	{
		if (m_fProgress_Ratio < 0.3f)
		{
			if (!m_isStartLowHp)
			{
				m_isStartLowHp = TRUE;
				UIEVENT_DESC Desc = {};
				Desc.eEventID = EUIEventID::PLAYER_LOW_HP;
				m_pUIManager->Get_UIEvents().Broadcast(Desc);

				m_isEndLowHp = FALSE;
				m_fTickTimeAcc = 1.f;
			}
		}
		else
		{
			if (!m_isEndLowHp)
			{
				m_isStartLowHp = FALSE;
				UIEVENT_DESC Desc = {};
				Desc.eEventID = EUIEventID::PLAYER_NORMAL_HP;
				m_pUIManager->Get_UIEvents().Broadcast(Desc);

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

void CUIPlayerStat_Progress::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	// CurRatio 갱신
	Convert_Stat_To_Ratio();
}

void CUIPlayerStat_Progress::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
}

void CUIPlayerStat_Progress::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIPlayerStat_Progress::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

void CUIPlayerStat_Progress::Initialize_Visible_Event()
{
	m_isActive = false;
	m_isFin_Event = false;
	m_fTimeAcc = 0.f;
	m_fAlpha_Ratio = 0.f;
}

void CUIPlayerStat_Progress::Initialize_InVisible_Event()
{
	m_isFin_Event = false;
	m_fTimeAcc = 0.f;
}

_bool CUIPlayerStat_Progress::Tick_Visible_Event(const _float fTimeDelta)
{
	m_fAlpha_Ratio += fTimeDelta * 2.f;
	if (m_fAlpha_Ratio >= 1.f)
	{
		m_fAlpha_Ratio = 1.f;
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}
	return false;
}

_bool CUIPlayerStat_Progress::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	Set_Active(false);
	return true;
}

void CUIPlayerStat_Progress::Bind_Events()
{
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
					Set_Active(true);
					this->Set_Visible();
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_OPEN == Desc.eEventID)
				{
					this->Set_Invisible();
				}
			}));

	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_START>(
			[this]()
			{
				this->Set_Invisible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<CINEMATIC_END>(
			[this]()
			{
				Set_Active(true);
				this->Set_Visible();
			}));


	// 대화 Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				Set_Invisible();
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{				
				Set_Active(true);
				Set_Visible();
			}));

	// 패널 Events
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					Set_Invisible();
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					Set_Active(true);
					Set_Visible();
				}
			}));
}

HRESULT CUIPlayerStat_Progress::Ready_Components(PLAYER_STAT_PROGRESS_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIPlayerStat_Progress::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;

	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUIPlayerStat_Progress::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_pPlayerStatCom = static_cast<CStatCom_Player*>(pResult->Get_Component<CMyStat>());
	if (nullptr == m_pPlayerStatCom)
		return E_FAIL;

	m_vOriginColor = m_vColorTint;
	m_vOriginGradiantColor = m_vGradiantColorTint;
	return S_OK;
}

HRESULT CUIPlayerStat_Progress::Convert_Stat_To_Ratio()
{
	_float f = {};
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::NONE_OWNER:
		break;
	case DTO::EUISubClassType::PLAYER_HP:
		f = m_pPlayerStatCom->Get_HealthRatio();
		break;
	case DTO::EUISubClassType::PLAYER_ARMOR:
		f = m_pPlayerStatCom->Get_Rate(CMyStat::STAT_TYPE::DEFENSE);
		break;
	case DTO::EUISubClassType::PLAYER_ENERGY:
		f = m_pPlayerStatCom->Get_Rate(CMyStat::STAT_TYPE::MENTAL);
		break;
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}

	if ((f - m_fPlayer_PreRatio) > 0.f)
	{
		m_fProgress_Ratio = f;
	}
	else
	{
		m_fCurRatio = f;
	}

	m_fPlayer_PreRatio = f;
	return S_OK;
}

void CUIPlayerStat_Progress::Low_HP(const _float fTimeDelta)
{
	m_vColorTint = Vec4{ 1.f, 0.f, 0.f, 1.f };
	m_vGradiantColorTint = Vec4{ 1.f, 0.f, 0.f, 1.f };

	if (m_fTickTimeAcc >= 1.f)
		m_isHPPulse = FALSE;
	else if (m_fTickTimeAcc < 0.3f)
		m_isHPPulse = TRUE;

	if (m_isHPPulse)
	{
		m_fTickTimeAcc += fTimeDelta;
	}
	else
	{
		m_fTickTimeAcc -= fTimeDelta;
	}

	m_vColorTint.x *= m_fTickTimeAcc;
}

CUIPlayerStat_Progress* CUIPlayerStat_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIPlayerStat_Progress* pInstance = new CUIPlayerStat_Progress(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIPlayerStat_Progress::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIPlayerStat_Progress::Clone(void* pArg)
{
	CUIPlayerStat_Progress* pInstance = new CUIPlayerStat_Progress(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIPlayerStat_Progress::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIPlayerStat_Progress::Free()
{
	Super::Free();
}
