#include "pch.h"
#include "UIMonsterStat_Progress.h"
#include "Client_Defines.h"
#include "Client_EventDefine.h"
//=================
// Component
//=================
#include "Canvas.h"
#include "WorldUI_Component.h"	
#include "MyStat.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
#include "UI_Manager.h"
#include "GameInstance.h"

CUIMonsterStat_Progress::CUIMonsterStat_Progress(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CUIProgress_Bar(pDevice, pDeviceContext)
{
}

CUIMonsterStat_Progress::CUIMonsterStat_Progress(const CUIMonsterStat_Progress& rhs)
	:CUIProgress_Bar(rhs)
{
}

HRESULT CUIMonsterStat_Progress::Initialize_Prototype()
{
	if (FAILED(Super::Initialize_Prototype()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Progress::Initialize(void* pArg)
{
	MONSTER_STAT_PROGRESS_DESC* pDesc = static_cast<MONSTER_STAT_PROGRESS_DESC*>(pArg);
	if (FAILED(Super::Initialize(pArg)))
		return E_FAIL;
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CUIMonsterStat_Progress::Awake(const _uint iCurrentLevelID)
{
	if (FAILED(Super::Awake(iCurrentLevelID)))
		return E_FAIL;

	if (FAILED(Attach_Personal_Info()))
		return E_FAIL;

	return S_OK;
}

void CUIMonsterStat_Progress::Update_Priority(const _float fTimeDelta)
{
	Super::Update_Priority(fTimeDelta);
}

void CUIMonsterStat_Progress::Update(const _float fTimeDelta)
{
	Super::Update(fTimeDelta);

	m_fMonsterHPTimeAcc += fTimeDelta;

	// CurRatio 갱신
	Convert_Stat_To_Ratio();
}

void CUIMonsterStat_Progress::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);

	if (m_pWorldUIComp->Get_ScaleOffset() < 0.4f || m_fMonsterHPTimeAcc > 3.f)
	{
		m_isVisible = false;
		m_pParentCanvasCache->Get_CommonParam_bool_Ref()[0] = true;
	}
	else
	{
		m_isVisible = true;
		m_pParentCanvasCache->Get_CommonParam_bool_Ref()[0] = false;
	}
}

void CUIMonsterStat_Progress::Ready_Before_Render(const _float fTimeDelta)
{
	Super::Ready_Before_Render(fTimeDelta);
}

HRESULT CUIMonsterStat_Progress::Render()
{
	if (!m_isVisible)
		return S_OK;
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	if (FAILED(Super::Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Progress::Ready_Components(MONSTER_STAT_PROGRESS_DESC* pDesc)
{
	if (FAILED(Super::Ready_Components(pDesc)))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Progress::Bind_ShaderResources()
{
	CShader* pShader = Get_Component<CShader>();
	if (FAILED(Get_Component<CTransform>()->Bind_ShaderResource(pShader)))
		return E_FAIL;
	if (FAILED(Super::Bind_ShaderResources()))
		return E_FAIL;
	return S_OK;
}

HRESULT CUIMonsterStat_Progress::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_vOriginColor = m_vColorTint;
	m_vOriginGradiantColor = m_vGradiantColorTint;

	if (m_isSpawned)
	{
		m_isVisible = false;
		m_fAlpha_Ratio = 1.f;
		m_fMonsterHPTimeAcc = 100.f;
		m_isSpawned = false;
		m_fCurRatio = m_pTargetStat->Get_HealthRatio();
		m_fPreMonsterHPRatio = m_fCurRatio;
	}
	return S_OK;
}

void CUIMonsterStat_Progress::Initialize_Visible_Event()
{
	m_isActive		= false;
	m_isFin_Event	= false;
	
	Ready_Fade(0.5f, 0.f, 1.f, m_fDelay);

}

void CUIMonsterStat_Progress::Initialize_InVisible_Event()
{
	m_isActive = false;
	m_isFin_Event	= false;
	Ready_Fade(1.f, 1.f, 0.f, 0.f);
}

_bool CUIMonsterStat_Progress::Tick_Visible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);
	if (isFade)
	{
		m_isFin_Event = true;
		m_isActive = true;
		return true;
	}

	return false;
}

_bool CUIMonsterStat_Progress::Tick_InVisible_Event(const _float fTimeDelta)
{
	_bool isFade = Tick_Fade(fTimeDelta);

	if (isFade)
	{
		Request_SetDead();
		m_isActive = false;
		m_isFin_Event = true;
		return true;
	}
	return false;
}

HRESULT CUIMonsterStat_Progress::Spawn_FromPool(void* pArg)
{
	if (FAILED(Super::Spawn_FromPool(pArg)))
		return E_FAIL;

	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);
	if (auto* pNamePlate= std::get_if<UI_NAMEPLATE_PREFAB_DATA>(&pDesc->Data))
	{
		auto* pComp = Get_Script_Component(L"WorldUIComponent");
		if (nullptr == pComp)
			return E_FAIL;

		m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
		if (nullptr == m_pWorldUIComp)
			return E_FAIL;

		m_pWorldUIComp->Set_Target(pNamePlate->pTarget);
		m_pTargetMoster = pNamePlate->pTarget;
		m_pWorldUIComp->Set_TargetWorldOffset(pNamePlate->vOffset);
		m_pWorldUIComp->Set_TargetBoneName(pNamePlate->strTargetBoneName);

		/* 몬스터 스탯 컴포넌트 부착 */
		m_pTargetStat = pNamePlate->pTarget->Get_Component<CMyStat>();
		if (nullptr == m_pTargetStat)
			return E_FAIL;
	}

	m_isSpawned = true;


	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::MONSTER_HP:
		m_fProgress_Ratio = m_pTargetStat->Get_HealthRatio();
		m_fCurRatio = m_pTargetStat->Get_HealthRatio();
		m_fPreMonsterHPRatio = m_fCurRatio;
		break;
	case DTO::EUISubClassType::MONSTER_ARMOR:
		m_fProgress_Ratio = 0.f;
		m_fCurRatio = 0.f;
		m_fPreMonsterHPRatio = m_fCurRatio;
		break;
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}

	m_isDeadRequest = false;
	return S_OK;
}

HRESULT CUIMonsterStat_Progress::Despawn_FromPool()
{
	if (FAILED(Super::Despawn_FromPool()))
		return E_FAIL;

	m_isVisible = false;
	m_isVisibleTrigger = false;
	m_isPreVisible = false;
	return S_OK;
}

void CUIMonsterStat_Progress::Bind_Events()
{
	Super::Bind_Events();

	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::MENU_CLOSE == Desc.eEventID)
				{
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
		m_pGameInstance->Subscribe<MONSTER_DEAD_EVENT_START>([this](CGameObject* pDead)
			{
				if (pDead == m_pTargetMoster)
					this->Set_Invisible();
			}));


	// 대화 Event
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_BEGIN>([this](_int iId)
			{
				m_isVisible = false;
			}));
	m_vecEventHandles.push_back(
		m_pGameInstance->Subscribe<DIALOGUE_END>([this]()
			{					
				m_isVisible = true;
			}));

	// 패널 Events
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_START == Desc.eEventID)
				{
					m_isVisible = false;
				}
			}));
	m_vecEventHandles.push_back(
		m_pUIManager->Get_UIEvents().Subscribe([this](const UIEVENT_DESC& Desc)
			{
				if (EUIEventID::TUTORIAL_PANNEL_END == Desc.eEventID)
				{
					m_isVisible = true;
				}
			}));
}

HRESULT CUIMonsterStat_Progress::Convert_Stat_To_Ratio()
{
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::MONSTER_HP:
		m_fProgress_Ratio = m_pTargetStat->Get_HealthRatio();
		break;
	case DTO::EUISubClassType::MONSTER_ARMOR:
		m_fProgress_Ratio = 0.f;
		break;
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}
	if (fabsf(m_fPreMonsterHPRatio - m_fProgress_Ratio) > 0.f)
	{
		m_fMonsterHPTimeAcc = 0.f;
	}

	m_fPreMonsterHPRatio = m_fProgress_Ratio;

	return S_OK;
}

CUIMonsterStat_Progress* CUIMonsterStat_Progress::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CUIMonsterStat_Progress* pInstance = new CUIMonsterStat_Progress(pDevice, pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("CUIMonsterStat_Progress::Create, Create Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CUIMonsterStat_Progress::Clone(void* pArg)
{
	CUIMonsterStat_Progress* pInstance = new CUIMonsterStat_Progress(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("CUIMonsterStat_Progress::Clone, Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUIMonsterStat_Progress::Free()
{
	Super::Free();
}
