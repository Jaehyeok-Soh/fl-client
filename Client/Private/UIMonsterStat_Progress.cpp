#include "pch.h"
#include "UIMonsterStat_Progress.h"
#include "Client_Defines.h"

//=================
// Component
//=================
#include "WorldUI_Component.h"	
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect_Tex.h"
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

HRESULT CUIMonsterStat_Progress::Attach_Personal_Info()
{
	CGameObject* pResult = m_pGameInstance->Get_GameObject_Front(ENUM_TO_UINT(ELevelType::STATIC), g_wszPlayerLayer);
	if (nullptr == pResult)
		return E_FAIL;

	m_vOriginColor = m_vColorTint;
	m_vOriginGradiantColor = m_vGradiantColorTint;
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

	// CurRatio °»½Å
	Convert_Stat_To_Ratio();
}

void CUIMonsterStat_Progress::Update_Late(const _float fTimeDelta)
{
	Super::Update_Late(fTimeDelta);
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

void CUIMonsterStat_Progress::OnUIEvent(ETriggerEventType eEvent, CGenericUI* pSender)
{
	if (!m_isActive)
		return;
}

void CUIMonsterStat_Progress::Initialize_Visible_Event()
{
	m_isActive		= false;
	m_isFin_Event	= false;
	m_fTimeAcc		= 0.f;
	m_fAlpha_Ratio	= 0.f;
}

void CUIMonsterStat_Progress::Initialize_InVisible_Event()
{
	m_isFin_Event	= false;
	m_fTimeAcc		= 0.f;
}

_bool CUIMonsterStat_Progress::Tick_Visible_Event(const _float fTimeDelta)
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

_bool CUIMonsterStat_Progress::Tick_InVisible_Event(const _float fTimeDelta)
{
	m_isFin_Event = true;
	return true;
}

HRESULT CUIMonsterStat_Progress::Spawn_FromPool(void* pArg)
{
	UI_PREFAB_DATA* pDesc = static_cast<UI_PREFAB_DATA*>(pArg);

	auto* pComp = Get_Script_Component(L"WorldUIComponent");
	if (nullptr == pComp)
		return E_FAIL;

	m_pWorldUIComp = static_cast<CWorldUI_Component*>(pComp);
	if (nullptr == m_pWorldUIComp)
		return E_FAIL;

	m_pWorldUIComp->Set_Target(pDesc->pTarget);
	/* ¸ó½ºÅÍ ½ºÅÈ ÄÄÆ÷³ÍÆ® ºÎÂø */

	m_bDead = false;
	return S_OK;
}

HRESULT CUIMonsterStat_Progress::Despawn_FromPool()
{
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

HRESULT CUIMonsterStat_Progress::Convert_Stat_To_Ratio()
{
	switch (m_eSubClassType)
	{
	case DTO::EUISubClassType::MONSTER_HP:break;
	case DTO::EUISubClassType::MONSTER_ARMOR:break;
	case DTO::EUISubClassType::END:
	default:
		return E_FAIL;
	}
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
